# XianzhiKrylov 26.48

## 新增特性

- **NVIDIA CUDA** 新增原生 BSR3 FGMRES＋块 ILU(0) L0，支持 FP64 和零初值。
- **海光 HIP** 新增 JSON v2 支持，可配置求解器、预条件器和适用的 Level，并按需启用原系统真实残差检查。
- **平台与兼容性** 保持现有 C API、ABI 和 JSON v1 配置兼容。
- **Samples** 新增 JSON v2 转 v1 工具，可将等价的 FP64 配置转换为 XianzhiKrylov 26.44 和 26.45 可读取的 JSON v1。

## 改进性能

- **NVIDIA CUDA** 降低大规模 CSR GMRES＋Jacobi 的显存占用并缩短求解时间，修复部分矩阵上 CSR BiCGStab 和 GMRES 搭配 ILU(0) L0 的性能回退。修复大规模块 ILU(0) 的索引溢出，以及块 FGMRES 在工作区分配失败后可能发生的异常访问。
- **海光 HIP** 提升单卡混合精度 CG、Double Double FGMRES、采用 LinglongAMG 预条件的迭代求解，以及 3×3 块 Jacobi 预条件 BiCGStab 的效率。缩短复数多右端项 FGMRES＋ILU(0) 的预条件准备时间。

---

# XianzhiKrylov 26.46 CUDA 更新说明

- 新增 JSON v2：按求解器、预条件器和 Level 选择配置，继续兼容 JSON v1。
- 常规 FP64 配置省略相对容差时使用 1e-6；显式容差保持不变。
- 优化部分 CG、FCG、BiCG、DILU、MSR 和 IC0 路线的执行开销，并整合 SDK 数据准备及ILU(0)预条件优化。性能收益取决于矩阵与执行环境。
- 修复 CG＋Jacobi 分阶段分析时可能误报数值失败的问题，保持公开 API/ABI 兼容。
- 中英文手册新增 CUDA 版本及 SM/PTX 编译目标表、分阶段数据更新说明。
- 独立 Samples 提供 JSON v2、v1 迁移配置，以及 CG＋Jacobi 的右端、系数和结构更新示例。
