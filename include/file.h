#ifndef FILE_H
#define FILE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>

#include "config.h"


/**
 * \brief 递归创建多层目录
 * \param path 要创建的目录路径
 * \param mode 目录权限
 * \return 成功返回0，失败返回-1
 */
int mkdir_p(const char *path, mode_t mode);

/**
 * \brief 创建文件，如果目录不存在则先依次创建目录
 * \param path 文件路径
 * \return 成功返回文件指针，失败返回NULL
 */
FILE *createFile(const char *path);

#endif // FILE_H