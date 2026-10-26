# XianzhiKrylov Product Guide

## Overview

XianzhiKrylov solves sparse linear systems

```text
A x = b
```

with GPU Krylov methods and preconditioners. Packages for supported GPU
vendors expose the same public C interface. The installed package determines
the vendor backend, GPU architectures, and available solver combinations.

The application prepares a matrix object and dense vector objects, submits
them to a compute handle, executes one or more phases, and reads the solution
from the output vector. Algorithm settings are selected by a JSON file rather
than by command-line flags or per-call option structures.

This document describes the product API. The top-level
[README](../README.md) describes the sample repository and how to run its
examples.

## Package Layout

An extracted package has this public layout:

```text
XianzhiKrylov/
├── README.md
├── include/
│   ├── XianzhiKrylovAPI.h
│   └── XianzhiKrylovData.h
├── lib/
│   ├── libXianzhiKrylov.so
│   └── ...
├── samples/
├── license.key
└── THIRD_PARTY_NOTICES.txt
```

The exact runtime libraries and GPU architecture subdirectories are package
specific. Keep the extracted package structure intact. Place the license file
from the store at the package root as `license.key`.

Applications include:

```c
#include "XianzhiKrylovAPI.h"
```

and link with `-lXianzhiKrylov`.

## Numerical Data

### Matrix Formats

`xianzhikrylov_matrix_data_t` supports:

| Format | Main storage |
| --- | --- |
| CSR | `row_offsets`, `column_indices`, `values` |
| BSR | block row offsets, block column indices, block values, block size and block layout |
| MSR | diagonal-separated offsets, column indices, diagonal values and off-diagonal values |
| LDU | diagonal values plus lower/upper addresses and face values |

Matrix patterns are zero based, sorted, duplicate free, and have no hidden
format conversion. The selected method/preconditioner combination must support
the requested format.

### Number Types

| Public constant | Storage |
| --- | --- |
| `XIANZHIKRYLOV_FP64` | `double` |
| `XIANZHIKRYLOV_COMPLEX_FP64` | `xianzhikrylov_complex_t` |
| `XIANZHIKRYLOV_FP32` | `float` |
| `XIANZHIKRYLOV_COMPLEX_FP32` | `xianzhikrylov_complex_fp32_t` |
| `XIANZHIKRYLOV_DD` | two FP64 planes |

FP32, complex, and Double-Double support depends on the package and selected
method. A package returns `XIANZHIKRYLOV_UNSUPPORTED` rather than silently
changing precision.

### Dense Vectors

A vector object describes one or more RHS or solution columns:

- `XIANZHIKRYLOV_RHS_MAJOR` stores `(row,rhs)` at
  `data[rhs*leading_dimension + row]`.
- `XIANZHIKRYLOV_ROW_MAJOR` stores `(row,rhs)` at
  `data[row*leading_dimension + rhs]`.
- `HOST` and `DEVICE` memory spaces are available where the package supports
  them.
- Double-Double vectors use a second `low` plane.

The single-vector function creates a compact one-column RHS-major vector. Use
`xianzhikrylov_multiple_vector_create` for explicit layout, stride, multiple
RHS, or Double-Double input.

## Public C API

The public interface has ten functions.

### 1. `xianzhikrylov_create`

```c
xianzhikrylov_status_t xianzhikrylov_create(
    int32_t device_id,
    xianzhikrylov_handle_t *handle);
```

Creates a device-bound compute handle. `device_id` is the visible process
device ordinal. The installed package selects the vendor backend.

### 2. `xianzhikrylov_config_set`

```c
xianzhikrylov_status_t xianzhikrylov_config_set(
    xianzhikrylov_handle_t handle,
    xianzhikrylov_config_option_t option,
    const char *value);
```

Loads the solver JSON file. ABI 3 currently defines one option:

```c
XIANZHIKRYLOV_CONFIG_FILE
```

Call this function before the first `execute` that depends on the
configuration. The library copies the path and does not retain the caller's
string.

The JSON file selects:

- Krylov method and method parameters;
- preconditioner and numerical controls;
- relative/absolute tolerances and residual norm;
- zero, provided-vector, or reuse initial-guess policy;
- optional external preconditioner provider and provider configuration.

### 3. `xianzhikrylov_matrix_create`

```c
xianzhikrylov_status_t xianzhikrylov_matrix_create(
    xianzhikrylov_matrix_t *matrix,
    const xianzhikrylov_matrix_data_t *data);
```

Creates a matrix descriptor. This operation does not allocate the complete
device matrix, analyze the sparsity pattern, or build a preconditioner.

The data structure starts with:

