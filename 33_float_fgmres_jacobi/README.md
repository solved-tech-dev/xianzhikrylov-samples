# fgmres_fp32 / jacobi on CSR

## Purpose

Solve one system `Ax=b` with **FGMRES and Jacobi**, using `float`
arrays for A, b, and x. The sample uses one right-hand side.
Use an NVIDIA or Hygon package.

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
- [`transport_1m.csrpat`](../matrices/transport_1m.csrpat): sparse pattern used by the default run.

The program creates values for A and b from the pattern settings and keeps the original system for verification.

## Output

The program prints API status, convergence status, iteration count, method information, residual, RHS norm, and timing. It then computes the original-system residual independently.

`PASS` and exit code `0` indicate that the returned solution satisfies the sample tolerance.

See the [repository overview](../README.md), [product manual](../docs/XianzhiKrylov_Manual_26.44_EN.pdf), and [troubleshooting guide](../docs/TROUBLESHOOTING.md).
