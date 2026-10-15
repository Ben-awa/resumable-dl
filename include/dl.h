#ifndef DL_H
#define DL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <signal.h>

/* ---- 可调参数 ---- */
#define DEFAULT_SIZE    (256 * 1024)   /* 默认逻辑大小 256KB，>=128KB 要求 */
#define CHUNK_SIZE      (4 * 1024)     /* 每块 4KB，模拟下载块 */
#define PROGRESS_FILE   ".progress"    /* 进度记录文件 */

/* ---- 进度记录结构（写入 .progress 文本文件） ---- */
typedef struct {
    char  target[256];   /* 目标空洞文件名 */
    long  total;         /* 逻辑总大小 (bytes) */
    long  done;          /* 已完成偏移 (bytes) */
} Progress;

/* ---- 函数声明 ---- */
int  cmd_create(const char *path, long size);
int  cmd_write(const char *path, long total);
int  cmd_status(const char *path);

int  progress_load(const char *path, Progress *p);
int  progress_save(const Progress *p);
void progress_remove(void);

#endif
