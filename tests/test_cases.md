# 测试用例

本文件列出针对本程序设计的测试用例，覆盖正常流程与边界情况。

## 用例 1：创建空洞文件并验证稀疏性

**步骤**：

```bash
./dl create test.bin 262144
ls -lh test.bin
du  -h test.bin
```

**预期**：`ls` 显示 256K，`du` 显示 0 或极小值，说明是稀疏文件。

## 用例 2：完整下载流程

**步骤**：

```bash
./dl write test.bin
./dl status test.bin
```

**预期**：write 跑完显示「全部完成！」，status 显示已完成 100%、空洞占比 0%。

## 用例 3：断点续传

**步骤**：

```bash
rm -f big.bin .progress
./dl create big.bin 104857600
./dl write big.bin          # 中途 Ctrl-C
./dl status big.bin         # 看断点
./dl write big.bin          # 续传
./dl status big.bin
```

**预期**：第二次 write 从断点处继续，而非从 0 开始；最终 100% 完成。

## 用例 4：边界——文件大小恰好 128KB

**步骤**：

```bash
./dl create min.bin 131072
./dl write min.bin
```

**预期**：正常创建、正常下载完成。128KB 是作业要求的下界，应当允许。

## 用例 5：边界——小于 128KB

**步骤**：

```bash
./dl create small.bin 65536
```

**预期**：程序拒绝，输出 `[create] 大小需 >= 128KB`，不创建文件。

## 用例 6：未 create 直接 write

**步骤**：

```bash
rm -f .progress
./dl write test.bin
```

**预期**：提示找不到进度记录，提示先 create，退出返回非 0。

## 用例 7：status 不存在的文件

**步骤**：

```bash
./dl status not_exist.bin
```

**预期**：`stat` 报错，输出类似 `stat: No such file or directory`。

## 用例 8：参数缺失

**步骤**：

```bash
./dl
./dl create
```

**预期**：打印用法说明，退出。
