# XianzhiKrylov Samples

This repository contains 35 independent C samples for the XianzhiKrylov C API.
Each sample demonstrates the complete call sequence: construct A and b, attach
the system, prepare the solver, solve, download x, and verify the residual.

Each directory contains:

- `sample.c`: the runnable sample program;
- `sample_common.h`: sample-only matrix reading, format conversion, and residual checks;
- `solver.json`: configuration file that selects the iterative method and preconditioner;
- `Makefile`: build and run commands;
- `README.md`: instructions for that sample;
- `data/`: contains a relative link to the shared sample matrix.

XianzhiKrylov packages and licenses are available from
[https://store.solved-tech.com/](https://store.solved-tech.com/).

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
| [35_dd_fgmres_amg](35_dd_fgmres_amg/README.md) | DD-FGMRES / LinglongAMG | CSR | double input / Double-Double solution |

The float and float-complex samples explicitly promote their sample buffers to
the FP64 and complex-FP64 descriptors required by the current public matrix
interface. They demonstrate how those data formats connect to the library; they
do not claim native FP32 solver arithmetic. The DD sample uses FP64 matrix
coefficients and returns a solution in two double planes.

## Requirements

Before running a sample, make sure you have:

- a Linux machine supported by the XianzhiKrylov package;
- a GPU driver and vendor runtime compatible with that package;
- a C compiler and GNU Make;
- a valid license for the installed GPU.

A package is compatible when its documentation lists your CPU architecture,
GPU model or architecture, and CUDA/DTK/MUSA runtime version. A newer runtime
is not automatically supported. See the package README for the exact versions.

CMake is not required.

## First Run

The following example uses `17_fgmres_ilu0`. It assumes that the package was
downloaded to `~/Downloads`.

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

If the package README specifies another number, use that value.

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

Successful output contains lines similar to:

```text
XianzhiKrylov status=1 converged=1 ... relative_residual=...
Independent residual RHS 1: relative_l2=... PASS
```

`status=1` and `converged=1` mean that the configured stopping criterion was
reached. `PASS` means that the sample independently checked the original system
`r=b-Ax` on the CPU.

Check the exit code:

```bash
echo $?
```

An exit code of `0` means success.

## Run Another Sample

Each sample directory can be built and run independently:

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

If a build fails, inspect the sample directory reported immediately before the
last `error` or `Error` line.

## Command Reference

- `make`: compile the current sample into `./sample`;
- `make run`: compile if needed, then run with the XianzhiKrylov library path;
- `make clean`: remove the generated `sample`;
- `XK_BACKEND=<n>`: select the GPU backend used at compile time;
- `XK_ROOT=/path/to/XianzhiKrylov`: point a standalone Samples checkout at the
  package containing `include/` and `lib/`;
- `GPU_RUNTIME_LIB_DIR=/path/to/runtime/lib`: add a vendor runtime library
  directory when it is not available in the system loader path.

For a standalone Samples checkout:

```bash
export XK_ROOT="/path/to/XianzhiKrylov"
cd "/path/to/xianzhikrylov-samples/17_fgmres_ilu0"
make clean
make XK_ROOT="$XK_ROOT" XK_BACKEND=1
make run XK_ROOT="$XK_ROOT" XK_BACKEND=1
```

When `samples/` is inside the package, `XK_ROOT` is optional because each
Makefile resolves the package root relative to its own directory.

## Data Flow

The samples create A and b in ordinary CPU memory and mark both descriptors as
`XIANZHIKRYLOV_HOST`.

1. `xianzhikrylov_init` creates the handle and selects the GPU backend.
2. `xianzhikrylov_set_system` copies the host arrays into library-owned GPU
   storage and synchronizes before returning.
3. `xianzhikrylov_solve_prepare` and `xianzhikrylov_solve` use the GPU copies.
4. `xianzhikrylov_solution_download` copies the solution back to a host buffer.

The library owns the uploaded A and b buffers, so the sample may release its
temporary host copies after `set_system` returns.

Applications that already store A or b in device memory can use
`XIANZHIKRYLOV_DEVICE` descriptors instead. The samples use the host path because
it works uniformly across the supported GPU vendors.

## Sample Data

The four pattern files under `samples/data/` are shared by all 35 samples. The
matrix file inside each `data/` directory is a relative symbolic link to one of
those files. The four pattern files occupy about 90 MiB.

`.csrpat` stores the matrix order, CSR row offsets and column indices, scalar
class, model identifier, fixed seed, and RHS count. It does not store A values or
b. `sample_common.h` generates A deterministically and constructs
`b=A*x_exact`:

- real systems use an exact solution of all ones;
- the complex three-RHS sample uses three distinct constant complex vectors.

Every sample downloads the computed solution and performs an independent
residual check on the original system.

`xianzhikrylov_print_info` reports preparation and solve time inside the library;
the host-to-device transfer in `set_system` is not included. Applications that
report complete end-to-end time should account for their own preparation and the
`set_system` call separately.

See [`PATTERN_FORMAT.md`](PATTERN_FORMAT.md) for the binary format.

## Troubleshooting

### How do I know whether the package is compatible?

Check every item below against the package README:

| Item | What to check |
| --- | --- |
| CPU architecture | Matches the package, for example `x86_64`. |
| Linux C library | Meets the minimum glibc version stated in the package documentation. |
| GPU vendor and backend | The package backend is CUDA, HIP, MUSA, SYCL, OpenCL, HCR, or another documented backend that supports the installed GPU. |
| GPU architecture | The GPU model or compiled target is listed as supported by the package, such as a CUDA SM target or Hygon `gfx` target. |
| Vendor runtime | The installed CUDA, DTK, MUSA, SYCL, OpenCL, or HCR runtime belongs to a version compatible with the package. |
| Driver | The installed driver supports the required vendor runtime and exposes the selected GPU. |

The package documentation is authoritative for version-specific and
architecture-specific support. A newer vendor runtime is not automatically
compatible unless the package documentation says so.

The license is a separate requirement: it must match the machine, GPU, and
product for which it was issued.

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

The build is not pointing at the package root. Check that the directory contains
`include/` and `lib/`. For a standalone Samples checkout, pass:

```bash
XK_ROOT="/path/to/XianzhiKrylov"
```

to both `make` and `make run`.

### `Missing library: lib/libXianzhiKrylov.so`

The package is incomplete or `XK_ROOT` is incorrect:

```bash
ls -l "$XK_ROOT/lib/libXianzhiKrylov.so"
```

### `cannot open shared object file`

A vendor runtime library is missing from the loader path. Find the runtime
directory in the package README, then run:

```bash
export GPU_RUNTIME_LIB_DIR="/path/to/vendor/runtime/lib"
make run XK_BACKEND=1 GPU_RUNTIME_LIB_DIR="$GPU_RUNTIME_LIB_DIR"
```

Do not copy a `.so` file from a package built for another vendor.

### `init status=11` or `LICENSE_ERROR`

The authorization check failed. Check:

- `license.key` or `licenses/*.key` exists in the package root;
- the license has not expired;
- the license was issued for this machine and GPU;
- the package vendor and version match the license;
- the system time and license file have not been modified.

Do not edit the JSON file or replace the library to bypass an authorization
error.

### The backend is unsupported

The `XK_BACKEND` value does not match the selected package. Use the number
documented in the package README.

### A matrix path does not exist

The relative link under `data/` was not preserved. Run from the original Git
checkout or extracted `.tar.gz`, or pass an absolute matrix path directly:

```bash
./sample /absolute/path/to/matrix.csrpat solver.json
```

Some ZIP tools do not preserve symbolic links.

### `solve status=2`

The solver reached its maximum iteration count. Check the environment first with
`17_fgmres_ilu0`. For a different matrix, adjust the method, preconditioner,
restart length, or iteration limit in `solver.json`.

### Source changes do not take effect

Recompile from a clean state:

```bash
make clean
make XK_BACKEND=1
```
