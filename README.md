# XianzhiKrylov Samples

## Overview

XianzhiKrylov solves sparse linear systems with GPU Krylov methods. Packages
from different GPU vendors use the same C API. This repository provides
independent C samples that load a matrix and right-hand side, select a method
with `config.json`, solve `Ax=b`, copy back x, and verify the original-system
residual.

The samples cover common iterative methods, preconditioners, sparse matrix
formats, multiple right-hand sides, and several data precisions.

You can download library packages and licenses from
<https://store.solved-tech.com/>.

## Repository Layout

```text
xianzhikrylov-samples/
├── README.md
├── Makefile                    # Build or clean every sample
├── common.mk                   # Shared build, link, run, and clean rules
│
├── common/
│   └── sample_common.h         # Helper code shared by multiple samples
│
├── libraries/
│   ├── README.md               # Library package and license layout
│   └── <package-name>/
│       ├── include/
│       ├── lib/
│       ├── license.key
│       └── ...
│
├── matrices/
│   ├── README.md               # Matrix file descriptions
│   ├── PATTERN_FORMAT.md       # Sparse pattern format
│   ├── transport_1m.csrpat
│   └── ...
│
├── docs/
│   ├── PRODUCT_GUIDE.md        # C API and configuration guide
│   └── TROUBLESHOOTING.md      # Common build and runtime problems
│
├── 01_cg/
│   ├── sample.c
│   ├── config.json
│   ├── Makefile
│   └── README.md
├── 02_bicg/
│   └── ...
└── ...
```

## Quick Start

### Requirements

- a downloaded XianzhiKrylov package and its license;
- a Linux machine supported by the selected package;
- the GPU and the toolkit/runtime required by the selected package;
- a C compiler and GNU Make.

### Library Package and License

Download a package and license from
<https://store.solved-tech.com/>. Extract the package into `libraries/`:

```text
libraries/
└── <package-name>/
    ├── include/
    ├── lib/
    ├── license.key
    └── ...
```

Keep `license.key` in the same package directory.

`common.mk` automatically uses the package containing:

```text
libraries/*/include/XianzhiKrylovAPI.h
```

### Build and Run

```bash
cd xianzhikrylov-samples
make -C 17_fgmres_ilu0
make -C 17_fgmres_ilu0 run
```

If several packages are installed, select one explicitly:

```bash
make -C 17_fgmres_ilu0 LIBRARY_ROOT=/path/to/package
make -C 17_fgmres_ilu0 run LIBRARY_ROOT=/path/to/package
```

Each sample also supports these commands from its own directory:

```bash
make
make run
make clean
```

Build or clean every sample from the repository root:

```bash
make
make clean
```

## Samples

### Samples List

