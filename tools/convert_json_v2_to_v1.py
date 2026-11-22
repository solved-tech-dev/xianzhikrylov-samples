#!/usr/bin/env python3
"""Convert an FP64 v2 configuration for the CUDA 26.44 or 26.45 C SDK."""

import argparse
import json
from pathlib import Path
import sys
from tempfile import NamedTemporaryFile


BLOCK_SIZES = {"csr": (1,), "bsr": (2, 4, 5), "msr": (1, 3), "ldu": (1,)}
BASIS_METHODS = {"fgmres", "gmres", "gcr"}
CG_METHODS = {"cg", "fcg"}
ILU_LEVEL2_METHODS = BASIS_METHODS | {"bicgstab"}
IDENTITY = {"none", "identity"}
INT_MAX = 2**31 - 1
MAX_JSON_BYTES = 1024 * 1024


def fields(value, allowed, name):
    if not isinstance(value, dict):
        raise ValueError(f"{name}: expected an object")
    unknown = value.keys() - allowed
    if unknown:
        raise ValueError(f"{name}.{min(unknown)}: unknown field")
    return value.copy()


def integer(value, low, high, name):
    if type(value) is not int or not low <= value <= high:
        raise ValueError(f"{name}: expected an integer in [{low}, {high}]")
    return value


def matrix_kind(config, target, matrix_format, block_size):
    fmt = config.get("matrix_format", "auto")
    if fmt == "auto":
        fmt = matrix_format
    elif matrix_format is not None and fmt != matrix_format:
        raise ValueError("matrix_format: conflicts with --matrix-format")
    if not isinstance(fmt, str) or fmt not in BLOCK_SIZES:
        raise ValueError("matrix_format: specify csr, bsr, msr or ldu (use --matrix-format for auto)")
    block = block_size
    if "block_size" in config:
        block = integer(config["block_size"], 1, 64, "block_size")
        if block_size is not None and block != block_size:
            raise ValueError("block_size: conflicts with --block-size")
    if block is None and fmt in {"csr", "ldu"}:
        block = 1
    if block is None:
        raise ValueError(f"block_size: supply the actual {fmt.upper()} block size with --block-size")
    if block not in BLOCK_SIZES[fmt]:
        supported = ", ".join(map(str, BLOCK_SIZES[fmt]))
        raise ValueError(f"block_size: CUDA {target} supports {fmt.upper()} block sizes {supported}; received {block}")
    return fmt, block


def allowed_preconditioners(fmt, block, method, target):
    if fmt == "bsr":
        return {"ilu0", "block_jacobi", "multicolor_block_dilu", "parilu0"} if method == "fgmres" else set()
    if fmt == "msr":
        if method in {"bicgstab", "cg", "fcg", "gcr"}:
            return {"jacobi" if block == 1 else "block_jacobi"}
        return set()
    if fmt == "ldu":
        if method == "cg":
            return {"dic"}
        return {"dilu", "jacobi"} if method == "bicgstab" else set()
    common = IDENTITY | {"jacobi", "ic0"}
    if method in CG_METHODS:
        return common
    if method == "fgmres" or (target == "26.45" and method in {"bicgstab", "gcr", "gmres"}):
        return common | {"ilu0", "parilu0", "csr_multicolor_dilu"}
    return common | {"ilu0"}


def convert_ilu0(fmt, method, level, target, kernels):
    if "factor" in kernels or "sptrsv" in kernels:
        raise ValueError("kernels: v2 ILU0 Level defines the available kernel settings")
    if level is None:
        level = 2 if method in ILU_LEVEL2_METHODS else 0
    if level == 1:
        raise ValueError(f"solver.level: CUDA {target} has no standard SDK equivalent for ILU0 Level 1")
    if level > 2 or (level == 2 and method not in ILU_LEVEL2_METHODS):
        raise ValueError("solver.level: ILU0 Level 2 requires FGMRES, BiCGStab, GCR or GMRES; BiCG uses Level 0")
    if fmt == "bsr":
        kernels["factor"] = "bsr_native_ilu0"
        kernels["sptrsv"] = "bsr_level_exact" if level == 0 else "bsr_jacobi4"
        return "ilu0"
    kernels["factor"] = "csr_native_ilu0"
    if level == 2:
        if target == "26.44" and method != "fgmres":
            raise ValueError(f"solver.level: CUDA {target} supports the requested CSR ILU0 level only with FGMRES")
        return "csr_ilu0_approx4"
    # Select the matching legacy configuration for Level 0.
    if method == "bicg" or (target == "26.44" and method in {"gmres", "gcr"}):
        kernels["sptrsv"] = "csr_level_exact"
        return "ilu0"
    kernels["sptrsv"] = "csr_syncfree_exact"
    return "csr_ilu0_syncfree_exact"


