/* XianzhiKrylov sample 08_fcg_jacobi: fcg / jacobi. */
#include "sample_common.h"
#include <stdio.h>

int main(int argc, char **argv)
{
    const char *matrix_path = argc > 1 ? argv[1] : "../matrices/diffusion_1m.csrpat";
    const char *json_path = argc > 2 ? argv[2] : "config.json";
    xk_matrix matrix = {0};
    xk_binding binding = {0};
    xk_solution solution = {0};
    xianzhikrylov_handle *handle = NULL;
    xianzhikrylov_status status;
    int rc = 2, destroyed = 0;

    if (!xk_matrix_load(&matrix, matrix_path))
        goto done;
    if (!xk_binding_make(&binding, &matrix, XIANZHIKRYLOV_CSR, 1)) {
        fprintf(stderr, "cannot prepare matrix format\n");
        goto done;
    }
    if (!xk_solution_init(&solution, &matrix)) {
        fprintf(stderr, "cannot allocate solution\n");
        goto done;
    }

    status = xianzhikrylov_init(0, json_path, &handle);
    if (status != XIANZHIKRYLOV_SUCCESS) {
        fprintf(stderr, "init status=%d\n", (int)status);
        rc = 1;
        goto done;
    }
    /* HOST input is copied into library-owned GPU storage synchronously. */
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
    status = xianzhikrylov_solution_copy(handle, &solution.descriptor);
    if (status != XIANZHIKRYLOV_SUCCESS) {
        fprintf(stderr, "solution_copy status=%d\n", (int)status);
        rc = 1;
        goto done;
    }
    {
        const xianzhikrylov_status print_status =
            xianzhikrylov_print_info(handle);
        const xianzhikrylov_status destroy_status =
            xianzhikrylov_destroy(&handle);
        const int residual_ok = xk_verify(&matrix, &solution, 1e-8L);
        destroyed = 1;
        rc = print_status == XIANZHIKRYLOV_SUCCESS &&
             destroy_status == XIANZHIKRYLOV_SUCCESS &&
             residual_ok ? 0 : 1;
    }

done:
    if (handle && !destroyed)
        (void)xianzhikrylov_destroy(&handle);
    xk_solution_free(&solution);
    xk_binding_free(&binding);
    xk_matrix_free(&matrix);
    return rc;
}
