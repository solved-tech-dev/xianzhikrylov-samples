# Multiple right-hand sides and preparation reuse

## Purpose

Solve three FP64 systems with the same CSR matrix using FGMRES and ILU(0).
The sample calls analysis, numeric preparation, and solve separately. It then
changes only the right-hand sides and solves again without rebuilding the
preconditioner.

## Build and Run

Place your library package and license as described in the
[main README](../README.md), then run these commands from this directory:

```bash
make
make run
```

Use `make clean` to remove the compiled program.

## Input Files

- `../matrices/transport_1m.csrpat`: the shared sparse matrix pattern.
- `config.json`: the iterative method and preconditioner configuration.

Matrix values are generated from the pattern settings. The first right-hand
side is `A * 1`; the other two are twice and three times that vector. The
second solve uses half of each right-hand side. All solves start from zero.

## Output

The library prints iterations and residuals for each right-hand side, along
with analysis, numeric preparation, and solve times. After the second solve,
`structure_builds=1 numeric_builds=1` shows that preparation was reused.

The sample checks all six results against the original matrix. Each relative
L2 residual must be no greater than `1e-8`. Exit code `0` means all checks passed.
