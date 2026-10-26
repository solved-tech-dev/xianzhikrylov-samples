# XKPAT1 Pattern Format

`.csrpat` files contain the sparse pattern of a square CSR matrix. They do not
contain matrix values or right-hand sides.

All integer fields are little-endian. The file starts with this 64-byte header:

| Offset | Type | Meaning |
| ---: | --- | --- |
| 0 | 8 bytes | Magic `XKPAT1\0\0` |
| 8 | `uint32_t` | Format version, currently `1` |
| 12 | `uint32_t` | `1` = real FP64, `2` = complex FP64 |
| 16 | `uint64_t` | Scalar matrix order `n` |
| 24 | `uint64_t` | Stored entry count `nnz` |
| 32 | `uint32_t` | RHS count |
| 36 | `uint32_t` | Suggested block size |
| 40 | `uint32_t` | Model identifier |
| 44 | `uint32_t` | Reserved, must be zero |
| 48 | `uint64_t` | Fixed value-generation seed |
| 56 | `uint64_t` | Reserved, must be zero |

After the header:

1. `int32_t row_offsets[n+1]`
2. `int32_t column_indices[nnz]`

Indices are zero-based. Every row must be strictly increasing with no duplicate
columns.

## Model identifiers

| ID | Pattern | Value generation |
| ---: | --- | --- |
| 1 | `diffusion_1m` | Symmetric SPD, strict diagonal dominance |
| 2 | `transport_1m` | General nonsymmetric, positive row sums |
| 3 | `coupled4_500k` | Symmetric block-oriented SPD, strict diagonal dominance |
| 4 | `complex_262k_3rhs` | Complex, strictly diagonally dominant, three RHS |

## Value generation

`sample_common.h` uses a fixed integer-only hash of:

- the seed stored in the file;
- model identifier;
- row and column indices;
- a fixed salt.

The result does not depend on time, the process, the CUDA device, or random
device state.

For real symmetric models, a value is derived from the ordered pair
`(min(i,j), max(i,j))`, so both sparse entries receive the same value.
Off-diagonal values are non-positive. The diagonal is calculated as the sum of
absolute off-diagonal values plus a positive increment, producing strict
diagonal dominance.

For the general transport model, the same strict row-dominance rule is applied,
but values depend on the ordered pair `(i,j)` and remain nonsymmetric.

For the complex model, off-diagonal values have deterministic real and
imaginary components. The diagonal is positive real and exceeds the row sum of
off-diagonal magnitudes.

## Known solutions

For real systems:

```text
x_exact = 1
b = A*x_exact
```

For the complex three-RHS model:

```text
x_1 = 1 + 0i
x_2 = 1 + 0.25i
x_3 = 1 - 0.25i
b_j = A*x_j
```

The solver still starts from its configured initial guess, normally zero. The
known solutions are used only to construct b and to understand the independent
host-side residual checks.