def validate_kernels(kernels, fmt, pc, method, target):
    spmv = {"default"}
    if fmt == "csr":
        spmv.add("csr_scalar")
        if pc in IDENTITY | {"jacobi"} and method in CG_METHODS | {"bicg", "bicgstab"}:
            spmv.add("csr_warp")
    elif fmt == "bsr":
        spmv.add("bsr_noatomic")
    factor = {"default"}
    triangular = {"default"}
    if pc == "parilu0":
        if fmt == "csr":
            factor.add("csr_parilu0_sync")
            if method == "fgmres":
                factor.add("csr_parilu0_async")
                triangular.add("csr_value_ready_exact")
            triangular.add("csr_syncfree_exact")
        else:
            factor.add("bsr_parilu0")
            triangular.update({"bsr_level_exact", "bsr_jacobi4"})
    elif fmt == "ldu" and pc in {"dilu", "dic"}:
        triangular.add("ldu_level_graph")
        if pc == "dilu":
            triangular.update({"ldu_jacobi4", "ldu_jacobi8"})
    for name, choices in (("spmv", spmv), ("factor", factor), ("sptrsv", triangular)):
        value = kernels.get(name, "default")
        if not isinstance(value, str) or value not in choices:
            raise ValueError(f"kernels.{name}: {value!r} has no supported CUDA {target} mapping for this route")


def convert_config(value, target, matrix_format, block_size):
    config = fields(value, {"matrix_format", "block_size", "matrix_symmetry", "backend",
                            "solver", "kernels", "parameters", "performance", "fusion",
                            "method_options"}, "config")
    if "method_options" in config:
        raise ValueError(f"method_options: CUDA {target} does not accept these options in the standard C SDK")
    fmt, block = matrix_kind(config, target, matrix_format, block_size)
    config.update(matrix_format=fmt, block_size=block)
    if config.get("backend", "any") not in ("any", "cuda"):
        raise ValueError("backend: this converter targets the CUDA SDK")
    solver = fields(config.get("solver"), {"method", "preconditioner", "level", "initial_guess",
                                          "max_iterations", "restart", "relative_tolerance",
                                          "absolute_tolerance", "verify_true_residual"}, "solver")
    method = solver.get("method")
    if not isinstance(method, str) or method not in ILU_LEVEL2_METHODS | CG_METHODS | {"pcg", "bicg"}:
        raise ValueError("solver.method: expected a supported Krylov method")
    if method == "pcg":
        method = "cg"
    pc = solver.get("preconditioner")
    if not isinstance(pc, str) or pc not in allowed_preconditioners(fmt, block, method, target):
        raise ValueError(f"solver.preconditioner: {fmt.upper()}{block} {method} + {pc!r} is outside the CUDA {target} C SDK")
    if solver["method"] == "pcg" and pc in IDENTITY:
        solver["method"] = "cg"
    needs_spd = method in CG_METHODS or pc == "ic0"
    symmetry = config.get("matrix_symmetry", "spd" if needs_spd else "general")
    if symmetry not in ("general", "spd"):
        raise ValueError("matrix_symmetry: use general or spd")
    if needs_spd and symmetry == "general":
        raise ValueError("matrix_symmetry: CG, FCG and IC0 require an SPD matrix")
    maximum = integer(solver.get("max_iterations", 1000), 1, INT_MAX, "solver.max_iterations")
    if method in BASIS_METHODS:
        restart = integer(solver.get("restart", 30), 1, 512, "solver.restart")
        legacy_basis_limit = fmt in {"bsr", "msr"} or (
            (method == "fgmres" and pc != "ic0") or (method == "gmres" and pc in IDENTITY)
        )
        if legacy_basis_limit and restart > maximum:
            raise ValueError(f"solver.restart: this CUDA {target} route requires restart <= max_iterations")
    elif "restart" in solver:
        raise ValueError("solver.restart: this method does not use restart")
    solver.setdefault("relative_tolerance", 1e-6)
    for name in ("relative_tolerance", "absolute_tolerance"):
        tolerance = solver.get(name, 0)
        if type(tolerance) not in (int, float) or not 0 <= tolerance <= sys.float_info.max:
            raise ValueError(f"solver.{name}: expected a finite nonnegative number")
    initial = solver.get("initial_guess", "provided")
    if initial not in ("zero", "provided", "reuse"):
        raise ValueError("solver.initial_guess: use zero, provided or reuse")
    verify = solver.get("verify_true_residual", False)
    if type(verify) is not bool:
        raise ValueError("solver.verify_true_residual: expected a boolean")
    if target == "26.44":
        if verify:
            raise ValueError("solver.verify_true_residual: CUDA 26.44 has no equivalent JSON option")
        solver.pop("verify_true_residual", None)

    level = solver.pop("level", None)
    if "level" in value["solver"]:
        integer(level, 0, 3, "solver.level")
    kernels = fields(config.get("kernels", {}), {"spmv", "sptrsv", "factor"}, "kernels")
    validate_kernels(kernels, fmt, pc, method, target)
    if pc == "ilu0":
        if "parameters" in config:
            raise ValueError("parameters: v2 ILU0 Level fixes the algorithm parameters")
        solver["preconditioner"] = convert_ilu0(fmt, method, level, target, kernels)
    elif level not in (None, 0):
        raise ValueError(f"solver.level: {pc} supports only Level 0")
    if pc == "parilu0":
        solver["preconditioner"] = "csr_parilu_syncfree_exact" if fmt == "csr" else "bsr_block_parilu"
    if "parameters" in config:
        params = fields(config["parameters"], {"factor_sweeps"}, "parameters")
        if "factor_sweeps" in params:
            if pc != "parilu0":
                raise ValueError("parameters.factor_sweeps: requires parilu0")
            integer(params["factor_sweeps"], 1, 64, "parameters.factor_sweeps")
    if fmt == "bsr" and initial != "zero" and not (
            block == 4 and pc == "ilu0" and kernels["sptrsv"] == "bsr_jacobi4"):
        raise ValueError(f"solver.initial_guess: CUDA {target} supports provided BSR x0 only for BSR4 ILU0 Level 2")

    if "performance" in config:
        profile = fields(config.pop("performance"), {"profile"}, "performance").get("profile")
        if profile != "auto" or "fusion" in config:
            raise ValueError("performance.profile: this converter supports auto without conflicting options")
    if "fusion" in config:
        fusion = fields(config["fusion"], {"policy", "max_scope", "variant"}, "fusion")
        if fusion.get("policy", "auto") not in ("auto", "legacy") or fusion.get("max_scope", "default") != "default":
            raise ValueError("fusion: this converter supports the default scope and auto or legacy policy")
        integer(fusion.get("variant", 0), 0, 0, "fusion.variant")
    if kernels:
        config["kernels"] = kernels
    config["solver"] = solver
    return config


