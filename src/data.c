#include <data.h>

struct list *dataAdd(struct list **head, struct mem *mem, bool iscopy)
{
    // head不能为空
    if(head == NULL)
        return NULL;

    // 申请节点内存
    struct list *new_node = (struct list *)MALLOC(sizeof(struct list));

    // 复制数据
    if (iscopy == true)
    {
        new_node->m = (struct mem *)MALLOC(sizeof(struct mem));
        new_node->m->data = (BYTE *)MALLOC(mem->size);
        new_node->m->size = mem->size;
        memcpy(new_node->m->data, mem->data, mem->size);
    }
    else
        new_node->m = mem;

    // 添加链表
    new_node->next = *head;
    *head = new_node;

    return new_node;
}

struct mem *dataGet(struct list **head)
{
    if (*head == NULL)
        return NULL;

    struct list *node = *head;
    struct mem *mem = node->m;

    *head = node->next;
    free(node);

    return mem;
}

void dataFree(struct list **head)
{
    // head不能为空
    if(head == NULL)
        return;

    while (*head)
    {
        struct list *node = *head;
        *head = node->next;
        FREE(*(node->m));
        free(node->m);
        free(node);
    }
}

struct string *strAdd(struct string **head, char *str, bool iscopy)
{
    // head不能为空
    if(head == NULL)
        return NULL;

    // 申请节点内存
    struct string *new_node = (struct string *)MALLOC(sizeof(struct string));

    // 复制字符串
    if (iscopy == true)
    {
        if (str)
        {
            new_node->str = strdup(str);
            if (new_node->str == NULL)
            {
                fprintf(stderr, BRED("致命错误：申请内存时，内存分配失败!\n"));
                free(new_node);
                exit(-1);
            }
        }
        else
        {
            new_node->str = MALLOC(1);
            new_node->str[0] = '\0';
        }
    }
    else
        new_node->str = str;

    // 添加链表
    new_node->next = *head;
    *head = new_node;

    return new_node;
}

char *strGet(struct string **head)
{
    if (*head == NULL)
        return NULL;

    struct string *node = *head;
    char *str = node->str;

    *head = node->next;
    free(node);

    return str;
}

void strFree(struct string **head)
{
    // head不能为空
    if(head == NULL)
        return;

    while (*head)
    {
        struct string *node = *head;
        *head = node->next;
        free(node->str);
        free(node);
    }
}

bool strCheckAndGet(struct string *head, const char *str)
{
    // head不能为空
    if(head == NULL)
        return false;
        
    while (head)
    {
        // 检测字符串
        if (!strcmp(head->str, str))
            return true;
        head = head->next;
    }

    return false;
}

size_t strCount(struct string *head)
{
    size_t count = 0;
    while (head)
    {
        count++;
        head = head->next;
    }

    return count;
}

/**
 * malloc的安全封装
 */
void *MALLOC(size_t size)
{
    void *ptr = malloc(size);
    if (ptr == NULL)
    {
        fprintf(stderr, BRED("致命错误：申请0x%lx字节内存时，内存分配失败!\n"), size);
        exit(-1);
    }
    return ptr;
}