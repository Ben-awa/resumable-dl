#include "dl.h"

/*
 * status: 打印
 *  - 逻辑大小 st_size
 *  - 实际占用块数 st_blocks（512B/块）
 *  - 空洞占比 = 1 - 实际字节/逻辑字节
 */
int cmd_status(const char *path)
{
    struct stat st;
    if (stat(path, &st) < 0) {
        perror("[status] stat");
        return -1;
    }

    long logical   = st.st_size;
    long allocated = st.st_blocks * 512;   /* st_blocks 单位是 512 字节 */

    double hole_ratio = 0.0;
    if (logical > 0)
        hole_ratio = 1.0 - (double)allocated / (double)logical;

    printf("=== status: %s ===\n", path);
    printf("逻辑大小   : %ld 字节 (%.2f KB)\n", logical, logical / 1024.0);
    printf("实际占用   : %ld 字节 (%.2f KB), %ld 块\n",
           allocated, allocated / 1024.0, (long)st.st_blocks);
    printf("空洞占比   : %.1f%%\n", hole_ratio * 100.0);

    Progress p;
    if (progress_load(path, &p) == 0)
        printf("已完成进度 : %ld / %ld (%.1f%%)\n",
               p.done, p.total, p.total ? 100.0 * p.done / p.total : 0.0);

    return 0;
}
