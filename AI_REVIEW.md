# AI 代码审查

本文件记录用 AI 对代码进行审查后发现的缺陷，按「现象 / 根因 / 验证」三段式描述。

## 缺陷 1：atol 解析参数存在溢出风险

**现象**：`main.c` 中用 `atol(argv[3])` 解析 size / total 参数。如果用户传入一个超过 `long` 范围的数字（例如 99999999999999999999），`atol` 的行为是未定义的，可能返回 `LONG_MAX`，也可能返回垃圾值。

**根因**：`atol` 本身不做溢出检测，标准里明确说「如果结果无法表示，行为未定义」。应当用 `strtol` 并检查 `errno` 和 `endptr`。

**验证**：
```bash
./dl create x.bin 99999999999999999999
ls -lh x.bin
```
观察生成的文件大小是否异常，或程序是否给出错误提示。

## 缺陷 2：SIGINT 处理函数中调用了非异步信号安全函数

**现象**：`write.c` 的 `on_sigint()` 只设置了 `g_stop = 1`，本身是安全的；但主循环在 `g_stop` 被置位后，仍可能正在执行 `progress_save()`（内部调用 `fopen` / `fprintf` / `fclose`）。如果 Ctrl-C 恰好发生在这期间，就可能出现文件写到一半被打断、进度文件损坏的情况。

**根因**：`fopen` / `fprintf` 不是异步信号安全函数（async-signal-safe）。信号处理期间如果主流程正在跑这些函数，状态可能不一致。

**验证**：
```bash
./dl write big.bin
# 在打印进度时连续快速按多次 Ctrl-C
cat .progress
```
观察 `.progress` 是否出现残缺或格式错乱。

## 缺陷 3：progress_load 用 sscanf 读 target 有长度上限

**现象**：`progress.c` 中用 `sscanf(line + 7, "%255s", p->target)` 读取 target 字段，最多 255 字符。如果目标文件路径超过 255 字符（长路径场景），会被截断，导致 `strcmp(p->target, path) != 0` 判断失败，进度读不出来。

**根因**：`Progress.target` 固定为 `char[256]`，`sscanf` 的 `%255s` 与之匹配，但本身没有对超长路径的处理逻辑。

**验证**：
```bash
# 构造一个超长路径（>255 字符）
mkdir -p $(python3 -c "print('a'*260)") 2>/dev/null || true
./dl create <超长路径>/x.bin 262144
./dl status <超长路径>/x.bin
```
观察 status 是否能正确读出进度。
