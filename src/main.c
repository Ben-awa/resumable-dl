#include "dl.h"

static void usage(const char *prog)
{
    printf("用法:\n");
    printf("  %s create <file> [size]   创建空洞文件(默认 256KB)\n", prog);
    printf("  %s write  <file> [total]  模拟下载写入(可 Ctrl-C 中断)\n", prog);
    printf("  %s status <file>          查看大小/块数/空洞占比/进度\n", prog);
}

int main(int argc, char **argv)
{
    if (argc < 3) { usage(argv[0]); return 1; }

    const char *cmd  = argv[1];
    const char *path = argv[2];
    long arg = (argc >= 4) ? atol(argv[3]) : 0;

    if (strcmp(cmd, "create") == 0)
        return cmd_create(path, arg > 0 ? arg : DEFAULT_SIZE);
    if (strcmp(cmd, "write") == 0)
        return cmd_write(path, arg);
    if (strcmp(cmd, "status") == 0)
        return cmd_status(path);

    usage(argv[0]);
    return 1;
}
