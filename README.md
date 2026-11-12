# CANN Fuzzy Extractor + LFW ArcFace + AdaMTrans

这是此前合成数据版 Fuzzy Extractor 的独立真实数据扩展工程。它读取 512 维 LFW ArcFace 特征，并把 WiFaKey 的 AdaMTrans 思路与 5G-NR BG2 LDPC 结构移植到 CANN / Ascend C。它不依赖也不覆盖 Fuzzy Commitment 或 Fuzzy Vault 工程。

## Gen / Rep 流程

注册 `Gen(w)`：

1. CANN AdaMTrans 将 ArcFace 特征转换成掩码后的 1536 bit 见证 `w`。
2. 将见证分为三个 512 bit 块，每块补 8 个零。
3. 为每块生成 WiFaKey BG2、`Z=10` 的随机 LDPC 码字 `c`。
4. 公开辅助数据为 `P = w XOR c`。
5. 使用公开随机种子派生 `R = HMAC-SHA-256(seed, w)`。

重构 `Rep(w', P, seed)`：

1. CANN LDPC 核解码 `w' XOR P` 得到 `c'`。
2. CANN 核输出重构见证 `w_hat = P XOR c'`。
3. Host 派生 `R' = HMAC-SHA-256(seed, w_hat)`。

## CANN 算子

- `fuzzy_extractor_lfw_adamtrans`：LSSC 量化、二值化和 AdaMTrans 掩码。
- `fuzzy_extractor_lfw_ldpc_reproduce`：WiFaKey BG2/Z=10 归一化 Min-Sum 解码和见证重构。

程序没有 CUDA、TensorFlow 或 PyTorch 运行依赖。当前版本没有声称复现 WiFaKey 完整 Neural-MS 训练或论文指标。

## 数据与运行

默认输入：

```text
/home/dsh/下载/embeddings_arcface_r100_lfw.csv
/home/dsh/cann-dev/biometric-data/lfw_issame.csv
```

运行：

```bash
./run-tests.sh
```

指定其他位置：

```bash
./run-tests.sh /path/to/embeddings.csv /path/to/lfw_issame.csv
```

特征 CSV 必须是 `12000 × 512`，每两行一对，并与 6000 行标签文件顺序一致。150 MB 特征文件不上传到 GitHub。

## 当前测试

- 正样本：LFW pair 0–4。
- 负样本：LFW pair 300–304。
- 每对由三个 `N=520/K=100` LDPC 块处理。
- CANN 输出与独立 Host 参考实现逐 bit、状态和迭代数比较。

当前结果：

```text
[RESULT] CANN/Host pipeline agreement: 10/10
[RESULT] Biometric classification: 10/10
[PASS] Fuzzy extractor LFW CANN pipeline completed.
```

分类结果不是程序退出条件；即使某个真实样本识别错误，只要 CANN 管线正确执行且与参考实现一致，测试仍可完成。

## 主要参数

| 项目 | 值 |
|---|---:|
| ArcFace / LSSC | 512 float / 1536 bit |
| 掩码保留 | 840 bit |
| LDPC | BG2 Set 2, Z=10 |
| N / K / M | 520 / 100 / 420 |
| 解码器 | Normalized Min-Sum，最多 50 轮 |
| 派生密钥 | HMAC-SHA-256，256 bit |

## 局限

- 当前是 CANN CPU 仿真，不代表真实 NPU 性能。
- 10 对样本只验证端到端流程，不是正式准确率报告。
- 阈值使用整份输入特征统计；正式实验必须按训练折拟合、测试折固定。
- 固定可复现掩码与伪随机码字仅供测试，生产系统必须使用 CSPRNG 和经过审计的 KDF/Extractor。
- 还需要评估见证熵、helper data 泄漏、FMR/FNMR、可链接性和 side-channel。

## 参考

- [WiFaKey 论文](https://arxiv.org/abs/2407.14804)
- [WiFaKey 官方源代码](https://github.com/xingbod/WiFaKey)

详见 `THIRD_PARTY_NOTICES.md`。

