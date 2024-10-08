/**
 * 获取libc
 */
#ifndef GET_H
#define GET_H

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <archive.h>

#include "gz.h"
#include "web.h"
#include "mem.h"
#include "file.h"
#include "user.h"
#include "list.h"
#include "config.h"

/**
 * \brief 获取libc
 * \param conf 配置
 * \param cwd 当前目录
 * \param libc libc文件名
 */
bool getlibc(const config *conf, const char *cwd , const char *libc);

#endif // GET_H