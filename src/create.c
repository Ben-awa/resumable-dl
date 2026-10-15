#include "dl.h"

/*
 * create: 生成空洞文件
 * 用 open(O_CREAT|O_WRONLY) + ftruncate() 造出逻辑大小，
 * 不写任何数据 -> 磁盘上不占块（稀疏文件）。
 */
int cmd_create(const char *path, long size)
{
    if (size < 128 * 1024) {
        fprintf(stderr, "[create] 大小需 >= 128KB\n");
        return -1;
    }

    int fd = open(path, O_CREAT | O_WRONLY | O_TRUNC, 0644);
    if (fd < 0) {
        perror("[create] open");
        return -1;
    }

    if (ftruncate(fd, size) < 0) {
        perror("[create] ftruncate");
        close(fd);
        return -1;
    }
    close(fd);

    /* 初始化进度文件：done = 0 */
    Progress p;
    memset(&p, 0, sizeof(p));
    snprintf(p.target, sizeof(p.target), "%s", path);
    p.total = size;
    p.done  = 0;
    progress_save(&p);

    printf("[create] 已创建空洞文件 %s，逻辑大小 %ld 字节\n", path, size);
    return 0;
}
