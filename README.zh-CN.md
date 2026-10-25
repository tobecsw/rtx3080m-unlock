# ROG RTX 3080M UEFI 算力解锁

在 Windows 启动前解除部分 FP16/BF16 张量计算吞吐限制。目前只在一台 **ROG Strix G533QS、5800H、RTX 3080 Laptop 16GB** 上完成真机验证。

| 项目 | 解锁前记录 | 解锁后 | 变化 |
|---|---:|---:|---:|
| FP32 | 14.84 | 14.82 TFLOPS | -0.1% |
| TF32 | 22.73 | 22.74 TFLOPS | +0.04% |
| FP16 | 45.32 | 74.65 TFLOPS | **+64.7%** |
| BF16 | 45.20 | 73.03 TFLOPS | **+61.6%** |

8192矩阵、20次中位数；前后PyTorch版本不同，属于与保存基线对比，不是严格同环境A/B。6项小规模计算正确性抽查通过，尚未验证长期稳定性、休眠唤醒和实际AI应用收益。

## 主要改动

- 适配独立AMD显示路径，不再要求NVIDIA负责UEFI显示，也不拆卸显示GOP。
- 使用本机自动生成的配置，验证PCI、BAR、PROM摘要和冷启动状态。
- 取得NVIDIA PCI独占后只写SS0，保留本次启动的SS1。
- 一次FLR后核对选择器和资源清理，全部通过才在同次启动中进入Windows。
- 始终要求按一次U；Esc或30秒未确认取消，不自动进入Windows。

这是启动时易失态修改，不刷VBIOS、不提高功耗上限，也不是永久解锁。其他机型未经验证。

## 使用

1. 使用独立FAT32 U盘，保留正常Windows Boot Manager，不覆盖Windows的EFI分区。
2. 将USB启动ZIP中的内容解压到U盘根目录。公开包不带NVIDIA固件，也不带他人的PROFILE或PROM。
3. 安装Bun后运行`Fetch-Firmware.cmd`，在本地下载、校验并生成所需固件。此步需要网络。
4. 保留`RTX-UNLOCK/DEBUG.ON`。程序会生成本机配置；不要复制别人机器的`PROFILE.BIN`。
5. 本程序未签名，测试机需关闭Secure Boot；使用一次性启动菜单选择U盘UEFI项。
6. 预检通过后按一次**U**，等待。已验证机器从确认到进入Windows约25秒；屏幕暂时不更新不等于没收到按键。
7. 进入Windows后，用`benchmarks/gemm_bench.py`测试，并核对U盘`RUNxx`日志。

若预检拒绝，保留日志，回正常Windows引导；若提示STOP、清理不确定或卡死，应先关机再恢复正常启动，不要在同次启动中反复尝试。

构建、第三方来源和许可证见[英文README](README.md)。测试见[TESTING.md](TESTING.md)，成功记录见[验证说明](docs/validation/README.md)。
