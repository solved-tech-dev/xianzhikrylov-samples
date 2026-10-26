/* XianzhiKrylov sample 33: native FP32 FGMRES + Jacobi. */
#include "sample_common.h"
#include <stdio.h>

#ifndef XK_SAMPLE_BACKEND
#error "Build with -DXK_SAMPLE_BACKEND=<backend value>"
#endif

static int xk_verify_float(const xk_matrix *a, const float *values,
                           const float *rhs, const float *solution,
                           long double tolerance)
{
    uint32_t r;
    int all = 1;

    for (r = 0; r < a->h.rhs_count; ++r) {
        long double residual_norm = 0.0L;
        long double rhs_norm = 0.0L;
        long double maximum = 0.0L;
        uint64_t i;

        for (i = 0; i < a->h.rows; ++i) {
            const uint64_t q = (uint64_t)r * a->h.rows + i;
            long double residual = (long double)rhs[q];
            const long double component = fabsl(residual);
            int32_t k;

            for (k = a->row[i]; k < a->row[i + 1]; ++k) {
                const uint64_t p =
                    (uint64_t)r * a->h.rows + (uint64_t)a->col[k];
                residual -= (long double)values[k] * solution[p];
            }
            residual = fabsl(residual);
            residual_norm += residual * residual;
            rhs_norm += component * component;
            if (residual > maximum) maximum = residual;
        }

        {
            const long double relative = rhs_norm > 0.0L
                ? sqrtl(residual_norm / rhs_norm)
                : sqrtl(residual_norm);
            const int pass = relative <= tolerance;
            printf("Independent FP32 residual RHS %u: "
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
    xianzhikrylov_system_data system = {0};
    xianzhikrylov_solution output = {0};
    xianzhikrylov_handle *handle = NULL;
    float *values = NULL, *rhs = NULL, *solution = NULL;
    uint64_t vector_count = 0;
    xianzhikrylov_status status;
    int rc = 2, destroyed = 0;

    if (!xk_matrix_load(&matrix, matrix_path))
        goto done;
    if (matrix.h.scalar != 1 ||
        !xk_size_ok(matrix.h.entries, sizeof(float)) ||
        !xk_size_ok(matrix.h.rows * matrix.h.rhs_count, sizeof(float))) {
        fprintf(stderr, "FP32 sample requires a real matrix\n");
        goto done;
    }

    vector_count = matrix.h.rows * matrix.h.rhs_count;
    values = (float *)malloc((size_t)matrix.h.entries * sizeof(float));
    rhs = (float *)malloc((size_t)vector_count * sizeof(float));
    solution = (float *)calloc((size_t)vector_count, sizeof(float));
    if (!values || !rhs || !solution) {
        fprintf(stderr, "cannot allocate FP32 buffers\n");
        goto done;
    }
    {
        uint64_t i;
        for (i = 0; i < matrix.h.entries; ++i)
            values[i] = (float)matrix.a[i];
        for (i = 0; i < vector_count; ++i)
            rhs[i] = (float)matrix.b[i];
    }

    system.abi_version = XIANZHIKRYLOV_DATA_ABI;
    system.struct_size = sizeof(system);
    system.matrix.abi_version = XIANZHIKRYLOV_DATA_ABI;
    system.matrix.struct_size = sizeof(system.matrix);
    system.matrix.format = XIANZHIKRYLOV_CSR;
    system.matrix.scalar_type = XIANZHIKRYLOV_FP32;
    system.matrix.index_type = XIANZHIKRYLOV_INDEX_I32;
    system.matrix.block_size = 1;
    system.matrix.block_layout = XIANZHIKRYLOV_BLOCK_ROW_MAJOR;
    system.matrix.memory_space = XIANZHIKRYLOV_HOST;
    system.matrix.device_id = -1;
    system.matrix.rows = matrix.h.rows;
    system.matrix.columns = matrix.h.rows;
    system.matrix.entries = matrix.h.entries;
    system.matrix.row_offsets =
        xk_input(matrix.row, (size_t)(matrix.h.rows + 1) * sizeof(int32_t));
    system.matrix.column_indices =
        xk_input(matrix.col, (size_t)matrix.h.entries * sizeof(int32_t));
    system.matrix.values =
        xk_input(values, (size_t)matrix.h.entries * sizeof(float));
    system.rhs.abi_version = XIANZHIKRYLOV_DATA_ABI;
    system.rhs.struct_size = sizeof(system.rhs);
    system.rhs.scalar_type = XIANZHIKRYLOV_FP32;
    system.rhs.layout = XIANZHIKRYLOV_RHS_MAJOR;
    system.rhs.memory_space = XIANZHIKRYLOV_HOST;
    system.rhs.device_id = -1;
    system.rhs.rows = matrix.h.rows;
    system.rhs.rhs_count = matrix.h.rhs_count;
    system.rhs.leading_dimension = matrix.h.rows;
    system.rhs.data =
        xk_input(rhs, (size_t)vector_count * sizeof(float));

    status = xianzhikrylov_init(
        XK_SAMPLE_BACKEND, 0, json_path, &handle);
    if (status != XIANZHIKRYLOV_SUCCESS) {
        fprintf(stderr, "init status=%d\n", (int)status);
        rc = 1;
        goto done;
    }
    status = xianzhikrylov_set_system(
        handle, XIANZHIKRYLOV_STRUCTURE | XIANZHIKRYLOV_RHS, &system);
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
    output.scalar_type = XIANZHIKRYLOV_FP32;
    output.layout = XIANZHIKRYLOV_RHS_MAJOR;
    output.memory_space = XIANZHIKRYLOV_HOST;
    output.device_id = -1;
    output.rows = matrix.h.rows;
    output.rhs_count = matrix.h.rhs_count;
    output.leading_dimension = matrix.h.rows;
    output.data.data = solution;
    output.data.bytes = (size_t)vector_count * sizeof(float);
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
            xk_verify_float(&matrix, values, rhs, solution, 1e-4L);
        destroyed = 1;
        rc = print_status == XIANZHIKRYLOV_SUCCESS &&
             destroy_status == XIANZHIKRYLOV_SUCCESS &&
             residual_ok ? 0 : 1;
    }

done:
    if (handle && !destroyed)
        (void)xianzhikrylov_destroy(&handle);
    free(solution);
    free(rhs);
    free(values);
    xk_matrix_free(&matrix);
    return rc;
}
