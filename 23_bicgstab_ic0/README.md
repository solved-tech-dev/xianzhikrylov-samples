# BiCGSTAB with IC(0)

This sample solves `Ax=b` with **BiCGSTAB** and **IC(0)** preconditioning.
Matrix format: `CSR`. Data format: `double`.

## Run this sample

The commands below assume that `samples/` is located inside an extracted
XianzhiKrylov package:

```text
XianzhiKrylov/
├── include/
├── lib/
└── samples/
```

### 1. Enter this sample

Replace `/path/to` with the directory where the package was extracted:

```bash
cd /path/to/XianzhiKrylov/samples/23_bicgstab_ic0
pwd
```

The final directory printed by `pwd` should be `23_bicgstab_ic0`. If the directory does not
exist, run `cd /path/to/XianzhiKrylov/samples` followed by `ls` to display the
available sample directories.

### 2. Verify the license

For a first run, place the license in the package root:

```bash
cp /path/to/xianzhikrylov-xxxx.license.key ../../license.key
chmod 600 ../../license.key
ls -l ../../license.key
```

If you already placed a license according to the
[main Samples guide](../README.md), skip this step.

### 3. Compile

For NVIDIA CUDA:

```bash
make clean
make XK_BACKEND=1
```

For Hygon DCU, ROCm, or HIP:

```bash
make clean
make XK_BACKEND=2
```

For another vendor, replace the numeric value after `XK_BACKEND=` with the
backend number documented in that vendor package.

Compilation creates an executable named `sample`.

### 4. Run

NVIDIA CUDA:

```bash
make run XK_BACKEND=1
```

Hygon DCU, ROCm, or HIP:

```bash
make run XK_BACKEND=2
```

`make run` sets the XianzhiKrylov library path for the duration of the program.
It is not necessary to edit `LD_LIBRARY_PATH` for the normal run.

Successful output ends with a line similar to:

```text
Independent residual RHS 1: relative_l2=... absmax=... PASS
```

`PASS` means that the program downloaded the solution and independently checked
the original system `r=b-Ax` on the CPU. Check the process exit code with:

```bash
echo $?
```

An exit code of `0` means success.

## Build from a standalone Samples checkout

When this directory is not inside the XianzhiKrylov package, point `XK_ROOT` at
the directory containing `include/` and `lib/`:

```bash
export XK_ROOT="/path/to/XianzhiKrylov"
make clean
make XK_ROOT="$XK_ROOT" XK_BACKEND=1
make run XK_ROOT="$XK_ROOT" XK_BACKEND=1
```

Replace `XK_BACKEND=1` with the backend number for the selected package.

## Files

- `sample.c`: complete call sequence through the public C API;
- `sample_common.h`: sample-only matrix reading, conversion, and residual checks;
- `solver.json`: configuration file that selects the iterative method and preconditioner;
- `Makefile`: build and run commands;
- `data/diffusion_1m.csrpat`: default sparse input pattern;
- `README.md`: this run guide.

## Input data

The `.csrpat` files contain only the sparse pattern and deterministic generation
metadata. Values for A are generated from a fixed seed, and b is constructed
from a known exact solution. See
[`../PATTERN_FORMAT.md`](../PATTERN_FORMAT.md) for the binary layout.

All 35 samples share the files under `../data/`. The matrix file in this
directory is a relative symbolic link, so copying this directory elsewhere
requires either copying the shared data directory or passing another matrix path
to `sample`.

## Common problems

- `XianzhiKrylovAPI.h: No such file or directory`: set `XK_ROOT` to the package
  root when samples and library are stored separately.
- `Missing library`: verify that `XK_ROOT/lib/libXianzhiKrylov.so` exists.
- `init status=11` or `LICENSE_ERROR`: check that the license exists, is current,
  and was issued for this GPU.
- Backend is unsupported: use the `XK_BACKEND` number documented by the selected
  package.
- A vendor runtime `.so` is missing: pass its directory through
  `GPU_RUNTIME_LIB_DIR`:

```bash
make run XK_BACKEND=1 GPU_RUNTIME_LIB_DIR="/path/to/vendor/runtime/lib"
```

- Source changes do not take effect: run `make clean` before rebuilding.

See the [main Samples guide](../README.md) for prerequisites, license setup, and
more detailed troubleshooting.
