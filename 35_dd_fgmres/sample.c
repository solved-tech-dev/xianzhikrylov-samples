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
    xianzhikrylov_system_data system = {0};
    xianzhikrylov_solution output = {0};
    xianzhikrylov_handle *handle = NULL;
    double *values_high = NULL, *values_low = NULL;
    double *rhs_high = NULL, *rhs_low = NULL;
    double *solution_high = NULL, *solution_low = NULL;
    xianzhikrylov_status status;
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

    system.abi_version = XIANZHIKRYLOV_DATA_ABI;
    system.struct_size = sizeof(system);
    system.matrix.abi_version = XIANZHIKRYLOV_DATA_ABI;
    system.matrix.struct_size = sizeof(system.matrix);
    system.matrix.format = XIANZHIKRYLOV_CSR;
    system.matrix.scalar_type = XIANZHIKRYLOV_DD;
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
        xk_input(values_high, (size_t)matrix.h.entries * sizeof(double));
    system.matrix_planes.values_low =
        xk_input(values_low, (size_t)matrix.h.entries * sizeof(double));
    system.rhs.abi_version = XIANZHIKRYLOV_DATA_ABI;
    system.rhs.struct_size = sizeof(system.rhs);
    system.rhs.scalar_type = XIANZHIKRYLOV_DD;
    system.rhs.layout = XIANZHIKRYLOV_RHS_MAJOR;
    system.rhs.memory_space = XIANZHIKRYLOV_HOST;
    system.rhs.device_id = -1;
    system.rhs.rows = matrix.h.rows;
    system.rhs.rhs_count = 1;
    system.rhs.leading_dimension = matrix.h.rows;
    system.rhs.data =
        xk_input(rhs_high, (size_t)matrix.h.rows * sizeof(double));
    system.rhs.low =
        xk_input(rhs_low, (size_t)matrix.h.rows * sizeof(double));

    status = xianzhikrylov_init(0, json_path, &handle);
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
    output.scalar_type = XIANZHIKRYLOV_DD;
    output.layout = XIANZHIKRYLOV_RHS_MAJOR;
    output.memory_space = XIANZHIKRYLOV_HOST;
    output.device_id = -1;
    output.rows = matrix.h.rows;
    output.rhs_count = 1;
    output.leading_dimension = matrix.h.rows;
    output.data.data = solution_high;
    output.data.bytes = (size_t)matrix.h.rows * sizeof(double);
    output.low.data = solution_low;
    output.low.bytes = (size_t)matrix.h.rows * sizeof(double);
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
        (void)xianzhikrylov_destroy(&handle);
    free(solution_low);
    free(solution_high);
    free(rhs_low);
    free(rhs_high);
    free(values_low);
    free(values_high);
    xk_matrix_free(&matrix);
    return rc;
}
