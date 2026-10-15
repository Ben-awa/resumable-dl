#include "dl.h"

static volatile int g_stop = 0;   /* Ctrl-C 标志 */

static void on_sigint(int sig)
{
    (void)sig;
    g_stop = 1;
}

/*
 * write: 模拟下载，把文件按 CHUNK_SIZE 逐块写入。
 * 从 .progress 的 done 处继续，每写完一块更新进度。
 * Ctrl-C 触发 on_sigint，落盘后退出 -> 下次可续。
 */
int cmd_write(const char *path, long total)
{
    Progress p;
    if (progress_load(path, &p) < 0) {
        fprintf(stderr, "[write] 找不到进度记录，请先 create\n");
        return -1;
    }
    if (total > 0) p.total = total;

    int fd = open(path, O_WRONLY);
    if (fd < 0) {
        perror("[write] open");
        return -1;
    }

    signal(SIGINT, on_sigint);

    char buf[CHUNK_SIZE];
    memset(buf, 'A', sizeof(buf));   /* 模拟下载数据 */

    long off = p.done;
    printf("[write] 从偏移 %ld 继续，总大小 %ld\n", off, p.total);

    while (off < p.total && !g_stop) {
        long remain = p.total - off;
        long n = (remain < CHUNK_SIZE) ? remain : CHUNK_SIZE;

        if (lseek(fd, off, SEEK_SET) < 0) {
            perror("[write] lseek");
            break;
        }
        if (write(fd, buf, n) != n) {
            perror("[write] write");
            break;
        }

        off += n;
        p.done = off;
        progress_save(&p);          /* 每块落盘，保证断点可靠 */
        printf("\r[write] 已完成 %ld / %ld 字节 (%.1f%%)",
               off, p.total, 100.0 * off / p.total);
        fflush(stdout);
    }
    printf("\n");

    if (g_stop)
        printf("[write] 收到 Ctrl-C，已保存断点于 %ld，可再次运行续传\n", p.done);
    else
        printf("[write] 全部完成！\n");

    close(fd);
    return 0;
}
