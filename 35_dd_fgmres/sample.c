/* XianzhiKrylov sample 35: Double-Double A, b, solve, and x. */
#include "sample_common.h"
#include <stdio.h>

static int xk_verify_dd(
    const xk_matrix *a,
    const double *values_high,
    const double *values_low,
    const double *rhs_high,
    const double *rhs_low,
    const double *solution_high,
    const double *solution_low,
    long double tolerance)
{
    uint64_t i;
    int32_t k;
    long double residual_norm = 0.0L;
    long double rhs_norm = 0.0L;
    long double maximum = 0.0L;

    for (i = 0; i < a->h.rows; ++i) {
        long double residual =
            (long double)rhs_high[i] + (long double)rhs_low[i];
        const long double component = fabsl(residual);

        for (k = a->row[i]; k < a->row[i + 1]; ++k) {
            const uint64_t column = (uint64_t)a->col[k];
            const long double value =
                (long double)values_high[k] + (long double)values_low[k];
            const long double x =
                (long double)solution_high[column] +
                (long double)solution_low[column];
            residual -= value * x;
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
        printf("Independent DD residual: "
               "relative_l2=%.12Le absmax=%.12Le %s\n",
               relative, maximum, pass ? "PASS" : "FAIL");
        return pass;
    }
}

int main(int argc, char **argv)
{
    const char *matrix_path =
        argc > 1 ? argv[1] : "../matrices/diffusion_1m.csrpat";
    const char *json_path = argc > 2 ? argv[2] : "config.json";
    xk_matrix matrix = {0};
    xianzhikrylov_matrix_data_t matrix_data = {0};
    xianzhikrylov_matrix_t matrix_object = NULL;
    xianzhikrylov_vector_t rhs_object = NULL;
    xianzhikrylov_vector_t solution_object = NULL;
    xianzhikrylov_handle_t handle = NULL;
    double *values_high = NULL, *values_low = NULL;
    double *rhs_high = NULL, *rhs_low = NULL;
    double *solution_high = NULL, *solution_low = NULL;
    xianzhikrylov_status_t status;
    int rc = 2, destroyed = 0;

    if (!xk_matrix_load(&matrix, matrix_path))
        goto done;
    if (matrix.h.scalar != 1 || matrix.h.rhs_count != 1) {
        fprintf(stderr, "DD sample requires a real, single-RHS matrix\n");
        goto done;
    }
    if (!xk_size_ok(matrix.h.entries, sizeof(double)) ||
        !xk_size_ok(matrix.h.rows, sizeof(double))) {
        fprintf(stderr, "DD sample is too large\n");
        goto done;
    }

    values_high =
        (double *)malloc((size_t)matrix.h.entries * sizeof(double));
    values_low =
        (double *)malloc((size_t)matrix.h.entries * sizeof(double));
    rhs_high = (double *)calloc((size_t)matrix.h.rows, sizeof(double));
    rhs_low = (double *)calloc((size_t)matrix.h.rows, sizeof(double));
    solution_high =
        (double *)calloc((size_t)matrix.h.rows, sizeof(double));
    solution_low =
        (double *)calloc((size_t)matrix.h.rows, sizeof(double));
    if (!values_high || !values_low || !rhs_high || !rhs_low ||
        !solution_high || !solution_low) {
        fprintf(stderr, "cannot allocate DD buffers\n");
        goto done;
    }

    {
        uint64_t i, row;
        for (i = 0; i < matrix.h.entries; ++i) {
            const double high = matrix.a[i];
            const double low =
                (double)(((long double)matrix.a[i] * 0x1.0p-54L) *
                         (0.5L + xk_unit(matrix.seed, matrix.model_id,
                                         i, UINT64_C(0x71), UINT64_C(0x52))));
            values_high[i] = high;
            values_low[i] = low;
        }
        for (row = 0; row < matrix.h.rows; ++row) {
            long double sum = 0.0L;
            int32_t k;
            for (k = matrix.row[row]; k < matrix.row[row + 1]; ++k) {
                sum += (long double)values_high[k] +
                       (long double)values_low[k];
            }
            rhs_high[row] = (double)sum;
            rhs_low[row] =
                (double)(sum - (long double)rhs_high[row]);
        }
    }

    matrix_data.abi_version = XIANZHIKRYLOV_ABI_VERSION;
    matrix_data.struct_size = sizeof(matrix_data);
    matrix_data.format = XIANZHIKRYLOV_CSR;
    matrix_data.value_type = XIANZHIKRYLOV_DD;
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
        xk_input(values_high, (size_t)matrix.h.entries * sizeof(double));
    matrix_data.values_low =
        xk_input(values_low, (size_t)matrix.h.entries * sizeof(double));

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
        &rhs_object, (int64_t)matrix.h.rows, 1,
        (int64_t)matrix.h.rows, XIANZHIKRYLOV_RHS_MAJOR,
        rhs_high, rhs_low, XIANZHIKRYLOV_DD, XIANZHIKRYLOV_HOST);
    if (status != XIANZHIKRYLOV_SUCCESS) {
        fprintf(stderr, "rhs vector_create status=%d\n", (int)status);
        rc = 1;
        goto done;
    }
    status = xianzhikrylov_multiple_vector_create(
        &solution_object, (int64_t)matrix.h.rows, 1,
        (int64_t)matrix.h.rows, XIANZHIKRYLOV_RHS_MAJOR,
        solution_high, solution_low, XIANZHIKRYLOV_DD,
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
        const int residual_ok = xk_verify_dd(
            &matrix, values_high, values_low, rhs_high, rhs_low,
            solution_high, solution_low, 1e-10L);
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
    free(solution_low);
    free(solution_high);
    free(rhs_low);
    free(rhs_high);
    free(values_low);
    free(values_high);
    xk_matrix_free(&matrix);
    return rc;
}
