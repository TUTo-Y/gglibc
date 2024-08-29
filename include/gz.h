/**
 * 解压
 */
#ifndef GZ_H
#define GZ_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include <zlib.h>
#include <archive_entry.h>

#include "mem.h"
#include "config.h"

#define MEM_ADD_SIZE 4096

/**
 * \brief 解压缩一个gz文件
 * \param in   输入数据
 * \param out  输出数据
 * \return 成功返回true，失败返回false
*/
bool gz(const mem *in, mem **out);

#endif
