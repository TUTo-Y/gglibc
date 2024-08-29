#include "list.h"

/**
 * \brief 初始化一个新的头链表
 */
void listInitList(list *l)
{
    if (l == NULL)
        return;

    l->count = 0;
    l->fd = l->bk = l;
}

/**
 * \brief 创建一个节点
 * \return 新的节点
 */
list *listCreateNode()
{
    list *node = NULL;

    // 创建节点
    node = (list *)malloc(sizeof(list));
    if (node == NULL)
        return NULL;

    // 初始化节点
    node->data = NULL;
    node->fd = node->bk = node;

    return node;
}

/**
 * \brief 向节点中写入数据
 * \param node 节点
 * \param data 数据
 * \param size 数据大小(当copy为true时有效)
 * \param copy 是否复制数据
 * \return 节点
 */
list *listDataToNode(list *node, const void *data, size_t size, bool copy)
{
    if (node == NULL || (copy == true && data == NULL))
        return NULL;

    // 复制数据
    if (copy == true)
    {
        node->data = malloc(size);
        if (node->data == NULL)
            return NULL;
        memcpy(node->data, data, size);
    }
    // 不复制数据，只复制指针
    else
    {
        node->data = (void *)data;
    }

    return node;
}

/**
 * \brief 向链表头处添加节点
 */
void listAddNodeInStart(list *l, list *node)
{
    if (l == NULL || node == NULL)
        return;

    list *tmp = l->fd;

    l->fd = node;
    node->bk = l;

    tmp->bk = node;
    node->fd = tmp;

    ++l->count;
}

/**
 * \brief 向链表尾处添加节点
 */
void listAddNodeInEnd(list *l, list *node)
{
    if (l == NULL || node == NULL)
        return;

    list *tmp = l->bk;

    l->bk = node;
    node->fd = l;

    tmp->fd = node;
    node->bk = tmp;

    ++l->count;
}

/**
 * \brief 从链表头处获取一个节点
 * \param l 链表
 * \return 节点，若链表为空则返回NULL
 */
list *listGetNodeFromStart(list *l)
{
    if (l == NULL || l->count == 0)
        return NULL;

    list *node = l->fd;
    list *fwd = node->fd;

    // 将链表取出
    l->fd = fwd;
    fwd->bk = l;

    // 将节点的fd和bk指向自己
    node->fd = node->bk = node;

    --l->count;

    return node;
}

/**
 * \brief 从链表尾处获取一个节点
 * \param l 链表
 * \return 节点, 若链表为空则返回NULL
 */
list *listGetNodeFromEnd(list *l)
{
    if (l == NULL || l->count == 0)
        return NULL;

    list *node = l->bk;
    list *bck = node->bk;

    // 将链表取出
    l->bk = bck;
    bck->fd = l;

    // 将节点的fd和bk指向自己
    node->fd = node->bk = node;

    --l->count;

    return node;
}

/**
 * \brief 删除一个节点数据
 * \param node 节点
 * \param freeData 释放数据的函数
 * \return void
 */
void listDeleteNodeData(list *node, void (*freeData)(void *))
{
    if (node == NULL)
        return;

    // 释放数据
    if (freeData && node->data)
        freeData(node->data);

    node->data = NULL;
}

/**
 * \brief 删除一个节点
 * \param l 链表(若改节点未被取出，必须提供链表)
 * \param node 节点
 * \param freeData 释放数据的函数
 * \return void
 */
void listDeleteNode(list *l, list *node, void (*freeData)(void *))
{
    if (node == NULL || node == l)
        return;

    // 释放节点数据
    listDeleteNodeData(node, freeData);

    // 若不是单个节点，则将节点从链表中取出
    if (node->fd != node || node->bk != node)
    {
        list *fd = node->fd;
        list *bk = node->bk;

        fd->bk = bk;
        bk->fd = fd;

        // 减少count
        if (l)
            --l->count;
    }

    // 释放节点
    free(node);

    return;
}

/**
 * \brief 删除链表
 * \param l 链表
 * \param freeData 释放数据的函数
 * \return void
 */
void listDeleteList(list *l, void (*freeData)(void *))
{
    if (l == NULL)
        return;

    // 释放所有节点
    while (l->fd != l)
        listDeleteNode(l, l->fd, freeData);

    return;
}

/**
 * \brief 获取链表的节点数量
 * \param l 链表
 * \return 节点数量
 */
int listGetCount(list *l)
{
    return l->count;
}

/**
 * \brief 从头部开始依次处理节点的每一个数据
 * \param l 链表
 * \param doData 处理数据的函数
 * \param arg 传递给处理数据的参数
 * \return void
 */
void listDoFromStart(list *l, void (*doData)(list *l, list *node, void *arg), void* arg)
{
    if (l == NULL || doData == NULL)
        return;

    list *node = l->fd;
    list *fwd = node->fd;

    while (node != l)
    {
        doData(l, node, arg);
        node = fwd;
        fwd = node->fd;
    }
}

/**
 * \brief 从尾部开始依次处理节点的每一个数据
 * \param l 链表
 * \param doData 处理数据的函数
 * \param arg 传递给处理数据的参数
 * \return void
 */
void listDoFromEnd(list *l, void (*doData)(list *l, list *node, void *arg), void* arg)
{
    if (l == NULL || doData == NULL)
        return;

    list *node = l->bk;
    list *bck = node->bk;

    while (node != l)
    {
        doData(l, node, arg);
        node = bck;
        bck = node->bk;
    }
}