# Double-Double 解 + FGMRES + LinglongAMG

本样例接受 FP64 稀疏矩阵 A 和 FP64 右端向量 b，使用外部 LinglongAMG
周期作为 DD-FGMRES 的预条件，最终下载两个 double 平面作为 Double-Double 解：

```text
x = high + low
```

Double-Double 的有效精度约为 106～108 位。本样例不会把 DD 解隐式舍入成普通 double。

本样例需要单独的 LinglongAMG 动态库及其配置文件；只下载 XianzhiKrylov 软件包时，
编译可以通过，但运行 `solve_prepare` 前必须先填写 `solver.json` 中的两个绝对路径。

## 先配置 LinglongAMG

先安装与当前 GPU 和 XianzhiKrylov 软件包匹配的 LinglongAMG，然后编辑本目录中的
`solver.json`，把两处占位路径改成真实绝对路径：

```json
"library_path": "/实际路径/liblinglongamg.so",
"config_path": "/实际路径/linglongamg-config.json"
```

`library_path` 指向 LinglongAMG 动态库，`config_path` 指向该库使用的 AMG 配置文件。
路径不能写成 `/absolute/path/to/...` 示例占位值，也不支持 `~` 或环境变量展开。

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
cd /解压位置/XianzhiKrylov/samples/35_dd_fgmres_amg
pwd
```

`pwd` 输出的最后一级目录应当是 `35_dd_fgmres_amg`。如果提示目录不存在，先用
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
- `solver.json`：DD-FGMRES、LinglongAMG 路径和预条件参数。
- `data/diffusion_1m.csrpat`：CSR double 标量格式，对称正定系统。

`.csrpat` 只保存稀疏模式、模型编号和固定 seed，不保存 A 的值。程序读入模式后确定性生成 A，并用已知精确解构造 b：实数系统使用 x=1，复数多 RHS 使用三个不同的常数复向量。生成过程不使用时间、设备编号或随机设备。

大型测试矩阵由 `samples/data/` 共享。本目录 `data/` 中的矩阵文件使用包内相对链接，避免复制
35 份数据。将单个样例复制到其他位置时，请同时提供矩阵路径，或另行复制所需数据。

该矩阵声明为对称正定。A、b 是普通 FP64 数据；只有解使用高低两个 double 平面。

程序使用 `high + low` 在 CPU 上独立检查原系统残差。只有接口返回成功且残差满足样例
检查时，进程才返回 0。DD 输出描述符必须同时提供 `data` 和 `low` 两块内存，容量各为
`n * sizeof(double)` 字节。
