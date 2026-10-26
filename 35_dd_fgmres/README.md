# fgmres_dd on CSR

## Purpose

Solve one system `Ax=b` with **DD-FGMRES**. A, b, and x each use
high and low `double` arrays; their sums give the Double-Double values.
Use an NVIDIA or Hygon package.

This sample has one right-hand side. It uses
`xianzhikrylov_multiple_vector_create()` with `rhs_count=1` to pass
both the high and low arrays. See
[36_multiple_rhs_reuse](../36_multiple_rhs_reuse/README.md) for the
separate double-precision, multiple-right-hand-side example.

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

See the [repository overview](../README.md), [product manual](../docs/XianzhiKrylov_Manual_26.44_EN.pdf), and [troubleshooting guide](../docs/TROUBLESHOOTING.md).
