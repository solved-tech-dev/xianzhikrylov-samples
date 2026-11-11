# XianzhiKrylov 26.46 (Hygon)

New features:
- Improved result writeback and nonzero-initial-guess handling for complex batched solves.
- Optimized device-memory management when GPU matrices are supplied directly.
- Preserved the existing C API and binary compatibility.

Performance improvements:
- Improved solve efficiency for block-Jacobi preconditioning, high-precision FGMRES, complex multiple-right-hand-side solves, and solves that use LinglongAMG as a preconditioner.
