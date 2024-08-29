/**
 * 基础内存数据
 */
#ifndef TUAL_MEM_H
#define TUAL_MEM_H

#include <stdio.h>
#include <stdlib.h>

typedef struct
{
    void *m;
    size_t size;
} mem;

/**
 * \brief 申请空间
 * \param size 大小
 */
mem *Malloc(size_t size);

/**
 * \brief 释放空间
 * \param mem 由Malloc申请的空间
 */
void Free(mem *mem);

/**
 * \brief 重新申请空间
 * \param mem 空间
 * \param size 大小
 */
void Realloc(mem *mem, size_t size);

#endif // TUAL_MEM_H