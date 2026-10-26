# Sample Matrix Data

| File | Order | Stored entries | Model | RHS count |
| --- | ---: | ---: | ---: | ---: |
| `diffusion_1m.csrpat` | 1,000,000 | 6,940,000 | 1 | 1 |
| `transport_1m.csrpat` | 1,000,000 | 6,940,000 | 2 | 1 |
| `coupled4_500k.csrpat` | 500,000 | 4,940,000 | 3 | 1 |
| `complex_262k_3rhs.csrpat` | 262,144 | 1,810,432 | 4 | 3 |

These files contain only CSR sparsity patterns. They do not contain the values
of A or the right-hand-side vector b. See
[`../PATTERN_FORMAT.md`](../PATTERN_FORMAT.md) for the binary layout and value
generation rules.

Each sample's `data/` directory contains a relative symbolic link to one of
these files. All 35 samples share the same data to avoid storing many duplicate
copies.

The original `.tar.gz` archive preserves these links. If a ZIP tool does not
preserve symbolic links, use the Git repository or the original `.tar.gz`
archive, or pass an absolute matrix path when running a sample.
