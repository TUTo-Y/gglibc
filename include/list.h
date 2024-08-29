/**
 * 双向链表
 */
#ifndef LIST_H
#define LIST_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct _list list;
typedef struct _list
{
    union
    {
        void *data;
        int count;
    };
    list *fd; // 向前
    list *bk; // 向后
} list;

/**
 * \brief 初始化一个新的头链表
 */
void listInitList(list *l);

/**
 * \brief 创建一个节点
 * \return 新的节点
 */
list *listCreateNode();

/**
 * \brief 向节点中写入数据
 * \param node 节点
 * \param data 数据
 * \param size 数据大小(当copy为true时有效)
 * \param copy 是否复制数据
 * \return 节点
 */
list *listDataToNode(list *node, const void *data, size_t size, bool copy);

/**
 * \brief 向链表头处添加节点
 * \param l 链表
 * \param node 节点
 */
void listAddNodeInStart(list *l, list *node);

/**
 * \brief 向链表尾处添加节点
 * \param l 链表
 * \param node 节点
 */
void listAddNodeInEnd(list *l, list *node);

/**
 * \brief 从链表头处获取一个节点
 * \param l 链表
 * \return 节点，若链表为空则返回NULL
 */
list *listGetNodeFromStart(list *l);

/**
 * \brief 从链表尾处获取一个节点
 * \param l 链表
 * \return 节点, 若链表为空则返回NULL
 */
list *listGetNodeFromEnd(list *l);

/**
 * \brief 删除一个节点数据
 * \param node 节点
 * \param freeData 释放数据的函数
 * \return void
 */
void listDeleteNodeData(list *node, void (*freeData)(void *));

/**
 * \brief 删除一个节点
 * \param l 链表(若改节点未被取出，必须提供链表)
 * \param node 节点
 * \param freeData 释放数据的函数
 * \return void
 */
void listDeleteNode(list *l, list *node, void (*freeData)(void *));

/**
 * \brief 删除链表
 * \param l 链表
 * \param freeData 释放数据的函数
 * \return void
 */
void listDeleteList(list *l, void (*freeData)(void *));

/**
 * \brief 获取链表的节点数量
 * \param l 链表
 * \return 节点数量
 */
int listGetCount(list *l);

/**
 * \brief 从头部开始依次处理节点的每一个数据
 * \param l 链表
 * \param doData 处理数据的函数
 * \param arg 传递给处理数据的参数
 * \return void
 */
void listDoFromStart(list *l, void (*doData)(list *l, list *node, void *arg), void* arg);

/**
 * \brief 从尾部开始依次处理节点的每一个数据
 * \param l 链表
 * \param doData 处理数据的函数
 * \param arg 传递给处理数据的参数
 * \return void
 */
void listDoFromEnd(list *l, void (*doData)(list *l, list *node, void *arg), void* arg);

#endif // LIST_H
