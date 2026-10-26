/* XianzhiKrylov sample 33: native FP32 FGMRES + Jacobi. */
#include "sample_common.h"
#include <stdio.h>

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
        argc > 1 ? argv[1] : "../matrices/transport_1m.csrpat";
    const char *json_path = argc > 2 ? argv[2] : "config.json";
    xk_matrix matrix = {0};
    xianzhikrylov_matrix_data_t matrix_data = {0};
    xianzhikrylov_matrix_t matrix_object = NULL;
    xianzhikrylov_vector_t rhs_object = NULL;
    xianzhikrylov_vector_t solution_object = NULL;
    xianzhikrylov_handle_t handle = NULL;
    float *values = NULL, *rhs = NULL, *solution = NULL;
    uint64_t vector_count = 0;
    xianzhikrylov_status_t status;
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

    matrix_data.abi_version = XIANZHIKRYLOV_ABI_VERSION;
    matrix_data.struct_size = sizeof(matrix_data);
    matrix_data.format = XIANZHIKRYLOV_CSR;
    matrix_data.value_type = XIANZHIKRYLOV_FP32;
    matrix_data.index_type = XIANZHIKRYLOV_INDEX_I32;
    matrix_data.block_size = 1;
    matrix_data.block_layout = XIANZHIKRYLOV_BLOCK_ROW_MAJOR;
    matrix_data.memory_space = XIANZHIKRYLOV_HOST;
    matrix_data.device_id = -1;
    matrix_data.rows = (int64_t)matrix.h.rows;
    matrix_data.columns = (int64_t)matrix.h.rows;
    matrix_data.entries = (int64_t)matrix.h.entries;
    matrix_data.row_offsets =
        xk_input(matrix.row, (size_t)(matrix.h.rows + 1) * sizeof(int32_t));
    matrix_data.column_indices =
        xk_input(matrix.col, (size_t)matrix.h.entries * sizeof(int32_t));
    matrix_data.values =
        xk_input(values, (size_t)matrix.h.entries * sizeof(float));

    status = xianzhikrylov_create(0, &handle);
    if (status != XIANZHIKRYLOV_SUCCESS) {
        fprintf(stderr, "create status=%d\n", (int)status);
        rc = 1;
        goto done;
    }
    status = xianzhikrylov_config_set(
        handle, XIANZHIKRYLOV_CONFIG_FILE, json_path);
    if (status != XIANZHIKRYLOV_SUCCESS) {
        fprintf(stderr, "config_set status=%d\n", (int)status);
        rc = 1;
        goto done;
    }
    status = xianzhikrylov_matrix_create(&matrix_object, &matrix_data);
    if (status != XIANZHIKRYLOV_SUCCESS) {
        fprintf(stderr, "matrix_create status=%d\n", (int)status);
        rc = 1;
        goto done;
    }
    status = xianzhikrylov_multiple_vector_create(
        &rhs_object, (int64_t)matrix.h.rows,
        (int64_t)matrix.h.rhs_count, (int64_t)matrix.h.rows,
        XIANZHIKRYLOV_RHS_MAJOR, rhs, NULL, XIANZHIKRYLOV_FP32,
        XIANZHIKRYLOV_HOST);
    if (status != XIANZHIKRYLOV_SUCCESS) {
        fprintf(stderr, "rhs vector_create status=%d\n", (int)status);
        rc = 1;
        goto done;
    }
    status = xianzhikrylov_multiple_vector_create(
        &solution_object, (int64_t)matrix.h.rows,
        (int64_t)matrix.h.rhs_count, (int64_t)matrix.h.rows,
        XIANZHIKRYLOV_RHS_MAJOR, solution, NULL, XIANZHIKRYLOV_FP32,
        XIANZHIKRYLOV_HOST);
    if (status != XIANZHIKRYLOV_SUCCESS) {
        fprintf(stderr, "solution vector_create status=%d\n", (int)status);
        rc = 1;
        goto done;
    }
    status = xianzhikrylov_execute(
        handle, XIANZHIKRYLOV_PHASE_ALL, matrix_object,
        solution_object, rhs_object);
    if (status != XIANZHIKRYLOV_CONVERGED) {
        fprintf(stderr, "execute status=%d (not converged)\n", (int)status);
        rc = 1;
        goto done;
    }

    {
        const xianzhikrylov_status_t print_status =
            xianzhikrylov_print_info(handle);
        const xianzhikrylov_status_t destroy_status =
            xianzhikrylov_destroy(handle);
        handle = NULL;
        const int residual_ok =
            xk_verify_float(&matrix, values, rhs, solution, 1e-4L);
        destroyed = 1;
        rc = print_status == XIANZHIKRYLOV_SUCCESS &&
             destroy_status == XIANZHIKRYLOV_SUCCESS &&
             residual_ok ? 0 : 1;
    }

done:
    if (handle && !destroyed)
        (void)xianzhikrylov_destroy(handle);
    if (solution_object)
        (void)xianzhikrylov_vector_destroy(solution_object);
    if (rhs_object)
        (void)xianzhikrylov_vector_destroy(rhs_object);
    if (matrix_object)
        (void)xianzhikrylov_matrix_destroy(matrix_object);
    free(solution);
    free(rhs);
    free(values);
    xk_matrix_free(&matrix);
    return rc;
}
