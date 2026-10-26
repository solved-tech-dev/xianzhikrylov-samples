# cg on CSR

## Purpose

This sample shows how to solve `Ax=b` with **cg** using CSR matrix data in FP64.

## Build and Run

Download a compatible package and license from [Solved Tech](https://store.solved-tech.com/). Put the extracted package under `libraries/<package-name>/` and keep its `license.key` in that directory.

```bash
make
make run
make clean
```

If the package is outside `libraries/`:

```bash
make LIBRARY_ROOT=/path/to/package
make run LIBRARY_ROOT=/path/to/package
```

## Input Files

- [`sample.c`](sample.c): complete sample program.
- [`config.json`](config.json): method, preconditioner, and method settings.
- [`diffusion_1m.csrpat`](../matrices/diffusion_1m.csrpat): sparse pattern used by the default run.

The program creates values for A and b from the pattern settings and keeps the original system for verification.

## Output

The program prints API status, convergence status, iteration count, method information, residual, RHS norm, and timing. It then computes the original-system residual independently.

`PASS` and exit code `0` indicate that the returned solution satisfies the sample tolerance.

See the [repository overview](../README.md), [product manual](../docs/XianzhiKrylov_Manual_26.44_CN.pdf), and [troubleshooting guide](../docs/TROUBLESHOOTING.md).
