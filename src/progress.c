#include "dl.h"

int progress_load(const char *path, Progress *p)
{
    FILE *fp = fopen(PROGRESS_FILE, "r");
    if (!fp) return -1;

    char line[512];
    memset(p, 0, sizeof(*p));
    while (fgets(line, sizeof(line), fp)) {
        if (strncmp(line, "target=", 7) == 0)
            sscanf(line + 7, "%255s", p->target);
        else if (strncmp(line, "total=", 6) == 0)
            p->total = atol(line + 6);
        else if (strncmp(line, "done=", 5) == 0)
            p->done = atol(line + 5);
    }
    fclose(fp);

    /* 校验进度是否属于当前文件 */
    if (strcmp(p->target, path) != 0)
        return -1;
    return 0;
}

int progress_save(const Progress *p)
{
    FILE *fp = fopen(PROGRESS_FILE, "w");
    if (!fp) return -1;
    fprintf(fp, "target=%s\ntotal=%ld\ndone=%ld\n",
            p->target, p->total, p->done);
    fclose(fp);
    return 0;
}

void progress_remove(void)
{
    remove(PROGRESS_FILE);
}
