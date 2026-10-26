# Complex float FGMRES on CSR

## Purpose

Solve one system `Ax=b` with **FGMRES**. A, b, and x use complex FP32:
each number has a `float` real part and a `float` imaginary part.
The sample uses one right-hand side and `xianzhikrylov_vector_create()`.
Use an NVIDIA or Hygon package.

The configuration name is `fgmres_complex_fp32_batch`. This sample
passes one vector. Multiple right-hand sides have a separate double sample:
[36_multiple_rhs_reuse](../36_multiple_rhs_reuse/README.md).

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
- [`complex_262k.csrpat`](../matrices/complex_262k.csrpat): sparse pattern used by the default run.

The program creates values for A and b from the pattern settings and keeps the original system for verification.

## Output

The program prints API status, convergence status, iteration count, method information, residual, RHS norm, and timing. It then computes the original-system residual independently.

`PASS` and exit code `0` mean that the independently checked relative
L2 residual is at most `1e-4`. The solver uses the stricter `1e-5`
relative tolerance in `config.json`.

See the [repository overview](../README.md), [product manual](../docs/XianzhiKrylov_Manual_26.44_EN.pdf), and [troubleshooting guide](../docs/TROUBLESHOOTING.md).