| Sample name | Method | Matrix format | Precision |
| --- | --- | --- | --- |
| [01_cg](01_cg/README.md) | cg | CSR | FP64 |
| [02_bicg](02_bicg/README.md) | bicg | CSR | FP64 |
| [03_bicgstab](03_bicgstab/README.md) | bicgstab | CSR | FP64 |
| [04_gmres](04_gmres/README.md) | gmres | CSR | FP64 |
| [05_fgmres](05_fgmres/README.md) | fgmres | CSR | FP64 |
| [06_gcr](06_gcr/README.md) | gcr | CSR | FP64 |
| [07_pcg_jacobi](07_pcg_jacobi/README.md) | pcg / jacobi | CSR | FP64 |
| [08_fcg_jacobi](08_fcg_jacobi/README.md) | fcg / jacobi | CSR | FP64 |
| [09_bicg_jacobi](09_bicg_jacobi/README.md) | bicg / jacobi | CSR | FP64 |
| [10_bicgstab_jacobi](10_bicgstab_jacobi/README.md) | bicgstab / jacobi | CSR | FP64 |
| [11_gmres_jacobi](11_gmres_jacobi/README.md) | gmres / jacobi | CSR | FP64 |
| [12_fgmres_jacobi](12_fgmres_jacobi/README.md) | fgmres / jacobi | CSR | FP64 |
| [13_gcr_jacobi](13_gcr_jacobi/README.md) | gcr / jacobi | CSR | FP64 |
| [14_bicg_ilu0](14_bicg_ilu0/README.md) | bicg / ilu0 | CSR | FP64 |
| [15_bicgstab_ilu0](15_bicgstab_ilu0/README.md) | bicgstab / ilu0 | CSR | FP64 |
| [16_gmres_ilu0](16_gmres_ilu0/README.md) | gmres / ilu0 | CSR | FP64 |
| [17_fgmres_ilu0](17_fgmres_ilu0/README.md) | fgmres / ilu0 | CSR | FP64 |
| [18_gcr_ilu0](18_gcr_ilu0/README.md) | gcr / ilu0 | CSR | FP64 |
| [19_pcg_ic0](19_pcg_ic0/README.md) | pcg / ic0 | CSR | FP64 |
| [20_fcg_ic0](20_fcg_ic0/README.md) | fcg / ic0 | CSR | FP64 |
| [21_gmres_ic0](21_gmres_ic0/README.md) | gmres / ic0 | CSR | FP64 |
| [22_fgmres_ic0](22_fgmres_ic0/README.md) | fgmres / ic0 | CSR | FP64 |
| [23_bicgstab_ic0](23_bicgstab_ic0/README.md) | bicgstab / ic0 | CSR | FP64 |
| [24_bicg_ic0](24_bicg_ic0/README.md) | bicg / ic0 | CSR | FP64 |
| [25_gcr_ic0](25_gcr_ic0/README.md) | gcr / ic0 | CSR | FP64 |
| [26_bsr4_fgmres_ilu0](26_bsr4_fgmres_ilu0/README.md) | fgmres / ilu0 | BSR4 | FP64 |
| [27_bsr4_fgmres_jacobi](27_bsr4_fgmres_jacobi/README.md) | fgmres / block_jacobi | BSR4 | FP64 |
| [28_bsr4_fgmres_dilu](28_bsr4_fgmres_dilu/README.md) | fgmres / multicolor_block_dilu | BSR4 | FP64 |
| [29_msr_bicgstab_jacobi](29_msr_bicgstab_jacobi/README.md) | bicgstab / jacobi | MSR | FP64 |
| [30_ldu_pcg_dic](30_ldu_pcg_dic/README.md) | pcg / dic | LDU | FP64 |
| [31_ldu_bicgstab_dilu](31_ldu_bicgstab_dilu/README.md) | bicgstab / dilu | LDU | FP64 |
| [32_complex_batch3](32_complex_batch3/README.md) | fgmres_complex_batch | CSR | complex FP64 |
| [33_float_fgmres_jacobi](33_float_fgmres_jacobi/README.md) | fgmres / jacobi | CSR | FP32 |
| [34_float_complex_batch3](34_float_complex_batch3/README.md) | fgmres_complex_fp32_batch | CSR | complex FP32 |
| [35_dd_fgmres](35_dd_fgmres/README.md) | DD-FGMRES | CSR | Double-Double |

### Sample Input

The files in `matrices/` contain sparse patterns and fixed generation settings.
Each sample creates deterministic matrix values and a right-hand side, then uses
the original system for an independent residual check. The sample `Makefile`
selects the appropriate default matrix.

`config.json` selects the method, preconditioner, tolerances, and precision-
specific method.

See [matrices/README.md](matrices/README.md) and
[matrices/PATTERN_FORMAT.md](matrices/PATTERN_FORMAT.md) for details.

## Output and Verification

A successful solve prints status, iteration count, method information,
residual, RHS norm, and preparation/solve timing. The program also copies x
and computes the original-system residual independently.

```text
XianzhiKrylov status=1 converged=1 iterations=<n> method=<method> residual=<r> rhs_norm=<b> relative_residual=<ratio> prepare_solve_seconds=<time>
Independent residual RHS 1: relative_l2=<value> absmax=<value> PASS
```

API status `1` means the solver reported convergence. Numerical correctness
requires the independent residual to satisfy the documented tolerance. `PASS`
and exit code `0` indicate that both checks succeeded.

See [docs/PRODUCT_GUIDE.md](docs/PRODUCT_GUIDE.md) for the unified C API,
configuration fields, precision definitions, and ownership rules.

## Troubleshooting

Common checks:

- missing header or library: make sure the selected package is under
  `libraries/<package-name>/`, or set `LIBRARY_ROOT`;
- license error: make sure `license.key` is in the selected package directory;
- missing input: use this Git checkout or pass an explicit matrix path;
- unsupported environment: use the package built for the target GPU and its
  required toolkit/runtime;
- missing indirect runtime library: set `GPU_RUNTIME_LIB_DIR`;
- no convergence: inspect `config.json`, the method support table, and the
  independent residual.

See [docs/TROUBLESHOOTING.md](docs/TROUBLESHOOTING.md) for detailed steps.
