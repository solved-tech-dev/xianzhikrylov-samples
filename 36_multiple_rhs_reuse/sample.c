/* Solve three FP64 right-hand sides and reuse the prepared matrix. */
#include "sample_common.h"

int main(int argc, char **argv)
{
    const char *matrix_path = argc > 1 ? argv[1] : "../matrices/transport_1m.csrpat";
    const char *config_path = argc > 2 ? argv[2] : "config.json";
    xk_matrix matrix = {0};
    xk_binding binding = {0};
    xk_solution solution = {0};
    xianzhikrylov_handle_t handle = NULL;
    xianzhikrylov_status_t status = XIANZHIKRYLOV_STATE_ERROR;
    int rc = 1;

    if (!xk_matrix_load(&matrix, matrix_path) ||
        !xk_matrix_three_rhs(&matrix) ||
        !xk_binding_make(&binding, &matrix, XIANZHIKRYLOV_CSR, 1) ||
        !xk_solution_init(&solution, &matrix)) goto done;

    status = xianzhikrylov_create(0, &handle);
    if (status != XIANZHIKRYLOV_SUCCESS) goto done;
    status = xianzhikrylov_config_set(handle, XIANZHIKRYLOV_CONFIG_FILE, config_path);
    if (status != XIANZHIKRYLOV_SUCCESS) goto done;

    status = xianzhikrylov_execute(handle, XIANZHIKRYLOV_PHASE_ANALYSIS,
                                   binding.matrix_object, NULL, NULL);
    if (status != XIANZHIKRYLOV_SUCCESS) goto done;
    status = xianzhikrylov_execute(handle, XIANZHIKRYLOV_PHASE_NUMERIC_PREPARE,
                                   binding.matrix_object, NULL, NULL);
    if (status != XIANZHIKRYLOV_SUCCESS) goto done;
    status = xianzhikrylov_execute(handle, XIANZHIKRYLOV_PHASE_SOLVE,
                                   binding.matrix_object, solution.vector_object,
                                   binding.rhs_object);
    if (status != XIANZHIKRYLOV_CONVERGED) goto done;
    status = xianzhikrylov_print_info(handle);
    if (status != XIANZHIKRYLOV_SUCCESS || !xk_verify(&matrix, &solution, 1e-8L)) goto done;

    /* Only b changes. A and its prepared preconditioner are reused. */
    xk_matrix_scale_rhs(&matrix, 0.5);
    status = xianzhikrylov_execute(handle, XIANZHIKRYLOV_PHASE_SOLVE,
                                   binding.matrix_object, solution.vector_object,
                                   binding.rhs_object);
    if (status != XIANZHIKRYLOV_CONVERGED) goto done;
    status = xianzhikrylov_print_info(handle);
    if (status != XIANZHIKRYLOV_SUCCESS || !xk_verify(&matrix, &solution, 1e-8L)) goto done;
    rc = 0;

done:
    if (rc) fprintf(stderr, "sample failed, last API status=%d\n", (int)status);
    if (handle && xianzhikrylov_destroy(handle) != XIANZHIKRYLOV_SUCCESS) rc = 1;
    xk_solution_free(&solution);
    xk_binding_free(&binding);
    xk_matrix_free(&matrix);
    return rc;
}
