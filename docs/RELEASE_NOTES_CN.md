# XianzhiKrylov 26.46 CUDA 更新说明

- 新增 JSON v2：按求解器、预条件器和 Level 选择配置，继续兼容 JSON v1。
- 常规 FP64 配置省略相对容差时使用 1e-6；显式容差保持不变。
- 优化部分 CG、FCG、BiCG、DILU、MSR 和 IC0 路线的执行开销，并整合 SDK 数据准备及精确三角求解优化。性能收益取决于矩阵与执行环境。
- 修复 CG＋Jacobi 分阶段分析时可能误报数值失败的问题，保持公开 API/ABI 兼容。
- 中英文手册新增 CUDA 版本及 SM/PTX 编译目标表、分阶段数据更新说明。
- 独立 Samples 提供 JSON v2、v1 迁移配置，以及 CG＋Jacobi 的右端、系数和结构更新示例。
