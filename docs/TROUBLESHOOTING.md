# Troubleshooting

The complete API and configuration reference is in
[XianzhiKrylov_Manual_26.44_CN.pdf](XianzhiKrylov_Manual_26.44_CN.pdf).

## Does the package work on this machine?

Check these items in the package README:

| Item | What to check |
| --- | --- |
| CPU | The package supports your CPU architecture, for example `x86_64`. |
| Linux C library | The package supports your glibc version. |
| GPU | The package lists your GPU model and compiled GPU target. |
| GPU platform | The package is built for the installed GPU and its required vendor toolkit/runtime. |
| Runtime environment | The installed toolkit/runtime matches the selected package. |

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
`include/` and `lib/`. Place the package under
`libraries/<package-name>/`, or select it with:

```bash
make LIBRARY_ROOT="/path/to/package"
make run LIBRARY_ROOT="/path/to/package"
```

## `Missing library: lib/libXianzhiKrylov.so`

The package is missing files, or `LIBRARY_ROOT` points to the wrong directory:

```bash
ls -l "$LIBRARY_ROOT/lib/libXianzhiKrylov.so"
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
make run GPU_RUNTIME_LIB_DIR="$GPU_RUNTIME_LIB_DIR"
```

Do not copy a `.so` file from a package built for another vendor.

## `config_set status=11` or `LICENSE_ERROR`

The license check failed. Check:

- `license.key` is in the selected package directory;
- the package and license were downloaded from
  <https://store.solved-tech.com/>;
- the license is still valid;
- the license was issued for this machine and GPU;
- the package matches the license.

## The package does not match the GPU

Download the library package built for the target GPU and its required vendor
toolkit/runtime from <https://store.solved-tech.com/>. Use the package provided
for the target GPU.

## Cannot find the matrix file

Use the repository's `matrices/` directory, or pass a real matrix path and the
sample configuration:

```bash
./sample /path/to/matrix.csrpat config.json
```

## `execute status=2`

The solver used all allowed iterations. First test the setup with
`17_fgmres_ilu0`. For another matrix, change the method, preconditioner,
restart length, or iteration limit in `config.json`.

## Changes to the source do not appear

Run a clean rebuild:

```bash
make clean
make
```
