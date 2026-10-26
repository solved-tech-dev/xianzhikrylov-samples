/* XianzhiKrylov sample 26_bsr4_fgmres_ilu0: fgmres / ilu0. */
#include "sample_common.h"
#include <stdio.h>

int main(int argc, char **argv)
{
    const char *matrix_path = argc > 1 ? argv[1] : "../matrices/coupled4_500k.csrpat";
    const char *json_path = argc > 2 ? argv[2] : "config.json";
    xk_matrix matrix = {0};
    xk_binding binding = {0};
    xk_solution solution = {0};
    xianzhikrylov_handle_t handle = NULL;
    xianzhikrylov_status_t status;
    int rc = 2, destroyed = 0;

    if (!xk_matrix_load(&matrix, matrix_path))
        goto done;
    if (!xk_binding_make(&binding, &matrix, XIANZHIKRYLOV_BSR, 4)) {
        fprintf(stderr, "cannot prepare matrix format\n");
        goto done;
    }
    if (!xk_solution_init(&solution, &matrix)) {
        fprintf(stderr, "cannot allocate solution\n");
        goto done;
    }

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
    /* HOST A, b, and x are prepared by the library during execute(ALL). */
    status = xianzhikrylov_execute(
        handle, XIANZHIKRYLOV_PHASE_ALL, binding.matrix_object,
        solution.vector_object, binding.rhs_object);
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
        const int residual_ok = xk_verify(&matrix, &solution, 1e-8L);
        destroyed = 1;
        rc = print_status == XIANZHIKRYLOV_SUCCESS &&
             destroy_status == XIANZHIKRYLOV_SUCCESS &&
             residual_ok ? 0 : 1;
    }

done:
    if (handle && !destroyed)
        (void)xianzhikrylov_destroy(handle);
    xk_solution_free(&solution);
    xk_binding_free(&binding);
    xk_matrix_free(&matrix);
    return rc;
}