```c
data.abi_version = XIANZHIKRYLOV_ABI_VERSION;
data.struct_size = sizeof(data);
```

Unused format-specific buffers must contain a null pointer and zero byte size.
The descriptor keeps the caller's array addresses; it does not own the arrays.

### 4. `xianzhikrylov_vector_create`

```c
xianzhikrylov_status_t xianzhikrylov_vector_create(
    xianzhikrylov_vector_t *vector,
    int64_t size,
    void *data,
    xianzhikrylov_value_type_t value_type,
    xianzhikrylov_memory_space_t memory_space);
```

Creates a compact one-column vector. It is equivalent to a multiple-vector
object with:

```text
rhs_count = 1
layout = XIANZHIKRYLOV_RHS_MAJOR
leading_dimension = size
```

Double-Double vectors must use the multiple-vector function so that both planes
can be supplied.

### 5. `xianzhikrylov_multiple_vector_create`

```c
xianzhikrylov_status_t xianzhikrylov_multiple_vector_create(
    xianzhikrylov_vector_t *vector,
    int64_t length,
    int64_t rhs_count,
    int64_t leading_dimension,
    xianzhikrylov_layout_t layout,
    void *data,
    void *low,
    xianzhikrylov_value_type_t value_type,
    xianzhikrylov_memory_space_t memory_space);
```

Creates a vector for RHS, an initial solution, or a solution destination.
The function does not allocate or copy the dense buffer. `low` is required for
Double-Double and must be null for other value types.

### 6. `xianzhikrylov_execute`

```c
xianzhikrylov_status_t xianzhikrylov_execute(
    xianzhikrylov_handle_t handle,
    xianzhikrylov_phase_t phase,
    xianzhikrylov_matrix_t matrix,
    xianzhikrylov_vector_t solution,
    xianzhikrylov_vector_t rhs);
```

Executes a preparation or solve phase.

| Phase | `matrix` | `solution` | `rhs` | Action |
| --- | --- | --- | --- | --- |
| `XIANZHIKRYLOV_PHASE_ANALYSIS` | required | null | null | structural analysis and planning |
| `XIANZHIKRYLOV_PHASE_NUMERIC_PREPARE` | required | null | null | numerical preconditioner preparation |
| `XIANZHIKRYLOV_PHASE_SOLVE` | required | required | required | solve using prepared data |
| `XIANZHIKRYLOV_PHASE_ALL` | required | required | required | complete preparation and solve |

`ANALYSIS` and `NUMERIC_PREPARE` return `XIANZHIKRYLOV_SUCCESS`.
`SOLVE` and `ALL` return `XIANZHIKRYLOV_CONVERGED` only when every RHS meets
its configured stopping criterion.

If the solution object uses `XIANZHIKRYLOV_HOST`, the final solution is written
directly to the caller's host buffer. No separate download function exists.

Reuse rules:

- unchanged A and b: repeat `SOLVE`;
- unchanged structure and changed A values: `NUMERIC_PREPARE`, then `SOLVE`;
- changed structure: `ANALYSIS`, `NUMERIC_PREPARE`, then `SOLVE`;
- no phase separation: `ALL`.

### 7. `xianzhikrylov_print_info`

```c
xianzhikrylov_status_t xianzhikrylov_print_info(
    xianzhikrylov_handle_t handle);
```

Prints the most recent solve status, per-RHS iterations, method ID, residual,
RHS norm, and preparation/solve timing. The function does not rerun the solve.

### 8. `xianzhikrylov_matrix_destroy`

```c
xianzhikrylov_status_t xianzhikrylov_matrix_destroy(
    xianzhikrylov_matrix_t matrix);
```

Destroys the descriptor only. The library never frees application arrays.
Set the caller's object variable to null after destruction.

### 9. `xianzhikrylov_vector_destroy`

```c
xianzhikrylov_status_t xianzhikrylov_vector_destroy(
    xianzhikrylov_vector_t vector);
```

Destroys a single-RHS or multiple-RHS vector descriptor without freeing its
dense buffer.

### 10. `xianzhikrylov_destroy`

```c
xianzhikrylov_status_t xianzhikrylov_destroy(
    xianzhikrylov_handle_t handle);
```

Releases library-owned workspace, prepared data, and external-provider state.
Cleanup does not require a valid runtime license.

## JSON Configuration

A standard FGMRES + ILU(0) configuration is:

```json
{
  "schema_version": 1,
  "matrix_format": "csr",
  "block_size": 1,
  "solver": {
    "method": "fgmres",
    "preconditioner": "ilu0",
    "max_iterations": 2000,
    "restart": 40,
    "relative_tolerance": 1e-9,
    "absolute_tolerance": 0,
    "initial_guess": "zero"
  }
}
```

