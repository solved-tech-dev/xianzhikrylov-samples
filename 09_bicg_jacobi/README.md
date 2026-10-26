# BiCG with Jacobi

This sample shows how to solve `Ax=b` using **BiCG** with the **Jacobi** preconditioner.
Matrix format: `CSR`. Data format: `double`.

## Run this sample

These commands assume that `samples/` is inside an extracted XianzhiKrylov
package:

```text
XianzhiKrylov/
├── include/
├── lib/
└── samples/
```

### 1. Enter this sample

Change `/path/to` to the directory where you extracted the package:

```bash
cd /path/to/XianzhiKrylov/samples/09_bicg_jacobi
pwd
```

The final directory printed by `pwd` should be `09_bicg_jacobi`. If the directory does not
exist, run `cd /path/to/XianzhiKrylov/samples` and `ls` to list the samples.

### 2. Check the license

On first use, copy the license to the package root:

```bash
cp /path/to/xianzhikrylov-xxxx.license.key ../../license.key
chmod 600 ../../license.key
ls -l ../../license.key
```

If the license is already installed, skip this step.

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

For another vendor, replace the number after `XK_BACKEND=` with the backend
number listed in that package's README.

The build creates an executable named `sample`.

### 4. Run

NVIDIA CUDA:

```bash
make run XK_BACKEND=1
```

Hygon DCU, ROCm, or HIP:

```bash
make run XK_BACKEND=2
```

`make run` sets the XianzhiKrylov library path while the program runs. You do not
need to edit `LD_LIBRARY_PATH` for a normal run.

When it works, the output ends with a line like:

```text
Independent residual RHS 1: relative_l2=... absmax=... PASS
```

`PASS` means that the sample copied x back to the CPU and checked `r=b-Ax` with
the original matrix. Check the exit code with:

```bash
echo $?
```

An exit code of `0` means success.

## Run from a separate Samples download

If this sample is outside the XianzhiKrylov package, set `XK_ROOT` to the
directory that contains `include/` and `lib/`:

```bash
export XK_ROOT="/path/to/XianzhiKrylov"
make clean
make XK_ROOT="$XK_ROOT" XK_BACKEND=1
make run XK_ROOT="$XK_ROOT" XK_BACKEND=1
```

Change `XK_BACKEND=1` to the value for your package.

## Files

- `sample.c`: a full solve example that uses the public C API;
- `sample_common.h`: helper code for reading A and b, converting formats, and checking the result;
- `solver.json`: configuration file that selects the iterative method and preconditioner;
- `Makefile`: build and run commands;
- `data/transport_1m.csrpat`: the sample matrix pattern;
- `README.md`: this run guide.

## Input data

The `.csrpat` files contain only the sparse pattern and fixed settings. The
program builds A from a fixed seed and builds b from a known exact solution. See
[`../PATTERN_FORMAT.md`](../PATTERN_FORMAT.md) for the binary layout.

All 35 samples share the files under `../data/`. The file in this `data/`
directory links to that shared data. If you copy this sample, also copy the
shared data or pass another matrix path to `sample`.

## Common problems

- `XianzhiKrylovAPI.h: No such file or directory`: set `XK_ROOT` to the package
  root when this sample and the package are in different directories.
- `Missing library`: check that `XK_ROOT/lib/libXianzhiKrylov.so` exists.
- `init status=11` or `LICENSE_ERROR`: check that the license exists, is valid,
  and was issued for this GPU.
- The backend is not supported: use the `XK_BACKEND` value listed in the selected
  package README.
- A vendor runtime `.so` is missing: pass its directory through
  `GPU_RUNTIME_LIB_DIR`:

```bash
make run XK_BACKEND=1 GPU_RUNTIME_LIB_DIR="/path/to/vendor/runtime/lib"
```

- Source changes do not take effect: run `make clean` before you rebuild.

See the [main Samples guide](../README.md) for prerequisites, license setup, and
more detailed troubleshooting.
