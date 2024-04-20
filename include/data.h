#ifndef _DATA_H
#define _DATA_H

#include <color.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

// 释放一个数据
#define FREE(m)         (free((m).data), (m).data = NULL, (m).size = 0)

// 数据类型
typedef unsigned char BYTE;

// 存放字符串
struct string
{
    char *str;
    struct string *next;
};

// 存放数据
struct mem
{
    BYTE   *data;
    size_t  size;
};

// 数据链表
struct list
{
    struct mem  *m;
    struct list *next;
};

/**
 * \brief   添加一个数据
 * \param   head 头指针
 * \param   mem  数据
 * \param   iscopy 是否复制数据true复制，false不复制
 * \return  返回数据指针
*/
struct list *dataAdd(struct list **head, struct mem *mem, bool iscopy);

/**
 * \brief   取出(删除)一个数据
 * \param   head 头指针
 * \return  返回数据指针
 * \warning 使用完后需要手动释放数据
*/
struct mem *dataGet(struct list **head);

/**
 * \brief 释放所有数据
 * \param head 头指针
*/
void dataFree(struct list **head);

/**
 * \brief 添加一个字符串
 * \param head  头指针
 * \param str   字符串
 * \param iscopy 是否复制数据true复制，false不复制
 * \return  返回字符串指针
*/
struct string *strAdd(struct string **head, char *str, bool iscopy);

/**
 * \brief   取出(删除)一个字符串
 * \param   head 头指针
 * \return  返回字符串指针
 * \warning 使用完后需要手动释放数据
*/
char *strGet(struct string **head);

/**
 * \brief 释放所有字符串
 * \param head 头指针
*/
void strFree(struct string **head);

/**
 * \brief   从字符串表中查找字符串
 * \param   head 头指针
 * \param   str  查找的字符串
 * \return  字符串是否存在这个字符串表中
*/
bool strCheckAndGet(struct string *head, const char *str);

/**
 * \brief 获取字符串表中字符串的数量
 * \param head 头指针
*/
size_t strCount(struct string *head);

/**
 * malloc的安全封装
*/
void *MALLOC(size_t size);

#endif // _DATA_H