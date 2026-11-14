# Migrating to 26.46

The C API and ABI remain compatible. Ordinary FP64 configurations default to relative_tolerance=1e-6 when omitted. Set tolerances explicitly when reproducibility requires fixed stopping criteria.

New configurations use schema_version=2. Choose solver.method and solver.preconditioner, then an optional solver.level. ILU0 and ParILU are distinct preconditioners. For supported CSR FGMRES/GMRES/BiCGStab/GCR and BSR FGMRES with ILU0, omission selects L2; BiCG with ILU0 supports only L0. Do not apply one preconditioner's Level mapping to another.

17_fgmres_ilu0/config.json selects exact L0; config_default.json selects the registered default. Run either using CONFIG_FILE in make run. configs/v1 contains compatible migration examples: run 07_pcg_jacobi with CONFIG_FILE=../configs/v1/07_pcg_jacobi.json or 17_fgmres_ilu0 with its corresponding file.

37_cg_jacobi_staged uses the omitted relative tolerance and verifies the result independently at 1e-6. It updates the RHS, then matrix coefficients, then sparsity. Existing numerical examples retain their explicit tolerances.

See the installation chapter of the Chinese/English manual for the CUDA and SM/PTX support table. Package README describes only that package. Check every API status; MAX_ITERATIONS and UNSUPPORTED are not convergence.
