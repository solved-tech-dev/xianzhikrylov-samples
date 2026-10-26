#include "sample_common.hpp"

// gmres + jacobi; JSON controls the selected algorithm.
// File reading and explicit format conversion are outside library preparation.
int main(int argc, char** argv)
{
    const auto sample_dir = sample::root(argv[0]);
    const auto matrix_path = argc > 1 ? std::filesystem::path(argv[1])
        : sample_dir / "data/transport_1m.csrpat";
    const auto json_path = argc > 2 ? std::filesystem::path(argv[2])
        : sample_dir / "solver.json";

    sample::Matrix matrix;
    if (!matrix.load(matrix_path)) return 2;
    sample::Binding binding;
    if (!binding.make(matrix, XIANZHIKRYLOV_CSR, 1)) return 2;
    sample::Solution solution(matrix);

    xianzhikrylov_handle* handle = nullptr;
    auto status = xianzhikrylov_init(XIANZHIKRYLOV_CUDA, 0, json_path.c_str(), &handle);
    if (status != XIANZHIKRYLOV_SUCCESS) {
        std::fprintf(stderr, "init status=%d\n", status); return 1;
    }

    status = xianzhikrylov_set_system(handle,
        XIANZHIKRYLOV_STRUCTURE | XIANZHIKRYLOV_RHS, &binding.system);
    if (status != XIANZHIKRYLOV_SUCCESS) {
        std::fprintf(stderr, "set_system status=%d\n", status);
        xianzhikrylov_destroy(&handle); return 1;
    }
    status = xianzhikrylov_solve_prepare(handle, XIANZHIKRYLOV_AUTO_STAGE, nullptr);
    if (status != XIANZHIKRYLOV_SUCCESS) {
        std::fprintf(stderr, "solve_prepare status=%d\n", status);
        xianzhikrylov_destroy(&handle); return 1;
    }
    status = xianzhikrylov_solve(handle);
    if (status != XIANZHIKRYLOV_CONVERGED) {
        std::fprintf(stderr, "solve status=%d (not converged)\n", status);
        xianzhikrylov_destroy(&handle); return 1;
    }
    status = xianzhikrylov_solution_download(handle, &solution.descriptor);
    if (status != XIANZHIKRYLOV_SUCCESS) {
        std::fprintf(stderr, "solution_download status=%d\n", status);
        xianzhikrylov_destroy(&handle); return 1;
    }
    const auto print_status = xianzhikrylov_print_info(handle);
    const auto destroy_status = xianzhikrylov_destroy(&handle);
    const bool residual_ok = sample::verify(matrix, solution);
    return print_status == XIANZHIKRYLOV_SUCCESS &&
           destroy_status == XIANZHIKRYLOV_SUCCESS && residual_ok ? 0 : 1;
}