Supported `initial_guess` values are:

- `zero`: the library starts from zero;
- `provided`: the current solution-vector contents are the initial guess and
  are overwritten by the final solution;
- `reuse`: use the previous internal solution where the route supports it.

For `provided`, initialize the solution vector before `SOLVE` or `ALL`.

The residual stopping rule is:

```text
norm_p(r) <= max(atol, rtol * norm_p(b))
```

where `r = b - A x`. `residual_norm` may select `l1`, `l2`, or `linf` on
supported packages. Changing the stopping norm does not replace required
internal inner products used by a Krylov algorithm.

## LinglongAMG and External Preconditioners

A package may integrate LinglongAMG or another external provider behind the
same ten public functions. The provider is selected by JSON:

```json
{
  "preconditioner": {
    "type": "amg",
    "provider": "linglongamg",
    "library_path": "/path/to/liblinglongamg.so",
    "config_path": "/path/to/amg_config.json"
  }
}
```

When that integration is installed:

- `ANALYSIS` may perform provider capability or structural checks;
- `NUMERIC_PREPARE` builds the AMG hierarchy and numerical state;
- `SOLVE` runs the outer Krylov method and calls one provider cycle per
  preconditioner application;
- `ALL` performs the complete sequence.

The XianzhiKrylov and provider licenses are independent. A missing or invalid
provider configuration returns an error; the library does not silently switch
to another preconditioner.

## Quick Start with `17_fgmres_ilu0`

`17_fgmres_ilu0` is the recommended first sample. It uses a general CSR matrix
and the common FGMRES + ILU(0) route.

Place the extracted package under `libraries/`:

```text
libraries/
└── XianzhiKrylov-<package-name>/
    ├── include/
    ├── lib/
    └── license.key
```

Build and run:

```bash
make -C 17_fgmres_ilu0
make -C 17_fgmres_ilu0 run
```

If several packages are installed:

```bash
make -C 17_fgmres_ilu0 run \
  LIBRARY_ROOT=/path/to/XianzhiKrylov-<package-name>
```

The solve portion of the sample follows this sequence:

```c
xianzhikrylov_handle_t handle = NULL;

xianzhikrylov_create(0, &handle);
xianzhikrylov_config_set(
    handle, XIANZHIKRYLOV_CONFIG_FILE, "config.json");

xianzhikrylov_matrix_create(
    &matrix_object, &matrix_data);
xianzhikrylov_multiple_vector_create(
    &rhs_object, rows, rhs_count, rows,
    XIANZHIKRYLOV_RHS_MAJOR, rhs_values, NULL,
    XIANZHIKRYLOV_FP64, XIANZHIKRYLOV_HOST);
xianzhikrylov_multiple_vector_create(
    &solution_object, rows, rhs_count, rows,
    XIANZHIKRYLOV_RHS_MAJOR, solution_values, NULL,
    XIANZHIKRYLOV_FP64, XIANZHIKRYLOV_HOST);

xianzhikrylov_execute(
    handle, XIANZHIKRYLOV_PHASE_ALL,
    matrix_object, solution_object, rhs_object);

xianzhikrylov_print_info(handle);
```

Production code must check every returned status. The complete sample also
computes `b - A x` independently and verifies its residual.

## Errors and Result Rules

Important status values:

| Status | Meaning |
| --- | --- |
| `XIANZHIKRYLOV_SUCCESS` | an action completed |
| `XIANZHIKRYLOV_CONVERGED` | every RHS met the stopping criterion |
| `XIANZHIKRYLOV_MAX_ITERATIONS` | iteration limit reached |
| `XIANZHIKRYLOV_INVALID_ARGUMENT` | malformed descriptor or argument |
| `XIANZHIKRYLOV_UNSUPPORTED` | route, format, precision, or provider is unavailable |
| `XIANZHIKRYLOV_STATE_ERROR` | calls were made in an invalid lifecycle state |
| `XIANZHIKRYLOV_LICENSE_ERROR` | runtime authorization failed |

`MAX_ITERATIONS`, `BREAKDOWN`, and `STAGNATED` are not successful numerical
results. Do not relax tolerances or modify the RHS to turn a failed residual
check into a pass.

## Ownership Summary

- The application owns all matrix and dense-vector buffers.
- Matrix/vector descriptors hold borrowed addresses.
- Keep buffers valid through every `execute` call that consumes the object.
- Destroy data objects during application cleanup.
- Call `xianzhikrylov_destroy` once after all work on that handle is complete.
- Serialize calls that share one handle.

See [TROUBLESHOOTING.md](TROUBLESHOOTING.md) for build, loading, device,
license, and data-file diagnostics.
