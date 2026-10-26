# fgmres + ilu0

使用 **fgmres＋ilu0** 求解 `Ax=b`，默认零初值。

## 编译和运行

在兼容的 Linux/<GPU_VENDOR> 环境中，从本目录执行：

```bash
export XIANZHIKRYLOV_LICENSE_PATHS="/absolute/path/to/license.key"
make XK_ROOT="/absolute/path/to/XianzhiKrylov" XK_BACKEND=1
make run XK_ROOT="/absolute/path/to/XianzhiKrylov" XK_BACKEND=1
```

也可以直接运行：

```bash
export LD_LIBRARY_PATH="/absolute/path/to/XianzhiKrylov/lib:/path/to/gpu/runtime/lib${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}"
./sample data/transport_1m.csrpat solver.json
```

`XK_ROOT` 应指向包含 `include/` 和 `lib/` 的已解压 XianzhiKrylov 目录。`XK_BACKEND`
应设置为对应 GPU 软件包的公开 backend 值。`make run` 会自动设置 XianzhiKrylov 库路径；
如厂商运行时不在系统默认路径，同时设置 `GPU_RUNTIME_LIB_DIR`。使用其他输入时，通过命令行传入矩阵文件和 `solver.json`。

## 输入和数据

- `sample.c`：调用公开七函数的完整示例。
- `sample_common.h`：本样例使用的文件读取、格式转换和残差检查。
- `solver.json`：求解方法和预条件参数。
- `data/transport_1m.csrpat`：CSR 标量格式，一般系统。

`.csrpat` 只保存稀疏模式、模型编号和固定 seed，不保存 A 的值。程序读入模式后确定性生成 A，并用已知精确解构造 b：实数系统使用 x=1，复数多 RHS 使用三个不同的常数复向量。生成过程不使用时间、设备编号或随机设备。

大型测试矩阵由 `samples/data/` 共享，本目录的 `data/` 使用包内相对链接，避免复制
32 份数据。将单个样例复制到其他位置时，请同时提供矩阵路径，或另行复制所需数据。

该矩阵是一般系统，实际收敛仍取决于矩阵和预条件。

程序会在 CPU 上使用原始矩阵独立检查 `r=b−Ax`。只有接口返回成功且残差满足样例检查时，
进程才返回 0。A 和 b 以 `XIANZHIKRYLOV_HOST` 描述符传给 `xianzhikrylov_set_system`，由库同步复制到 GPU。该调用返回后，库不再依赖调用方的 A、b CPU 缓冲区。`xianzhikrylov_print_info` 报告的准备／求解时间不包含这次上传；需要统计完整端到端时间时，应用应另行计入自己的数据准备和 `set_system` 调用。
