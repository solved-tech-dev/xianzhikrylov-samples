# XianzhiKrylov 26.46 CUDA release notes

- Add JSON v2 for solver, preconditioner and Level selection; retain JSON v1 compatibility.
- Default omitted ordinary FP64 relative tolerance to 1e-6; preserve explicit tolerances.
- Reduce overhead in selected CG, FCG, BiCG, DILU, MSR and IC0 routes, incorporating SDK staging and exact triangular-solve improvements. Performance depends on the matrix and execution environment.
- Fix premature numerical failure during staged CG/Jacobi analysis; preserve the public API and ABI.
- Update bilingual manuals with CUDA versions, SM/PTX compilation targets and staged data-update guidance.
- Provide independent Samples with JSON v2, v1 migration configurations and staged CG/Jacobi updates for RHS, coefficients and sparsity.
