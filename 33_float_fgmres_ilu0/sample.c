/* XianzhiKrylov sample 33: float application storage, FGMRES + ILU(0). */
#include "sample_common.h"
#include <stdio.h>

#ifndef XK_SAMPLE_BACKEND
#error "Build with -DXK_SAMPLE_BACKEND=<backend value>"
#endif

static int xk_prepare_float_system(const xk_matrix *a,
                                   float **float_values,
                                   float **float_rhs,
                                   double **api_values,
                                   double **api_rhs,
                                   xk_binding *binding)
{
    const uint64_t vector_count = a->h.rows * a->h.rhs_count;
    uint64_t i;

    if (a->h.scalar != 1 ||
        !xk_size_ok(a->h.entries, sizeof(float)) ||
        !xk_size_ok(vector_count, sizeof(float)) ||
        !xk_size_ok(a->h.entries, sizeof(double)) ||
        !xk_size_ok(vector_count, sizeof(double)))
        return 0;

    *float_values = (float *)malloc((size_t)a->h.entries * sizeof(float));
    *float_rhs = (float *)malloc((size_t)vector_count * sizeof(float));
    *api_values = (double *)malloc((size_t)a->h.entries * sizeof(double));
    *api_rhs = (double *)malloc((size_t)vector_count * sizeof(double));
    if (!*float_values || !*float_rhs || !*api_values || !*api_rhs)
        return 0;

    for (i = 0; i < a->h.entries; ++i) {
        (*float_values)[i] = (float)a->a[i];
        (*api_values)[i] = (double)(*float_values)[i];
    }
    for (i = 0; i < vector_count; ++i) {
        (*float_rhs)[i] = (float)a->b[i];
        (*api_rhs)[i] = (double)(*float_rhs)[i];
    }

    memset(binding, 0, sizeof(*binding));
    xk_init_matrix_descriptor(&binding->system.matrix, a);
    binding->system.matrix.format = XIANZHIKRYLOV_CSR;
    binding->system.matrix.row_offsets = xk_input(
        a->row, (size_t)(a->h.rows + 1) * sizeof(int32_t));
    binding->system.matrix.column_indices = xk_input(
        a->col, (size_t)a->h.entries * sizeof(int32_t));
    binding->system.matrix.values = xk_input(
        *api_values, (size_t)a->h.entries * sizeof(double));
    binding->system.abi_version = XIANZHIKRYLOV_DATA_ABI;
    binding->system.struct_size = sizeof(binding->system);
    xk_init_vector_descriptor(
        &binding->system.rhs, XIANZHIKRYLOV_FP64,
        a->h.rows, a->h.rhs_count,
        xk_input(*api_rhs, (size_t)vector_count * sizeof(double)));
    return 1;
}

static int xk_verify_float(const xk_matrix *a, const float *values,
                           const float *rhs, const float *solution,
                           long double tolerance)
{
    uint32_t r;
    int all = 1;
    for (r = 0; r < a->h.rhs_count; ++r) {
        long double rr = 0.0L, bb = 0.0L, maximum = 0.0L;
        uint64_t i;
        for (i = 0; i < a->h.rows; ++i) {
            const uint64_t q = (uint64_t)r * a->h.rows + i;
            long double residual = (long double)rhs[q];
            const long double rhs_norm = fabsl(residual);
            int32_t k;
            for (k = a->row[i]; k < a->row[i + 1]; ++k) {
                const uint64_t p =
                    (uint64_t)r * a->h.rows + (uint64_t)a->col[k];
                residual -= (long double)values[k] * solution[p];
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
            printf("Independent float residual RHS %u: "
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
        argc > 1 ? argv[1] : "data/transport_1m.csrpat";
    const char *json_path = argc > 2 ? argv[2] : "solver.json";
    xk_matrix matrix = {0};
    xk_binding binding = {0};
    xk_solution api_solution = {0};
    xianzhikrylov_handle *handle = NULL;
    float *float_values = NULL, *float_rhs = NULL, *float_solution = NULL;
    double *api_values = NULL, *api_rhs = NULL;
    uint64_t solution_count = 0;
    xianzhikrylov_status status;
    int rc = 2, destroyed = 0;

    if (!xk_matrix_load(&matrix, matrix_path))
        goto done;
    if (!xk_prepare_float_system(
            &matrix, &float_values, &float_rhs,
            &api_values, &api_rhs, &binding)) {
        fprintf(stderr, "cannot prepare float application buffers\n");
        goto done;
    }
    if (!xk_solution_init(&api_solution, &matrix)) {
        fprintf(stderr, "cannot allocate API solution\n");
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
    status = xianzhikrylov_solution_download(
        handle, &api_solution.descriptor);
    if (status != XIANZHIKRYLOV_SUCCESS) {
        fprintf(stderr, "solution_download status=%d\n", (int)status);
        rc = 1;
        goto done;
    }

    solution_count = matrix.h.rows * matrix.h.rhs_count;
    float_solution =
        (float *)malloc((size_t)solution_count * sizeof(float));
    if (!float_solution) {
        fprintf(stderr, "cannot allocate float solution\n");
        goto done;
    }
    {
        uint64_t i;
        for (i = 0; i < solution_count; ++i)
            float_solution[i] = (float)api_solution.x[i];
    }

    {
        const xianzhikrylov_status print_status =
            xianzhikrylov_print_info(handle);
        const xianzhikrylov_status destroy_status =
            xianzhikrylov_destroy(&handle);
        const int residual_ok = xk_verify_float(
            &matrix, float_values, float_rhs, float_solution, 1e-5L);
        destroyed = 1;
        rc = print_status == XIANZHIKRYLOV_SUCCESS &&
             destroy_status == XIANZHIKRYLOV_SUCCESS &&
             residual_ok ? 0 : 1;
    }

done:
    if (handle && !destroyed)
        (void)xianzhikrylov_destroy(&handle);
    free(float_solution);
    free(api_rhs);
    free(api_values);
    free(float_rhs);
    free(float_values);
    xk_solution_free(&api_solution);
    xk_binding_free(&binding);
    xk_matrix_free(&matrix);
    return rc;
}
