# Staged CG with Jacobi

This example generates a small SPD system and uses only the public C API. Its JSON omits relative_tolerance, selecting the 1e-6 default. Each solve checks the original-system relative L2 residual independently.

Run `make run LIBRARY_ROOT=/absolute/path/to/XianzhiKrylov`.

The program demonstrates four solves:

1. ANALYSIS, NUMERIC_PREPARE and SOLVE for the initial matrix.
2. Change only the RHS and call SOLVE, reusing preparation.
3. Change diagonal coefficients and call NUMERIC_PREPARE then SOLVE.
4. Change the sparsity and entry count, recreate the matrix descriptor, then repeat all three phases.

ANALYSIS and NUMERIC_PREPARE require NULL solution/RHS arguments and return SUCCESS. SOLVE requires all objects and must return CONVERGED. No separate public matrix-update function is required: the explicit phase communicates the update. Keep arrays alive for all calls that reference them.

This small example demonstrates API use, not performance measurement.
