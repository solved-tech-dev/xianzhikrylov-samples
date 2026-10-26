# XianzhiKrylov Samples

本仓库包含 35 个相互独立的 C 样例，用来演示 XianzhiKrylov C 接口的完整调用过程：
读取矩阵 A 和右端向量 b、上传数据、准备求解器、求解、取回解，并在 CPU 上检查残差。

每个目录中的文件含义相同：

- `sample.c`：可以编译运行的程序；
- `sample_common.h`：示例程序使用的矩阵读取、格式转换和残差检查代码；
- `solver.json`：本样例使用的迭代法和预条件器；
- `Makefile`：用一条命令完成编译；
- `README.md`：本样例的简明运行说明；
- `data/`：其中放置指向共享样例矩阵的相对链接。

库软件包和授权文件请从
[https://store.solved-tech.com/](https://store.solved-tech.com/) 下载。

## 不知道选哪个样例时

先运行 `17_fgmres_ilu0`。它使用一般非对称 CSR 矩阵和常用的
FGMRES + ILU(0) 组合，适合第一次确认软件包、授权、GPU 运行环境和编译环境是否正常。

常见选择如下：

- 对称正定系统：`01_cg`、`07_pcg_jacobi` 或 `19_pcg_ic0`；
- 一般非对称系统：`17_fgmres_ilu0`；
- BSR 块矩阵：`26_bsr4_fgmres_ilu0`；
- 复数多右端项：`32_complex_batch3`。

如果主要想了解数据格式如何接入：

- float 应用数据：`33_float_fgmres_ilu0`；
- float complex 应用数据：`34_float_complex_batch3`；
- double complex：`32_complex_batch3`；
- Double-Double 解：`35_dd_fgmres_amg`。

| 样例 | 求解路线（迭代法 / 预条件器） | 矩阵格式 | 数据格式 |
| --- | --- | --- | --- |
| [01_cg](01_cg/README.md) | cg / none | csr | double |
| [02_bicg](02_bicg/README.md) | bicg / none | csr | double |
| [03_bicgstab](03_bicgstab/README.md) | bicgstab / none | csr | double |
| [04_gmres](04_gmres/README.md) | gmres / none | csr | double |
| [05_fgmres](05_fgmres/README.md) | fgmres / none | csr | double |
| [06_gcr](06_gcr/README.md) | gcr / none | csr | double |
| [07_pcg_jacobi](07_pcg_jacobi/README.md) | pcg / jacobi | csr | double |
| [08_fcg_jacobi](08_fcg_jacobi/README.md) | fcg / jacobi | csr | double |
| [09_bicg_jacobi](09_bicg_jacobi/README.md) | bicg / jacobi | csr | double |
| [10_bicgstab_jacobi](10_bicgstab_jacobi/README.md) | bicgstab / jacobi | csr | double |
| [11_gmres_jacobi](11_gmres_jacobi/README.md) | gmres / jacobi | csr | double |
| [12_fgmres_jacobi](12_fgmres_jacobi/README.md) | fgmres / jacobi | csr | double |
| [13_gcr_jacobi](13_gcr_jacobi/README.md) | gcr / jacobi | csr | double |
| [14_bicg_ilu0](14_bicg_ilu0/README.md) | bicg / ilu0 | csr | double |
| [15_bicgstab_ilu0](15_bicgstab_ilu0/README.md) | bicgstab / ilu0 | csr | double |
| [16_gmres_ilu0](16_gmres_ilu0/README.md) | gmres / ilu0 | csr | double |
| [17_fgmres_ilu0](17_fgmres_ilu0/README.md) | fgmres / ilu0 | csr | double |
| [18_gcr_ilu0](18_gcr_ilu0/README.md) | gcr / ilu0 | csr | double |
| [19_pcg_ic0](19_pcg_ic0/README.md) | pcg / ic0 | csr | double |
| [20_fcg_ic0](20_fcg_ic0/README.md) | fcg / ic0 | csr | double |
| [21_gmres_ic0](21_gmres_ic0/README.md) | gmres / ic0 | csr | double |
| [22_fgmres_ic0](22_fgmres_ic0/README.md) | fgmres / ic0 | csr | double |
| [23_bicgstab_ic0](23_bicgstab_ic0/README.md) | bicgstab / ic0 | csr | double |
| [24_bicg_ic0](24_bicg_ic0/README.md) | bicg / ic0 | csr | double |
| [25_gcr_ic0](25_gcr_ic0/README.md) | gcr / ic0 | csr | double |
| [26_bsr4_fgmres_ilu0](26_bsr4_fgmres_ilu0/README.md) | fgmres / ilu0 | bsr | double |
| [27_bsr4_fgmres_jacobi](27_bsr4_fgmres_jacobi/README.md) | fgmres / block_jacobi | bsr | double |
| [28_bsr4_fgmres_dilu](28_bsr4_fgmres_dilu/README.md) | fgmres / multicolor_block_dilu | bsr | double |
| [29_msr_bicgstab_jacobi](29_msr_bicgstab_jacobi/README.md) | bicgstab / jacobi | msr | double |
| [30_ldu_pcg_dic](30_ldu_pcg_dic/README.md) | pcg / dic | ldu | double |
| [31_ldu_bicgstab_dilu](31_ldu_bicgstab_dilu/README.md) | bicgstab / dilu | ldu | double |
| [32_complex_batch3](32_complex_batch3/README.md) | fgmres_complex_batch / none | csr | double complex |
| [33_float_fgmres_ilu0](33_float_fgmres_ilu0/README.md) | fgmres / ilu0 | csr | float（应用侧） |
| [34_float_complex_batch3](34_float_complex_batch3/README.md) | fgmres_complex_batch / none | csr | float complex（应用侧） |
| [35_dd_fgmres_amg](35_dd_fgmres_amg/README.md) | DD-FGMRES / LinglongAMG | csr | double 输入，Double-Double 解 |

“数据格式”指调用方在应用侧保存 A、b 和 x 时使用的类型：

- `float` 和 `float complex` 样例会显式填充 FP64 描述符。当前公开接口的矩阵系数
  按 FP64 接收，因此这两个样例演示数据存储与接口衔接，不声称底层使用 FP32 算术。
- `double complex` 使用 `xianzhikrylov_complex`，其中实部和虚部均为 `double`。
- Double-Double 样例的矩阵系数和 b 为 FP64，解由两个 `double` 平面表示，
  有效精度约 106～108 位；它不是 DD 系数矩阵。

## 运行前需要什么

一台已经安装好 GPU 驱动和对应厂商工具包的 Linux 机器，以及：

- 一个与当前 GPU 和系统匹配的 XianzhiKrylov 软件包；
- 一个 C 编译器；
- GNU Make；
- 一个签发给当前 GPU、当前软件包并在有效期内的授权文件。

软件包已经包含 `include/`、`lib/`、`samples/` 时，不需要另外安装
CMake，也不需要修改源码。

可以先检查用到的两个命令是否存在：

```bash
cc --version
make --version
```

如果系统提示 `command not found`，Ubuntu/Debian 可安装基础编译工具：

```bash
sudo apt update
sudo apt install build-essential
```

其他 Linux 发行版请使用各自的软件包管理命令安装 C 编译器、链接器和 GNU Make。

## 第一次运行：照着做

下面的例子使用 `17_fgmres_ilu0`，并假设 XianzhiKrylov 软件包已经下载到
`~/Downloads`。

### 第 1 步：解压并进入软件包

```bash
cd ~/Downloads
tar -xzf xianzhikrylov-*.tar.gz
cd XianzhiKrylov
```

不同软件包解压后的目录名可能略有不同。如果 `cd XianzhiKrylov` 失败，先执行
`ls` 查看解压出来的目录，再进入其中包含 `include/`、`lib/` 和 `samples/`
的那一层。

确认目录没有解压错：

```bash
pwd
ls
test -f include/XianzhiKrylovAPI.h && echo "include OK"
test -f lib/libXianzhiKrylov.so && echo "lib OK"
test -d samples && echo "samples OK"
```

三条检查都显示 `OK` 后继续。
只要有一条没有显示 `OK`，就说明软件包没有完整解压，不要继续执行后面的命令。

### 第 2 步：放好授权文件

只有一台机器时，把收到的 `.license.key` 文件复制到软件包根目录并命名为
`license.key`：

```bash
cp /你的授权文件目录/xianzhikrylov-xxxx.license.key ./license.key
chmod 600 ./license.key
ls -l ./license.key
```

把命令中的 `/你的授权文件目录/xianzhikrylov-xxxx.license.key` 换成实际文件名。

有多份设备授权时，也可以放在软件包根目录的 `licenses/` 下：

```bash
mkdir -p licenses
cp /你的授权文件目录/*.license.key licenses/
ls -l licenses/
```

如果下载到的是 `.licenses.zip` 压缩包，也可以直接解压到该目录：

```bash
unzip /你的授权文件目录/xianzhikrylov-xxxx.licenses.zip -d licenses
find licenses -type f -name "*.key" -print
```

`find` 至少应列出授权文件。授权 `.key` 文件需要直接放在 `licenses/` 目录中；
如果压缩包多套了一层目录，请把其中的 `.key` 文件移动到 `licenses/` 下。

授权文件必须与当前机器、GPU 和软件包匹配。不要修改授权文件内容。

如果你在集群上运行，要先申请 GPU 并进入计算节点。不要在没有 GPU 的登录节点上运行
求解程序。

### 第 3 步：确认 GPU 后端编号

样例编译时需要传入 `XK_BACKEND`。常见编号为：

| GPU 后端 | 通常使用的 `XK_BACKEND` |
| --- | --- |
| NVIDIA CUDA | `1` |
| 海光 DCU / ROCm / HIP | `2` |
| Moore Threads MUSA | `3` |
| SYCL | `4` |
| OpenCL | `5` |
| HCR | `6` |

以所下载软件包 README 标明的值为准。只要软件包没有使用特殊后端编号，下文的
NVIDIA 示例使用 `1`，海光示例使用 `2`。

### 第 4 步：编译

先固定软件包根目录，再进入推荐样例：

```bash
export XK_ROOT="$PWD"
cd samples/17_fgmres_ilu0
make clean
make XK_BACKEND=1
```

海光平台将最后一行改为：

```bash
make XK_BACKEND=2
```

其他平台把 `1` 或 `2` 换成软件包 README 中标明的编号。

编译成功时不会出现 `error`，并且本目录中会生成一个名为 `sample` 的可执行文件。

### 第 5 步：运行

NVIDIA：

```bash
make run XK_BACKEND=1
```

海光：

```bash
make run XK_BACKEND=2
```

正常运行时，终端中能看到类似以下内容：

```text
XianzhiKrylov status=1 converged=1 ... relative_residual=...
Independent residual RHS 1: relative_l2=... PASS
```

最后一行包含 `PASS` 表示本程序在 CPU 上重新计算原系统的 `r=b-Ax` 并通过检查。
`status=1 converged=1` 表示求解器达到 JSON 中配置的收敛条件。

检查程序退出码：

```bash
echo $?
```

显示 `0` 表示成功，其他值表示失败。

## 运行其他样例

每个样例目录都可以独立编译和运行。例如运行纯 CG：

```bash
cd ..
cd 01_cg
make clean
make XK_BACKEND=1
make run XK_BACKEND=1
```

海光平台仍然把 `XK_BACKEND` 改成 `2`。其他样例也按相同方式进入目录后编译。

如果要一次编译全部 35 个样例：

```bash
cd samples
make XK_BACKEND=1
```

命令没有报错且正常结束时，表示全部样例已经编译完成；出现错误时，先查看最后一个
`Error` 或 `error` 之前的目录名，再进入该目录单独排查。

## 编译命令中的参数是什么意思

- `make`：按照当前目录的 `Makefile` 编译程序，生成 `sample`。
- `make run`：如果尚未编译，会先编译，然后运行；它会自动设置
  XianzhiKrylov 动态库的查找路径。
- `make clean`：删除之前生成的 `sample`，便于彻底重新编译。
- `XK_BACKEND=1`：告诉样例当前软件包使用哪一种 GPU 后端。
- `XK_ROOT=/绝对路径/XianzhiKrylov`：只在 Samples 不在软件包内时使用，指向包含
  `include/`、`lib/` 的软件包根目录。
- `GPU_RUNTIME_LIB_DIR=/绝对路径`：厂商运行时动态库不在系统默认搜索路径时使用。

如果要从另外下载或克隆的 Samples 目录运行，先设置软件包位置：

```bash
export XK_ROOT="/绝对路径/XianzhiKrylov"
cd "/绝对路径/xianzhikrylov-samples/17_fgmres_ilu0"
make clean
make XK_ROOT="$XK_ROOT" XK_BACKEND=1
make run XK_ROOT="$XK_ROOT" XK_BACKEND=1
```

Samples 在软件包的 `samples/` 中时不需要设置 `XK_ROOT`，因为 Makefile 会自动使用
`../..`。

## 数据是怎样进入 GPU 的

样例先在普通 CPU 内存中生成矩阵 A 和向量 b，并把它们标记为
`XIANZHIKRYLOV_HOST`。程序调用 `xianzhikrylov_set_system` 时：

1. 库分配自己的 GPU 缓冲区；
2. 库把 A、b 同步复制到 GPU；
3. `solve_prepare` 和 `solve` 使用这些 GPU 数据；
4. `solution_download` 把解复制回 CPU。

因此应用代码不需要直接调用厂商复制函数。`set_system` 成功返回后，库不再依赖
调用方原来的 A、b CPU 缓冲区。

如果应用本来就把 A 或 b 放在 GPU 显存中，也可以改用
`XIANZHIKRYLOV_DEVICE` 描述符。Samples 默认演示更容易使用的 `HOST` 方式。

## 样例矩阵有什么特点

四个测试系统共用放在 `samples/data/` 下的模式文件。各样例 `data/` 目录中的矩阵文件
是指向共享数据的相对链接，避免重复保存几十兆的数据。四个模式文件合计约 90 MiB。

`.csrpat` 只保存稀疏模式、矩阵阶数、行列索引、模型类型和固定种子，不保存 A 的数值，
也不保存 b。程序运行时用固定算法生成 A，并通过已知精确解构造 `b=A*x_exact`：

- 实数系统使用全 1 精确解；
- 复数三右端项样例使用三个不同的常数复向量。

每个样例最后都会在原系统上独立检查残差，不需要另外准备 b。

`xianzhikrylov_print_info` 报告的库内准备和求解时间不包含 `set_system` 的上传；
需要统计完整端到端时间时，应用应另行计入数据准备和 `set_system` 调用。

`.csrpat` 的完整二进制格式见 [`PATTERN_FORMAT.md`](PATTERN_FORMAT.md)。

## 遇到问题先检查什么

### `XianzhiKrylovAPI.h: No such file or directory`

当前目录不是正确的软件包。先执行 `ls`，确认能看到 `include/` 和 `lib/`。
如果 Samples 不在软件包内，给 `make` 和 `make run` 都加上：

```bash
XK_ROOT="/绝对路径/XianzhiKrylov"
```

### `Missing library: lib/libXianzhiKrylov.so`

软件包不完整，或者 `XK_ROOT` 指向了错误目录。检查：

```bash
ls -l "$XK_ROOT/lib/libXianzhiKrylov.so"
```

### 运行时报 `cannot open shared object file`

通常是厂商运行时库没有在动态库搜索路径中。先按软件包 README 找到运行时库目录，
再执行，例如：

```bash
export GPU_RUNTIME_LIB_DIR="/厂商运行时库目录"
make run XK_BACKEND=1 GPU_RUNTIME_LIB_DIR="$GPU_RUNTIME_LIB_DIR"
```

不要从其他厂商的软件包复制 `.so` 文件来替换当前文件。

### `init status=11` 或 `LICENSE_ERROR`

表示授权检查失败。依次检查：

- 软件包根目录是否存在 `license.key` 或 `licenses/*.key`；
- 授权是否过期；
- 授权是否签发给当前机器和 GPU；
- 使用的软件包厂商和版本是否正确；
- 系统时间、用户目录和授权文件是否被修改。

不要通过修改 JSON 或替换动态库绕过授权错误。

### `init status=5` 或提示 backend 不支持

`XK_BACKEND` 编号与当前软件包不一致。查看当前软件包 README，把编号改为该后端
对应的值。

### `data/... No such file or directory`

样例目录 `data/` 中的矩阵文件链接没有随仓库或压缩包正确保留。确认从 `samples/`
下的共享 `data/` 目录运行，或者手动传入真实矩阵文件的绝对路径：

```bash
./sample /绝对路径/矩阵文件.csrpat solver.json
```

从普通 ZIP 文件解压时可能丢失符号链接，推荐使用 Git 仓库或原始 `.tar.gz`。

### `solve status=2`

求解达到最大迭代次数，但没有失败到无法继续。先运行 `17_fgmres_ilu0` 确认环境；
如果换成自己的矩阵，需要根据矩阵性质调整 `solver.json` 中的求解方法、预条件器、
重启长度或最大迭代次数。

### 修改样例后没有生效

先清理再重新编译：

```bash
make clean
make XK_BACKEND=1
```
