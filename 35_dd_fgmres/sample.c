/* XianzhiKrylov sample 35: FP64 system with Double-Double solution. */
#include "sample_common.h"
#include <stdio.h>

#ifndef XK_SAMPLE_BACKEND
#error "Build with -DXK_SAMPLE_BACKEND=<backend value>"
#endif

static int xk_verify_dd(const xk_matrix *a, const double *high,
                        const double *low, long double tolerance)
{
    uint32_t r;
    int all = 1;
    for (r = 0; r < a->h.rhs_count; ++r) {
        long double rr = 0.0L, bb = 0.0L, maximum = 0.0L;
        uint64_t i;
        for (i = 0; i < a->h.rows; ++i) {
            const uint64_t q = (uint64_t)r * a->h.rows + i;
            long double residual = a->b[q];
            const long double rhs_norm = fabsl(residual);
            int32_t k;
            for (k = a->row[i]; k < a->row[i + 1]; ++k) {
                const uint64_t p =
                    (uint64_t)r * a->h.rows + (uint64_t)a->col[k];
                residual -= (long double)a->a[k] *
                    ((long double)high[p] + (long double)low[p]);
            }
            residual = fabsl(residual);
            rr += residual * residual;
            bb += rhs_norm * rhs_norm;
            if (residual > maximum) maximum = residual;
        }
        {
            const long double relative =
                bb > 0.0L ? sqrtl(rr / bb) : sqrtl(rr);
            const int pass = relative <= tolerance;
            printf("Independent DD residual RHS %u: "
                   "relative_l2=%.12Le absmax=%.12Le %s\n",
                   r + 1, relative, maximum, pass ? "PASS" : "FAIL");
            all = all && pass;
        }
    }
    return all;
}

int main(int argc, char **argv)
{
    const char *matrix_path =
        argc > 1 ? argv[1] : "data/diffusion_1m.csrpat";
    const char *json_path = argc > 2 ? argv[2] : "solver.json";
    xk_matrix matrix = {0};
    xk_binding binding = {0};
    xianzhikrylov_solution output = {0};
    xianzhikrylov_handle *handle = NULL;
    double *high = NULL, *low = NULL;
    uint64_t count = 0;
    xianzhikrylov_status status;
    int rc = 2, destroyed = 0;

    if (!xk_matrix_load(&matrix, matrix_path))
        goto done;
    if (matrix.h.scalar != 1 || matrix.h.rhs_count != 1) {
        fprintf(stderr, "DD sample requires a real, single-RHS system\n");
        goto done;
    }
    if (!xk_binding_make(&binding, &matrix, XIANZHIKRYLOV_CSR, 1)) {
        fprintf(stderr, "cannot prepare matrix format\n");
        goto done;
    }

    count = matrix.h.rows * matrix.h.rhs_count;
    high = (double *)calloc((size_t)count, sizeof(double));
    low = (double *)calloc((size_t)count, sizeof(double));
    if (!high || !low) {
        fprintf(stderr, "cannot allocate Double-Double solution\n");
        goto done;
    }

    status = xianzhikrylov_init(
        XK_SAMPLE_BACKEND, 0, json_path, &handle);
    if (status != XIANZHIKRYLOV_SUCCESS) {
        fprintf(stderr, "init status=%d\n", (int)status);
        rc = 1;
        goto done;
    }
    status = xianzhikrylov_set_system(
        handle, XIANZHIKRYLOV_STRUCTURE | XIANZHIKRYLOV_RHS,
        &binding.system);
    if (status != XIANZHIKRYLOV_SUCCESS) {
        fprintf(stderr, "set_system status=%d\n", (int)status);
        rc = 1;
        goto done;
    }
    status = xianzhikrylov_solve_prepare(
        handle, XIANZHIKRYLOV_AUTO_STAGE, NULL);
    if (status != XIANZHIKRYLOV_SUCCESS) {
        fprintf(stderr, "solve_prepare status=%d\n", (int)status);
        rc = 1;
        goto done;
    }
    status = xianzhikrylov_solve(handle);
    if (status != XIANZHIKRYLOV_CONVERGED) {
        fprintf(stderr, "solve status=%d (not converged)\n", (int)status);
        rc = 1;
        goto done;
    }

    output.abi_version = XIANZHIKRYLOV_DATA_ABI;
    output.struct_size = sizeof(output);
    output.scalar_type = XIANZHIKRYLOV_DD;
    output.layout = XIANZHIKRYLOV_RHS_MAJOR;
    output.memory_space = XIANZHIKRYLOV_HOST;
    output.device_id = -1;
    output.rows = matrix.h.rows;
    output.rhs_count = matrix.h.rhs_count;
    output.leading_dimension = matrix.h.rows;
    output.data.data = high;
    output.data.bytes = (size_t)count * sizeof(double);
    output.low.data = low;
    output.low.bytes = (size_t)count * sizeof(double);
    status = xianzhikrylov_solution_download(handle, &output);
    if (status != XIANZHIKRYLOV_SUCCESS) {
        fprintf(stderr, "solution_download status=%d\n", (int)status);
        rc = 1;
        goto done;
    }

    {
        const xianzhikrylov_status print_status =
            xianzhikrylov_print_info(handle);
        const xianzhikrylov_status destroy_status =
            xianzhikrylov_destroy(&handle);
        const int residual_ok =
            xk_verify_dd(&matrix, high, low, 1e-10L);
        destroyed = 1;
        rc = print_status == XIANZHIKRYLOV_SUCCESS &&
             destroy_status == XIANZHIKRYLOV_SUCCESS &&
             residual_ok ? 0 : 1;
    }

done:
    if (handle && !destroyed)
        (void)xianzhikrylov_destroy(&handle);
    free(low);
    free(high);
    xk_binding_free(&binding);
    xk_matrix_free(&matrix);
    return rc;
}
