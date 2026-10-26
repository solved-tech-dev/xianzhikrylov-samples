# XianzhiKrylov Samples

32 independent C samples for the XianzhiKrylov C API. Each directory contains
`sample.c`, its local `sample_common.h`, `solver.json`, a Makefile, a short
README, and a package-internal data link. XianzhiKrylov libraries and license
files are available from [https://store.solved-tech.com/](https://store.solved-tech.com/).

| Sample | Route | Format | Default data |
| --- | --- | --- | --- |
| [01_cg](01_cg/README.md) | cg / none | csr | `diffusion_1m.csrpat` |
| [02_bicg](02_bicg/README.md) | bicg / none | csr | `transport_1m.csrpat` |
| [03_bicgstab](03_bicgstab/README.md) | bicgstab / none | csr | `transport_1m.csrpat` |
| [04_gmres](04_gmres/README.md) | gmres / none | csr | `transport_1m.csrpat` |
| [05_fgmres](05_fgmres/README.md) | fgmres / none | csr | `transport_1m.csrpat` |
| [06_gcr](06_gcr/README.md) | gcr / none | csr | `transport_1m.csrpat` |
| [07_pcg_jacobi](07_pcg_jacobi/README.md) | pcg / jacobi | csr | `diffusion_1m.csrpat` |
| [08_fcg_jacobi](08_fcg_jacobi/README.md) | fcg / jacobi | csr | `diffusion_1m.csrpat` |
| [09_bicg_jacobi](09_bicg_jacobi/README.md) | bicg / jacobi | csr | `transport_1m.csrpat` |
| [10_bicgstab_jacobi](10_bicgstab_jacobi/README.md) | bicgstab / jacobi | csr | `transport_1m.csrpat` |
| [11_gmres_jacobi](11_gmres_jacobi/README.md) | gmres / jacobi | csr | `transport_1m.csrpat` |
| [12_fgmres_jacobi](12_fgmres_jacobi/README.md) | fgmres / jacobi | csr | `transport_1m.csrpat` |
| [13_gcr_jacobi](13_gcr_jacobi/README.md) | gcr / jacobi | csr | `transport_1m.csrpat` |
| [14_bicg_ilu0](14_bicg_ilu0/README.md) | bicg / ilu0 | csr | `transport_1m.csrpat` |
| [15_bicgstab_ilu0](15_bicgstab_ilu0/README.md) | bicgstab / ilu0 | csr | `transport_1m.csrpat` |
| [16_gmres_ilu0](16_gmres_ilu0/README.md) | gmres / ilu0 | csr | `transport_1m.csrpat` |
| [17_fgmres_ilu0](17_fgmres_ilu0/README.md) | fgmres / ilu0 | csr | `transport_1m.csrpat` |
| [18_gcr_ilu0](18_gcr_ilu0/README.md) | gcr / ilu0 | csr | `transport_1m.csrpat` |
| [19_pcg_ic0](19_pcg_ic0/README.md) | pcg / ic0 | csr | `diffusion_1m.csrpat` |
| [20_fcg_ic0](20_fcg_ic0/README.md) | fcg / ic0 | csr | `diffusion_1m.csrpat` |
| [21_gmres_ic0](21_gmres_ic0/README.md) | gmres / ic0 | csr | `diffusion_1m.csrpat` |
| [22_fgmres_ic0](22_fgmres_ic0/README.md) | fgmres / ic0 | csr | `diffusion_1m.csrpat` |
| [23_bicgstab_ic0](23_bicgstab_ic0/README.md) | bicgstab / ic0 | csr | `diffusion_1m.csrpat` |
| [24_bicg_ic0](24_bicg_ic0/README.md) | bicg / ic0 | csr | `diffusion_1m.csrpat` |
| [25_gcr_ic0](25_gcr_ic0/README.md) | gcr / ic0 | csr | `diffusion_1m.csrpat` |
| [26_bsr4_fgmres_ilu0](26_bsr4_fgmres_ilu0/README.md) | fgmres / ilu0 | bsr | `coupled4_500k.csrpat` |
| [27_bsr4_fgmres_jacobi](27_bsr4_fgmres_jacobi/README.md) | fgmres / block_jacobi | bsr | `coupled4_500k.csrpat` |
| [28_bsr4_fgmres_dilu](28_bsr4_fgmres_dilu/README.md) | fgmres / multicolor_block_dilu | bsr | `coupled4_500k.csrpat` |
| [29_msr_bicgstab_jacobi](29_msr_bicgstab_jacobi/README.md) | bicgstab / jacobi | msr | `transport_1m.csrpat` |
| [30_ldu_pcg_dic](30_ldu_pcg_dic/README.md) | pcg / dic | ldu | `diffusion_1m.csrpat` |
| [31_ldu_bicgstab_dilu](31_ldu_bicgstab_dilu/README.md) | bicgstab / dilu | ldu | `transport_1m.csrpat` |
| [32_complex_batch3](32_complex_batch3/README.md) | fgmres_complex_batch / none | csr | `complex_262k_3rhs.csrpat` |

## Prerequisites

- An extracted XianzhiKrylov package matching the target Linux system and
  `<GPU_VENDOR>`.
- A complete C11 compiler toolchain, including the compiler and linker.
- GNU Make.
- The matching GPU vendor development toolkit, such as CUDA, DTK, MUSA, or
  another supported toolkit.
- A valid XianzhiKrylov license for the selected GPU.

## Quick start

Keep `samples/` beside the package `include/` and `lib/` directories, or pass
their parent explicitly:

```bash
export XIANZHIKRYLOV_LICENSE_PATHS="/absolute/path/to/license.key"
cd samples/17_fgmres_ilu0
make XK_ROOT="/absolute/path/to/XianzhiKrylov" XK_BACKEND=1
make run XK_ROOT="/absolute/path/to/XianzhiKrylov" XK_BACKEND=1
```

Build all samples:

```bash
make XK_ROOT="/absolute/path/to/XianzhiKrylov" XK_BACKEND=1
```

Each Makefile supports `CC`, `CFLAGS`, `CPPFLAGS`, `LDFLAGS`, `LDLIBS`,
`XK_ROOT`, `XK_BACKEND`, `GPU_RUNTIME_LIB_DIR`, and `RUN_ARGS`. Set
`GPU_RUNTIME_LIB_DIR` when the vendor runtime is not in the system loader path.
Use `make clean` before changing the package, backend, toolkit, or compiler
flags. CMake is not required.

## Data and output

The four million-order test systems are shared under `samples/data/`; each
sample's `data/` entry is a relative link into that directory to avoid copying
the same data repeatedly. The four pattern files occupy about 90 MiB.

`.csrpat` stores only:

- matrix order and stored-entry count;
- CSR row offsets and column indices;
- scalar type, model identifier, fixed seed, and RHS count.

It does not store matrix values or the right-hand side. `sample_common.h`
generates deterministic values from the pattern and fixed seed, then constructs
the right-hand side from known exact solutions:

- real systems use `x_exact = 1` and `b = A*x_exact`;
- the complex three-RHS sample uses three distinct constant complex solutions.

Generation is deterministic and uses integer-only hashing, so no timestamp,
device ID, or process-local random source changes the system. SPD models are
generated with a positive diagonal, non-positive off-diagonal entries, and
strict diagonal dominance. General models preserve a well-conditioned positive
row sum.

The binary values are generated before the library preparation call. BSR, MSR,
and LDU samples convert the generated CSR system explicitly. Each run validates
the original equation on the host; file reading, value generation, conversion,
download, and residual checking are outside the library timing.

See [`PATTERN_FORMAT.md`](PATTERN_FORMAT.md) for the `.csrpat` binary layout,
fixed value-generation rules, and known solutions.
