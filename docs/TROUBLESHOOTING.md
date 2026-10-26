# Troubleshooting

## Does the package work on this machine?

Check these items in the package README:

| Item | What to check |
| --- | --- |
| CPU | The package supports your CPU architecture, for example `x86_64`. |
| Linux C library | The package supports your glibc version. |
| GPU | The package lists your GPU model and compiled GPU target. |
| Backend | The package uses CUDA, HIP, MUSA, SYCL, OpenCL, HCR, or another documented backend. |
| Vendor toolkit | The installed CUDA Toolkit, DTK, MUSA SDK, or other toolkit version is supported. |

GPU driver installation and driver selection are outside this repository. Use
the driver supplied by the target system or cluster provider.

## `cc` or `make` is missing

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

Use the same kind of package command on other Linux systems.

## `XianzhiKrylovAPI.h: No such file or directory`

The build cannot find the package root. Check that the directory contains
`include/` and `lib/`. If the samples and package are in different directories,
set:

```bash
export XK_ROOT="/path/to/XianzhiKrylov"
```

Use this value with both `make` and `make run`.

## `Missing library: lib/libXianzhiKrylov.so`

The package is missing files, or `XK_ROOT` points to the wrong directory:

```bash
ls -l "$XK_ROOT/lib/libXianzhiKrylov.so"
```

## `cannot open shared object file`

The system cannot find a shared library. First list the dependencies of the
sample:

```bash
ldd ./sample
```

Look for `not found`. The missing file can be the XianzhiKrylov library, a GPU
runtime library, or another indirect dependency.

For a vendor runtime library, add its directory:

```bash
export GPU_RUNTIME_LIB_DIR="/path/to/vendor/runtime/lib"
make run XK_BACKEND=1 GPU_RUNTIME_LIB_DIR="$GPU_RUNTIME_LIB_DIR"
```

Do not copy a `.so` file from a package built for another vendor.

## `init status=11` or `LICENSE_ERROR`

The license check failed. Check:

- `license.key` or `licenses/*.key` is in the package root;
- the license is still valid;
- the license was issued for this machine and GPU;
- the package matches the license.

## The backend value is wrong

The `XK_BACKEND` value does not match the package. Use the value from the
package README.

## Cannot find the matrix file

The link under `data/` is broken. Use the original Git checkout or extracted
`.tar.gz`, or pass a real matrix path:

```bash
./sample /path/to/matrix.csrpat solver.json
```

Some ZIP tools do not keep symbolic links.

## `solve status=2`

The solver used all allowed iterations. First test the setup with
`17_fgmres_ilu0`. For another matrix, change the method, preconditioner,
restart length, or iteration limit in `solver.json`.

## Changes to the source do not appear

Run a clean rebuild:

```bash
make clean
make XK_BACKEND=1
```
