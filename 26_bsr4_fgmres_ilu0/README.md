# fgmres + ilu0

使用 **fgmres＋ilu0** 求解 `Ax=b`，默认零初值。

## 运行这个样例

下面的命令针对已经解压、并且包含以下三个目录的 XianzhiKrylov 软件包：

```text
XianzhiKrylov/
├── include/
├── lib/
└── samples/
```

### 1. 进入本样例

把 `/解压位置` 换成你的实际解压目录：

```bash
cd /解压位置/XianzhiKrylov/samples/26_bsr4_fgmres_ilu0
pwd
```

`pwd` 输出的最后一级目录应当是 `26_bsr4_fgmres_ilu0`。如果提示目录不存在，先用
`cd /解压位置/XianzhiKrylov/samples` 和 `ls` 查看实际目录名。

### 2. 确认授权已经放好

第一次使用 XianzhiKrylov 时，在软件包根目录放置授权文件：

```bash
cp /你的授权文件目录/xianzhikrylov-xxxx.license.key \
  ../../license.key
chmod 600 ../../license.key
ls -l ../../license.key
```

把授权文件名替换成实际名称。如果已经按
[样例总说明](../README.md) 放好授权，可以跳过这一步。

### 3. 编译

NVIDIA CUDA 平台执行：

```bash
make clean
make XK_BACKEND=1
```

海光 DCU／ROCm 平台执行：

```bash
make clean
make XK_BACKEND=2
```

其他厂商平台请使用 [样例总说明](../README.md) 中的后端编号，把命令中的
`1` 或 `2` 换成对应数字。编译成功后会生成名为 `sample` 的文件。

### 4. 运行

NVIDIA CUDA：

```bash
make run XK_BACKEND=1
```

海光 DCU／ROCm：

```bash
make run XK_BACKEND=2
```

`make run` 会自动设置 XianzhiKrylov 和自己的 `lib/` 路径，不需要手工设置
`LD_LIBRARY_PATH`。

正常结束时应当看到类似内容：

```text
XianzhiKrylov status=1 converged=1 ... relative_residual=...
Independent residual RHS 1: relative_l2=... PASS
```

`PASS` 表示程序已经取回解，并在 CPU 上使用原矩阵重新检查了 `r=b-Ax`。

检查退出码：

```bash
echo $?
```

显示 `0` 表示成功。

## 样例目录不在软件包内时

如果当前 Samples 是另外从 Git 仓库克隆的，需要告诉 Makefile 软件包在哪里：

```bash
export XK_ROOT="/解压位置/XianzhiKrylov"
make clean
make XK_ROOT="$XK_ROOT" XK_BACKEND=1
make run XK_ROOT="$XK_ROOT" XK_BACKEND=1
```

海光平台仍把 `XK_BACKEND=1` 改成 `XK_BACKEND=2`。`XK_ROOT` 必须指向直接包含
`include/` 和 `lib/` 的那一层目录。

## 常见问题

- 找不到 `XianzhiKrylovAPI.h`：`XK_ROOT` 指向了错误目录，或者 Samples 与软件包
  分开后没有设置 `XK_ROOT`。
- 提示 `Missing library`：检查
  `/解压位置/XianzhiKrylov/lib/libXianzhiKrylov.so` 是否存在。
- 提示 `LICENSE_ERROR`、`init status=11`：检查 `../../license.key` 是否存在、
  是否过期，以及是否签发给当前 GPU。
- 提示 backend 不支持：`XK_BACKEND` 数字与当前软件包不一致。
- 运行时报缺少厂商 `.so`：先按软件包 README 找到运行时库目录，再执行
  `make run XK_BACKEND=1 GPU_RUNTIME_LIB_DIR="/厂商运行时库目录"`。
- 修改源码后结果没有变化：先执行 `make clean`，再重新编译。

更完整的入门步骤和错误处理见 [样例总说明](../README.md)。

## 输入和数据

- `sample.c`：调用公开七函数的完整示例。
- `sample_common.h`：本样例使用的文件读取、格式转换和残差检查。
- `solver.json`：求解方法和预条件参数。
- `data/coupled4_500k.csrpat`：BSR4 块格式，一般系统。

`.csrpat` 只保存稀疏模式、模型编号和固定 seed，不保存 A 的值。程序读入模式后确定性生成 A，并用已知精确解构造 b：实数系统使用 x=1，复数多 RHS 使用三个不同的常数复向量。生成过程不使用时间、设备编号或随机设备。

大型测试矩阵由 `samples/data/` 共享。本目录 `data/` 中的矩阵文件使用包内相对链接，避免复制
32 份数据。将单个样例复制到其他位置时，请同时提供矩阵路径，或另行复制所需数据。

该矩阵是一般系统，实际收敛仍取决于矩阵和预条件。

程序会在 CPU 上使用原始矩阵独立检查 `r=b−Ax`。只有接口返回成功且残差满足样例检查时，
进程才返回 0。A 和 b 以 `XIANZHIKRYLOV_HOST` 描述符传给 `xianzhikrylov_set_system`，由库同步复制到 GPU。该调用返回后，库不再依赖调用方的 A、b CPU 缓冲区。`xianzhikrylov_print_info` 报告的准备／求解时间不包含这次上传；需要统计完整端到端时间时，应用应另行计入自己的数据准备和 `set_system` 调用。