def convert(root, target, matrix_format=None, block_size=None):
    if not isinstance(root, dict) or type(root.get("schema_version")) is not int or root["schema_version"] != 2:
        raise ValueError("schema_version: expected 2")
    if "profiles" not in root:
        config = root.copy()
        del config["schema_version"]
        return {"schema_version": 1, **convert_config(config, target, matrix_format, block_size)}
    fields(root, {"schema_version", "profiles"}, "root")
    profiles = root["profiles"]
    if not isinstance(profiles, dict) or not profiles:
        raise ValueError("profiles: expected a nonempty object")
    output = {}
    for name, config in profiles.items():
        if not name or "\x00" in name:
            raise ValueError("profiles: expected a nonempty profile name without NUL")
        try:
            output[name] = convert_config(config, target, matrix_format, block_size)
        except ValueError as exc:
            raise ValueError(f"profiles.{name}.{exc}") from exc
    return {"schema_version": 1, "profiles": output}


def unique_object(pairs):
    result = {}
    for name, value in pairs:
        if name in result:
            raise ValueError(f"duplicate JSON field {name!r}")
        result[name] = value
    return result


def reject_constant(token):
    raise ValueError(f"invalid JSON number {token}")


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("input", type=Path, help="configuration file with schema_version 2")
    parser.add_argument("--target", required=True, choices=("26.44", "26.45"), help="installed CUDA SDK version")
    parser.add_argument("--matrix-format", choices=tuple(BLOCK_SIZES), help="actual format if omitted or auto")
    parser.add_argument("--block-size", type=int, help="actual block size if omitted")
    parser.add_argument("-o", "--output", type=Path, help="write to a separate file instead of stdout")
    args = parser.parse_args()
    try:
        with args.input.open("rb") as input_file:
            source = input_file.read(MAX_JSON_BYTES + 1)
        if len(source) > MAX_JSON_BYTES:
            raise ValueError("configuration exceeds 1 MiB")
        root = json.loads(source.decode("utf-8"), object_pairs_hook=unique_object, parse_constant=reject_constant)
        converted = convert(root, args.target, args.matrix_format, args.block_size)
        result = json.dumps(converted, indent=2, ensure_ascii=False, allow_nan=False) + "\n"
        if len(result.encode("utf-8")) > MAX_JSON_BYTES:
            raise ValueError("converted configuration exceeds 1 MiB")
        if args.output is None:
            sys.stdout.write(result)
        else:
            if args.output.exists() and args.output.samefile(args.input):
                raise ValueError("--output: choose a path distinct from the input file")
            with NamedTemporaryFile(mode="w", encoding="utf-8", dir=args.output.parent, delete=False) as stream:
                temp_path = Path(stream.name)
                try:
                    stream.write(result)
                    stream.close()
                    temp_path.replace(args.output)
                finally:
                    if temp_path.exists():
                        temp_path.unlink()
    except (ValueError, OSError, RecursionError) as exc:
        parser.error(str(exc))


if __name__ == "__main__":
    main()
