# XianzhiKrylov Samples

## Overview

XianzhiKrylov is a GPU library for solving sparse linear systems with Krylov
subspace methods. It provides iterative solvers and preconditioners through one
C API. Different GPU vendor packages use the same API.

This repository provides 35 C examples. They show how to pass A and b to the
library, choose a solver route with `solver.json`, solve the system, copy back
x, and check the residual.

XianzhiKrylov binaries and runtime licenses are not part of this repository.
Download them from [Solved Tech](https://store.solved-tech.com/).

## Quick Start

### What you need

- a Linux machine supported by the XianzhiKrylov package;
- a GPU vendor toolkit version supported by the package;
- a C compiler and GNU Make;
- a valid license for the installed GPU.

A package is compatible when its README lists your CPU architecture, GPU model
or architecture, and CUDA Toolkit/DTK/MUSA SDK version. A newer toolkit is not
automatically supported. GPU driver selection is outside this repository; use
the driver supplied by the target system.

CMake is not required.

### Package and license layout

The recommended layout places this repository inside the extracted package:

```text
XianzhiKrylov/
├── include/
├── lib/
├── license.key                # one license
├── licenses/                  # optional: multiple .key files
│   └── device-1.license.key
└── samples/                   # contents of this repository
```

Use either `license.key` or `licenses/*.key`. The license must be in the package
root, next to `include/` and `lib/`.

### Build and run the first sample

This example uses `17_fgmres_ilu0` on an NVIDIA GPU.

If you downloaded this README from GitHub, start with a separate directory:

```bash
git clone https://github.com/solved-tech-dev/xianzhikrylov-samples.git
export XK_ROOT="/path/to/XianzhiKrylov"
cp /path/to/xianzhikrylov-xxxx.license.key "$XK_ROOT/license.key"
cd xianzhikrylov-samples/17_fgmres_ilu0
make clean
make XK_ROOT="$XK_ROOT" XK_BACKEND=1
make run XK_ROOT="$XK_ROOT" XK_BACKEND=1
```

Replace `/path/to/XianzhiKrylov` with the extracted package directory.

If this repository is already in the package's `samples/` directory, use:

```bash
cp /path/to/xianzhikrylov-xxxx.license.key ../../license.key
cd /path/to/XianzhiKrylov/samples/17_fgmres_ilu0
make clean
make XK_BACKEND=1
make run XK_BACKEND=1
```

For Hygon DCU with the HIP package, use `XK_BACKEND=2`. For other vendors, use
the value in that package's README.

When it works, the output contains:

```text
XianzhiKrylov status=1 converged=1 iterations=<n> route=<route> residual=<r> rhs_norm=<b> relative_residual=<ratio> prepare_solve_seconds=<time>
Independent residual RHS 1: relative_l2=<value> absmax=<value> PASS
```

The values depend on the matrix and GPU. `PASS` means that x was copied back to
the CPU and `r=b-Ax` was checked with the original matrix. Solver status `1`
means convergence; API calls that only perform an action return `0` on success.

`17_fgmres_ilu0` is a performance-sized example with about one million rows, not
a tiny smoke test. It may take several seconds and use a noticeable amount of
GPU memory.

## Features

The examples cover:

- iterative methods: CG, PCG, FCG, BiCG, BiCGSTAB, GMRES, FGMRES, and GCR;
- preconditioners: none, Jacobi, ILU(0), IC(0), DIC, DILU, and block variants;
- matrix formats: CSR, BSR, MSR, and LDU;
- FP64, complex FP64, FP32, complex FP32, and Double-Double data.

The exact route mix depends on the vendor package.

## Examples

`Precision` is the native type used for A, b, and x in the sample and in the
selected solver route.

| Sample name | Method | Matrix type | Precision |
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

## Using Your Own Matrix

You do not need to convert your matrix to `.csrpat`. That format is only used by
the samples to keep the repository small.

For your own CSR matrix:

1. Put row offsets, column indices, and values in CPU arrays.
2. Fill an `xianzhikrylov_system_data` descriptor.
3. Pass it to `xianzhikrylov_set_system`.
4. Select the method and preconditioner in `solver.json`.
5. Run prepare, solve, and solution download.

In `sample.c`, replace the calls to `xk_matrix_load` and `xk_binding_make` with
your own loader and descriptor setup. Keep the public API calls from
`xianzhikrylov_init` through `xianzhikrylov_destroy`.

The package headers in `include/` contain the current structures and field
rules. Standard routes use FP64; the precision samples pass FP32, complex FP32,
or Double-Double descriptors directly. The samples generate b from a known
exact solution so that they can check the answer.

## Repository Layout

```text
samples/
├── README.md                  # overview, quick start, and example list
├── Makefile                   # build or clean all samples
├── PATTERN_FORMAT.md          # format of the .csrpat matrix files
├── routes.json                # method and data settings for all samples
├── data/                      # shared matrix patterns
├── docs/
│   └── TROUBLESHOOTING.md     # detailed fault finding
├── 01_cg/
│   ├── sample.c               # runnable C program
│   ├── sample_common.h        # loader, format conversion, and residual check
│   ├── solver.json            # method and preconditioner settings
│   ├── Makefile               # build and run this sample
│   ├── README.md              # short run guide
│   └── data/
│       └── diffusion_1m.csrpat -> ../../data/diffusion_1m.csrpat
├── ...
└── 35_dd_fgmres/
    └── ...
```

`sample_common.h` reads the sparse pattern, builds A and b, converts matrix
formats, and checks the result. The library does not require this helper.

## Sample Data

The four `.csrpat` files store sparse patterns, not complete linear systems.
Matrix values and b are built at runtime from fixed settings and known exact
solutions. Therefore every run uses the same data without storing millions of
matrix values in Git.

The `data/` file in each sample is a link to the shared data. A Git clone or
original `.tar.gz` keeps these links. If your ZIP tool loses links, pass the
matrix path directly to `sample`.

See [data/README.md](data/README.md) and
[PATTERN_FORMAT.md](PATTERN_FORMAT.md) for details.

## Data Flow

The sample creates A and b in CPU memory and marks them as
`XIANZHIKRYLOV_HOST`.

1. `xianzhikrylov_init` creates the handle and selects the GPU.
2. `xianzhikrylov_set_system` copies A and b into GPU storage owned by the
   library and waits for the copy to finish.
3. `xianzhikrylov_solve_prepare` prepares the selected route.
4. `xianzhikrylov_solve` computes x.
5. `xianzhikrylov_solution_download` copies x back to CPU memory.

The library owns the uploaded data. The sample can free its CPU copies after
`set_system` returns.

## Troubleshooting

Common first checks:

- missing header: set `XK_ROOT` to the package root;
- missing shared library: run `ldd ./sample`;
- license error: check `license.key` or `licenses/*.key`;
- missing matrix: keep the Git/`.tar.gz` links or pass a direct path;
- wrong backend: check `XK_BACKEND` in the package README.

See [docs/TROUBLESHOOTING.md](docs/TROUBLESHOOTING.md) for the full guide.
