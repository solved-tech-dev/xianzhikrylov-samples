# Sample Pattern Data

| File | Rows | Stored entries | Model | RHS |
| --- | ---: | ---: | ---: | ---: |
| `diffusion_1m.csrpat` | 1,000,000 | 6,940,000 | 1 | 1 |
| `transport_1m.csrpat` | 1,000,000 | 6,940,000 | 2 | 1 |
| `coupled4_500k.csrpat` | 500,000 | 4,940,000 | 3 | 1 |
| `complex_262k_3rhs.csrpat` | 262,144 | 1,810,432 | 4 | 3 |

The files contain CSR sparsity patterns only. See
[`../PATTERN_FORMAT.md`](../PATTERN_FORMAT.md) for the binary layout and value
generation rules.

Each sample directory has a relative link to its default pattern file. The
links resolve inside this repository and avoid duplicating the large patterns
32 times.
