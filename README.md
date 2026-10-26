# XianzhiKrylov Samples

This repository contains 35 independent C samples for the XianzhiKrylov C API.
Each sample shows how to pass A and b to XianzhiKrylov, run a solver, copy back
x, and check the residual.

You can download XianzhiKrylov packages and licenses from
[https://store.solved-tech.com/](https://store.solved-tech.com/).

## Package and License Layout

The XianzhiKrylov package and license are not part of this Samples repository.
The recommended layout is:

```text
XianzhiKrylov/                  # one extracted vendor package
├── include/                   # C API headers
├── lib/                       # library and runtime core files
├── license.key                # one license file
├── licenses/                  # optional: multiple .key files
│   └── device-1.license.key
└── samples/                   # contents of this Samples repository
    ├── README.md
    ├── Makefile
    ├── data/
    └── 01_cg/ ... 35_dd_fgmres/
```

Use either the single `license.key` file or the `licenses/*.key` directory.
The license must be inside the package root, next to `include/` and `lib/`.

If the Samples directory is kept separately:

```text
workspace/
├── XianzhiKrylov/
│   ├── include/
│   ├── lib/
│   ├── license.key
│   └── licenses/
└── xianzhikrylov-samples/
    ├── README.md
    ├── Makefile
    └── 01_cg/ ... 35_dd_fgmres/
```

In this case, set `XK_ROOT` to `workspace/XianzhiKrylov`.
Do not merge the `lib/` or `include/` files from different vendor packages.

## Project Structure

```text
samples/
├── README.md                  # sample list, setup, and general help
├── Makefile                   # build or clean all samples
├── PATTERN_FORMAT.md          # format of the .csrpat matrix files
├── routes.json                # method and data settings for all samples
├── data/
│   ├── README.md              # size and model information for the data
│   ├── diffusion_1m.csrpat
│   ├── transport_1m.csrpat
│   ├── coupled4_500k.csrpat
│   └── complex_262k_3rhs.csrpat
├── 01_cg/
│   ├── sample.c               # runnable C program for this route
│   ├── sample_common.h         # local helper code used by sample.c
│   ├── solver.json            # iterative method and preconditioner settings
│   ├── Makefile               # build and run this sample
│   ├── README.md              # short guide for this sample
│   └── data/
│       └── diffusion_1m.csrpat -> ../../data/diffusion_1m.csrpat
├── ...
└── 35_dd_fgmres/
    ├── sample.c
    ├── sample_common.h
    ├── solver.json
    ├── Makefile
    ├── README.md
    └── data/
        └── diffusion_1m.csrpat -> ../../data/diffusion_1m.csrpat
```

The files work as follows:

- `README.md`: this file, which lists all samples and gives the first-run steps;
- `Makefile` at the top level: builds or cleans all 35 samples;
- `PATTERN_FORMAT.md`: explains how a `.csrpat` file is stored;
- `routes.json`: lists the method, preconditioner, matrix format, and data format for every sample;
- `data/`: stores the four matrix patterns shared by all samples;
- each numbered directory: one solver route and an independent sample program;
- `sample.c`: calls the public C API from start to finish;
- `sample_common.h`: reads A and b, converts matrix formats, and checks the result;
- `solver.json`: configuration file that selects the iterative method and preconditioner;
- the local `Makefile`: builds and runs one sample;
- the local `README.md`: gives short run steps for that sample;
- the local `data/` link: points to the shared pattern used by that sample.

## Samples List

| Sample | Solver route | Matrix format | Data format |
| --- | --- | --- | --- |
| [01_cg](01_cg/README.md) | cg / none | CSR | double |
| [02_bicg](02_bicg/README.md) | bicg / none | CSR | double |
| [03_bicgstab](03_bicgstab/README.md) | bicgstab / none | CSR | double |
| [04_gmres](04_gmres/README.md) | gmres / none | CSR | double |
| [05_fgmres](05_fgmres/README.md) | fgmres / none | CSR | double |
| [06_gcr](06_gcr/README.md) | gcr / none | CSR | double |
| [07_pcg_jacobi](07_pcg_jacobi/README.md) | pcg / jacobi | CSR | double |
| [08_fcg_jacobi](08_fcg_jacobi/README.md) | fcg / jacobi | CSR | double |
| [09_bicg_jacobi](09_bicg_jacobi/README.md) | bicg / jacobi | CSR | double |
| [10_bicgstab_jacobi](10_bicgstab_jacobi/README.md) | bicgstab / jacobi | CSR | double |
| [11_gmres_jacobi](11_gmres_jacobi/README.md) | gmres / jacobi | CSR | double |
| [12_fgmres_jacobi](12_fgmres_jacobi/README.md) | fgmres / jacobi | CSR | double |
| [13_gcr_jacobi](13_gcr_jacobi/README.md) | gcr / jacobi | CSR | double |
| [14_bicg_ilu0](14_bicg_ilu0/README.md) | bicg / ilu0 | CSR | double |
| [15_bicgstab_ilu0](15_bicgstab_ilu0/README.md) | bicgstab / ilu0 | CSR | double |
| [16_gmres_ilu0](16_gmres_ilu0/README.md) | gmres / ilu0 | CSR | double |
| [17_fgmres_ilu0](17_fgmres_ilu0/README.md) | fgmres / ilu0 | CSR | double |
| [18_gcr_ilu0](18_gcr_ilu0/README.md) | gcr / ilu0 | CSR | double |
| [19_pcg_ic0](19_pcg_ic0/README.md) | pcg / ic0 | CSR | double |
| [20_fcg_ic0](20_fcg_ic0/README.md) | fcg / ic0 | CSR | double |
| [21_gmres_ic0](21_gmres_ic0/README.md) | gmres / ic0 | CSR | double |
| [22_fgmres_ic0](22_fgmres_ic0/README.md) | fgmres / ic0 | CSR | double |
| [23_bicgstab_ic0](23_bicgstab_ic0/README.md) | bicgstab / ic0 | CSR | double |
| [24_bicg_ic0](24_bicg_ic0/README.md) | bicg / ic0 | CSR | double |
| [25_gcr_ic0](25_gcr_ic0/README.md) | gcr / ic0 | CSR | double |
| [26_bsr4_fgmres_ilu0](26_bsr4_fgmres_ilu0/README.md) | fgmres / ilu0 | BSR4 | double |
| [27_bsr4_fgmres_jacobi](27_bsr4_fgmres_jacobi/README.md) | fgmres / block_jacobi | BSR4 | double |
| [28_bsr4_fgmres_dilu](28_bsr4_fgmres_dilu/README.md) | fgmres / multicolor_block_dilu | BSR4 | double |
| [29_msr_bicgstab_jacobi](29_msr_bicgstab_jacobi/README.md) | bicgstab / jacobi | MSR | double |
| [30_ldu_pcg_dic](30_ldu_pcg_dic/README.md) | pcg / dic | LDU | double |
| [31_ldu_bicgstab_dilu](31_ldu_bicgstab_dilu/README.md) | bicgstab / dilu | LDU | double |
| [32_complex_batch3](32_complex_batch3/README.md) | fgmres_complex_batch / none | CSR | double complex |
| [33_float_fgmres_ilu0](33_float_fgmres_ilu0/README.md) | fgmres / ilu0 | CSR | float |
| [34_float_complex_batch3](34_float_complex_batch3/README.md) | fgmres_complex_batch / none | CSR | float complex |
| [35_dd_fgmres](35_dd_fgmres/README.md) | DD-FGMRES / LinglongAMG | CSR | double input / Double-Double solution |

The float and float-complex samples convert their data to the FP64 and
complex-FP64 descriptors needed by the current public C API. This does not mean
that the solver uses FP32 arithmetic. The DD sample uses FP64 matrix values and
returns a solution in two double planes.

## Requirements

Before running a sample, make sure you have:

- a Linux machine supported by the XianzhiKrylov package;
- a GPU vendor toolkit version compatible with that package;
- a C compiler and GNU Make;
- a valid license for the installed GPU.

A package is compatible when its README lists your CPU architecture, GPU model
or architecture, and CUDA Toolkit/DTK/MUSA SDK version. A newer
toolkit is not automatically supported. GPU driver selection is outside this
repository; use the driver supplied by the target system.

CMake is not required.

## First Run

This example uses `17_fgmres_ilu0`. It assumes that you downloaded the package
to `~/Downloads`.

### Step 1: Extract and enter the package

```bash
cd ~/Downloads
tar -xzf xianzhikrylov-*.tar.gz
cd XianzhiKrylov
```

The extracted directory name can differ between packages. If `cd XianzhiKrylov`
fails, run `ls`, then enter the directory containing `include/`, `lib/`, and
`samples/`.

Verify the package:

```bash
pwd
ls
test -f include/XianzhiKrylovAPI.h && echo "include OK"
test -f lib/libXianzhiKrylov.so && echo "lib OK"
test -d samples && echo "samples OK"
```

All three checks must print `OK`.

### Step 2: Install the license

For a single-device license, copy the received `.license.key` file to the
package root:

```bash
cp /path/to/xianzhikrylov-xxxx.license.key ./license.key
chmod 600 ./license.key
ls -l ./license.key
```

For multiple device licenses, place the key files under `licenses/`:

```bash
mkdir -p licenses
cp /path/to/*.license.key licenses/
ls -l licenses/
```

If the license arrives in a ZIP file:

```bash
unzip /path/to/xianzhikrylov-xxxx.licenses.zip -d licenses
find licenses -type f -name "*.key" -print
```

The `.key` files must be directly inside `licenses/`. If the archive creates an
extra directory level, move the key files up one level.

The license must match the machine, GPU, and package. Do not edit its contents.

On a cluster, request a GPU and run the sample on a compute node. Do not run the
solver on a login node without a GPU.

### Step 3: Select the GPU backend

The samples use `XK_BACKEND` to select the compiled backend:

| GPU backend | Common `XK_BACKEND` value |
| --- | --- |
| NVIDIA CUDA | `1` |
| Hygon DCU / ROCm / HIP | `2` |
| Moore Threads MUSA | `3` |
| SYCL | `4` |
| OpenCL | `5` |
| HCR | `6` |

If the package README lists another number, use that value.

### Step 4: Compile

From the package root:

```bash
export XK_ROOT="$PWD"
cd samples/17_fgmres_ilu0
make clean
make XK_BACKEND=1
```

For Hygon, replace the last command with:

```bash
make XK_BACKEND=2
```

Compilation creates an executable named `sample`.

### Step 5: Run

For NVIDIA:

```bash
make run XK_BACKEND=1
```

For Hygon:

```bash
make run XK_BACKEND=2
```

When it works, the output contains lines like:

```text
XianzhiKrylov status=1 converged=1 ... relative_residual=...
Independent residual RHS 1: relative_l2=... PASS
```

`status=1` and `converged=1` mean that the solver reached the stop condition in
`solver.json`. `PASS` means that the sample checked `r=b-Ax` on the CPU with the
original matrix.

Check the exit code:

```bash
echo $?
```

An exit code of `0` means success.

## Run Another Sample

You can build and run each sample by itself:

```bash
cd ..
cd 01_cg
make clean
make XK_BACKEND=1
make run XK_BACKEND=1
```

To build all samples:

```bash
cd /path/to/XianzhiKrylov/samples
make XK_BACKEND=1
```

If a build fails, look at the sample directory named just before the last
`error` or `Error` line.

## Command Reference

- `make`: compile the current sample into `./sample`;
- `make run`: compile if needed, then run with the XianzhiKrylov library path;
- `make clean`: remove the generated `sample`;
- `XK_BACKEND=<n>`: select the GPU backend used at compile time;
- `XK_ROOT=/path/to/XianzhiKrylov`: point a separate Samples checkout at the
  package that contains `include/` and `lib/`;
- `GPU_RUNTIME_LIB_DIR=/path/to/runtime/lib`: add a vendor runtime library
  directory when the system cannot find it.

For a separate Samples checkout:

```bash
export XK_ROOT="/path/to/XianzhiKrylov"
cd "/path/to/xianzhikrylov-samples/17_fgmres_ilu0"
make clean
make XK_ROOT="$XK_ROOT" XK_BACKEND=1
make run XK_ROOT="$XK_ROOT" XK_BACKEND=1
```

When `samples/` is inside the package, `XK_ROOT` is not needed. Each Makefile
finds the package root from its own directory.

## Data Flow

The samples create A and b in ordinary CPU memory and mark both descriptors as
`XIANZHIKRYLOV_HOST`.

1. `xianzhikrylov_init` creates the handle and selects the GPU backend.
2. `xianzhikrylov_set_system` copies the host arrays into GPU storage owned by
   the library and waits for the copy to finish.
3. `xianzhikrylov_solve_prepare` and `xianzhikrylov_solve` use the GPU copies.
4. `xianzhikrylov_solution_download` copies the solution back to a host buffer.

The library owns the uploaded A and b data, so the sample can free its CPU
copies after `set_system` returns.

If your program already keeps A or b in GPU memory, use
`XIANZHIKRYLOV_DEVICE` descriptors. The samples use host memory because it works
the same way on all supported GPU vendors.

## Sample Data

The four pattern files under `samples/data/` are shared by all 35 samples. The
matrix file inside each `data/` directory is a relative symbolic link to one of
those files. The four pattern files occupy about 90 MiB.

`.csrpat` stores the matrix order, CSR row offsets and column indices, scalar
class, model number, fixed seed, and RHS count. It does not store A values or b.
`sample_common.h` builds A in a fixed way and builds `b=A*x_exact`:

- real systems use an exact solution of all ones;
- the complex three-RHS sample uses three distinct constant complex vectors.

Every sample copies back x and checks the residual with the original system.

`xianzhikrylov_print_info` reports setup and solve time inside the library. It
does not include the CPU-to-GPU copy in `set_system`. If you report total run
time, also count your input work and the `set_system` call.

See [`PATTERN_FORMAT.md`](PATTERN_FORMAT.md) for the binary format.

## Troubleshooting

### How do I know whether the package is compatible?

Check every item below against the package README:

| Item | What to check |
| --- | --- |
| CPU architecture | Matches the package, for example `x86_64`. |
| Linux C library | Meets the minimum glibc version stated in the package README. |
| GPU vendor and backend | The package backend is CUDA, HIP, MUSA, SYCL, OpenCL, HCR, or another documented backend that supports the installed GPU. |
| GPU architecture | The GPU model or compiled target is listed as supported by the package, such as a CUDA SM target or Hygon `gfx` target. |
| Vendor toolkit | The installed CUDA Toolkit, DTK, MUSA SDK, or other vendor toolkit version is supported by the package. |

Use the package README for exact version and GPU support. A newer vendor toolkit
is not automatically supported.

GPU driver installation and driver-version selection are outside this
repository. Use the driver supplied by the target system or platform provider.

A license is also required. It must match the machine, GPU, and product for
which it was issued.

### `cc` or `make` is missing

Check the required build tools:

```bash
cc --version
make --version
```

On Ubuntu or Debian, install the basic build tools with:

```bash
sudo apt update
sudo apt install build-essential
```

Use the equivalent package manager on other Linux distributions.

### `XianzhiKrylovAPI.h: No such file or directory`

The build cannot find the package root. Check that the directory contains
`include/` and `lib/`. If the samples and package are in different directories,
pass:

```bash
XK_ROOT="/path/to/XianzhiKrylov"
```

to both `make` and `make run`.

### `Missing library: lib/libXianzhiKrylov.so`

The package is missing files, or `XK_ROOT` points to the wrong directory:

```bash
ls -l "$XK_ROOT/lib/libXianzhiKrylov.so"
```

### `cannot open shared object file`

The system cannot find a vendor runtime library. Find its directory in the
package README, then run:

```bash
export GPU_RUNTIME_LIB_DIR="/path/to/vendor/runtime/lib"
make run XK_BACKEND=1 GPU_RUNTIME_LIB_DIR="$GPU_RUNTIME_LIB_DIR"
```

Do not copy a `.so` file from a package built for another vendor.

### `init status=11` or `LICENSE_ERROR`

The license check failed. Check:

- `license.key` or `licenses/*.key` exists in the package root;
- the license has not expired;
- the license was issued for this machine and GPU;
- the package vendor and version match the license;
- the system time and license file have not been modified.

Do not edit the JSON file or replace the library to get around a license error.

### The backend value is wrong

The `XK_BACKEND` value does not match the selected package. Use the number in
the package README.

### Cannot find the matrix file

The link under `data/` is broken. Run from the original Git
checkout or extracted `.tar.gz`, or pass an absolute matrix path directly:

```bash
./sample /absolute/path/to/matrix.csrpat solver.json
```

Some ZIP tools do not keep symbolic links.

### `solve status=2`

The solver used all allowed iterations. Test the environment first with
`17_fgmres_ilu0`. For a different matrix, adjust the method, preconditioner,
restart length, or iteration limit in `solver.json`.

### Changes to the source do not appear

Recompile from a clean state:

```bash
make clean
make XK_BACKEND=1
```
