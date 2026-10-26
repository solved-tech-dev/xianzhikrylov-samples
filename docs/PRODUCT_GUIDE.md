# XianzhiKrylov C API Guide

## Scope

XianzhiKrylov solves sparse linear systems `Ax=b` with GPU Krylov methods and
preconditioners. Packages from different GPU vendors use the same C API. Place
an extracted package under `libraries/` or set `LIBRARY_ROOT` to its root.
The runtime license must be stored as `license.key` in that same package root.

The package README defines the supported CPU, glibc, GPU architectures, vendor
toolkit, and available route set. A newer toolkit is not automatically
supported.

## Data Model

Initialize every public structure with:

```c
memset(&value, 0, sizeof(value));
value.abi_version = XIANZHIKRYLOV_DATA_ABI;
value.struct_size = sizeof(value);
```

### Matrix

`xianzhikrylov_matrix` describes A:

- `rows` and `columns` are scalar dimensions for CSR and LDU, block
  dimensions for BSR and MSR.
- `entries` counts CSR nonzeros, BSR/MSR nonzero blocks, or LDU faces.
- `row_offsets` uses `int32_t` for CSR/MSR/LDU and `uint64_t` for BSR.
- `column_indices` follows the same index rule.
- Columns in each row must be sorted, unique, zero based, and in range.
- CSR, BSR, and LDU require an explicit diagonal. MSR stores its diagonal
  separately.
- `values` holds row-major coefficients. MSR also uses
  `offdiagonal_values`; LDU uses `lower_values` and `upper_values`.

### Scalar Types

| Constant | C storage |
| --- | --- |
| `XIANZHIKRYLOV_FP64` | `double` |
| `XIANZHIKRYLOV_COMPLEX_FP64` | `xianzhikrylov_complex` |
| `XIANZHIKRYLOV_FP32` | `float` |
| `XIANZHIKRYLOV_COMPLEX_FP32` | `xianzhikrylov_complex_fp32` |
| `XIANZHIKRYLOV_DD` | Two FP64 planes, high in `data`/`values` and low in `low`/`matrix_planes` |

The FP32, complex FP32, and Double-Double coefficient routes currently accept
host CSR input. They use the declared precision for matrix-vector products,
vector updates, solver arithmetic, convergence checks, and output.

DD matrix low planes are stored in the tail extension:

```c
xianzhikrylov_system_data system = {0};
system.matrix.values = high_values;
system.matrix_planes.values_low = low_values;
```

The tail extension preserves the old ABI2 structure prefix.

### Vectors

`xianzhikrylov_vector` stores RHS vectors and optional initial guesses:

- `rows` is the scalar vector length.
- `rhs_count` is the number of right-hand sides.
- `RHS_MAJOR`: element `(i,j)` is at `j*leading_dimension+i`.
- `ROW_MAJOR`: element `(i,j)` is at `i*leading_dimension+j`.
- DD vectors use `data` for the high plane and `low` for the low plane.
- Buffer sizes are measured in bytes and include any padding.

`HOST` input is copied by the library. Ordinary FP64 routes may borrow a
`DEVICE` buffer owned by the caller. Other precision routes currently accept
host CSR input.

## Seven C Functions

### `xianzhikrylov_init(XIANZHIKRYLOV_BACKEND_AUTO, device_id, config_path, &handle)`

Creates a handle, lets the installed vendor package select its compiled backend,
selects the visible GPU, and reads the JSON configuration. The function returns
only the status and fills `handle` on success.

### `xianzhikrylov_set_system(handle, changes, &system)`

Passes A, b, and an optional initial x. Use
`XIANZHIKRYLOV_STRUCTURE | XIANZHIKRYLOV_RHS` for the first call.

Later calls may select:

- `XIANZHIKRYLOV_VALUES`: A changed, structure stayed the same.
- `XIANZHIKRYLOV_RHS`: b changed.
- `XIANZHIKRYLOV_STRUCTURE`: the complete system must be replaced.
- `XIANZHIKRYLOV_INITIAL_X`: provide a non-zero initial guess where supported.

### `xianzhikrylov_solve_prepare(handle, stage, &info)`

Builds structural or numerical preparation data:

- `XIANZHIKRYLOV_STRUCTURE_STAGE`
- `XIANZHIKRYLOV_NUMERIC_STAGE`
- `XIANZHIKRYLOV_AUTO_STAGE`

`info` is optional and reports epochs and build counts.

### `xianzhikrylov_solve(handle)`

Runs the route selected by `config.json` and returns the status. Every RHS must
meet its stopping condition before the call returns `CONVERGED`.

### `xianzhikrylov_print_info(handle)`

Prints the latest status, iteration count, route ID, residual, RHS norm, and
timing information.

### `xianzhikrylov_solution_download(handle, &output)`

Copies x to caller-owned host buffers. `output.scalar_type` must exactly match
the route precision. No implicit conversion is performed.

### `xianzhikrylov_destroy(&handle)`

Releases library-owned resources and clears the handle. Cleanup does not
require a valid runtime license.

## Configuration

Each sample has a `config.json`. A standard route looks like:

```json
{
  "schema_version": 1,
  "matrix_format": "csr",
  "matrix_symmetry": "general",
  "block_size": 1,
  "solver": {
    "method": "fgmres",
    "preconditioner": "ilu0",
    "max_iterations": 500,
    "restart": 30,
    "relative_tolerance": 1e-8,
    "absolute_tolerance": 0,
    "initial_guess": "zero"
  }
}
```

Common iterative methods include `cg`, `pcg`, `fcg`, `bicg`, `bicgstab`,
`gmres`, `fgmres`, and `gcr`. Jacobi, ILU(0), IC(0), DIC, DILU, HSGS/SSOR,
and block variants are available only on compatible matrix and method routes.

The convergence test is
`||r||_p <= max(atol, rtol*||b||_p)`, where `p` is the configured residual norm
and `r=b-Ax`. Selecting a norm changes stopping and reporting, not the
algorithm's required internal inner products.

## Extended Routes

Native FP32:

```json
{"extended_solver":{"method":"fgmres_fp32","preconditioner":"jacobi","restart":30,"max_iterations":500,"tolerance":1e-5}}
```

Native complex FP32 with multiple RHS:

```json
{"extended_solver":{"method":"fgmres_complex_fp32_batch","restart":15,"max_iterations":500,"tolerance":1e-5}}
```

Native Double-Double matrix solve without AMG:

```json
{"extended_solver":{"method":"fgmres_dd","provider":"none","restart":15,"max_iterations":1000,"tolerance":1e-10}}
```

The installed package determines whether additional complex FP64, projected
PCG, external AMG, or application-specific routes are available. An
unsupported combination returns `UNSUPPORTED`; the library does not silently
change method or preconditioner.

## Result Rules

- Status `1` means `CONVERGED`; status `0` means an action completed.
- `MAX_ITERATIONS`, `BREAKDOWN`, and `STAGNATED` are not successful solves.
- Review the independent residual printed by the sample.
- Do not change b or relax tolerances to make an incorrect result pass.
- DD output remains two planes; do not round it to one FP64 array unless the
  application explicitly accepts that loss of precision.
