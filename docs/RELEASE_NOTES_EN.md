# XianzhiKrylov 26.48

## New features

- **NVIDIA CUDA** Adds native BSR3 FGMRES with block ILU(0) at Level 0 for FP64 and a zero initial guess.
- **Hygon HIP** Adds JSON v2 support for selecting the solver, preconditioner, and applicable Level, with optional true-residual verification for the original system.
- **Platform and compatibility** Preserves compatibility with the existing C API, ABI, and JSON v1 configurations.
- **Samples** Adds a JSON v2-to-v1 tool that converts equivalent FP64 configurations to JSON v1 for XianzhiKrylov 26.44 and 26.45.

## Performance improvements

- **NVIDIA CUDA** Reduces memory use and solve time for large CSR GMRES with Jacobi. Fixes performance regressions for selected matrices using CSR BiCGStab or GMRES with ILU(0) Level 0. Fixes index overflow in large block ILU(0) and invalid memory access when block FGMRES workspace allocation fails.
- **Hygon HIP** Improves single-GPU mixed-precision CG, double-double FGMRES, LinglongAMG-preconditioned solves, and BiCGStab with 3×3 block Jacobi. Reduces preconditioner setup time for complex FGMRES with ILU(0) and multiple right-hand sides.

---

# XianzhiKrylov 26.46 CUDA release notes

- Add JSON v2 for solver, preconditioner and Level selection; retain JSON v1 compatibility.
- Default omitted ordinary FP64 relative tolerance to 1e-6; preserve explicit tolerances.
- Reduce overhead in selected CG, FCG, BiCG, DILU, MSR and IC0 routes, incorporating SDK staging and ILU(0) preconditioner improvements. Performance depends on the matrix and execution environment.
- Fix premature numerical failure during staged CG/Jacobi analysis; preserve the public API and ABI.
- Update bilingual manuals with CUDA versions, SM/PTX compilation targets and staged data-update guidance.
- Provide independent Samples with JSON v2, v1 migration configurations and staged CG/Jacobi updates for RHS, coefficients and sparsity.
