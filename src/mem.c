#include "mem.h"

/**
 * \brief 申请空间
 * \param size 大小
 */
mem *Malloc(size_t size)
{
    mem *m = NULL;

    m = (mem *)malloc(sizeof(mem));
    if (m == NULL)
        return NULL;

    m->size = size;
    m->m = malloc(m->size);
    if (m->m == NULL)
    {
        free(m);
        return NULL;
    }

    return m;
}

/**
 * \brief 释放空间
 * \param mem 由Malloc申请的空间
 */
void Free(mem *mem)
{
    if (mem)
    {
        free(mem->m); // free(NULL) 是安全的，不需要额外检查
        free(mem);
    }
}

/**
 * \brief 重新申请空间
 * \param mem 空间
 * \param size 大小
 */
void Realloc(mem *mem, size_t size)
{
    if (mem == NULL || size == 0)
    {
        return;
    }

    mem->m = realloc(mem->m, size);
    if (mem->m)
        mem->size = size;
    else
        mem->size = 0;
}