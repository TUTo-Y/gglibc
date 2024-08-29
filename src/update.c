#include "update.h"

bool parse(const char *str, list *libc, list *libc_dbg)
{
    if (str == NULL || libc == NULL || libc_dbg == NULL)
        return false;

    // 读取文件
    char *head = strchr(strstr(strstr((char *)str, "pool/main/g/glibc:"), "total"), '\n') + 1;
    char *end = strstr(head, "\n\n");

    if (head == NULL)
    {
        ERR("无法找到 pool/main/g/glibc 目录\n");
        return false;
    }

    // 初始化libc和libc_sym
    listDeleteList(libc, free);
    listDeleteList(libc_dbg, free);

    // 拷贝pool/main/g/glibc目录
    char *text = (char *)malloc(end - head + 1);
    memcpy(text, head, end - head);
    text[end - head] = '\0';

    // 每一行进行分析
    char *line = strtok(text, "\n");
    while (line)
    {
        // 解析数据
        char *tmp = NULL;
        // dbg文件
        if (tmp = strstr(line, "libc6-dbg"))
        {
            // 检测是否是AMD64或者i386
            if (strstr(tmp, "amd64") || strstr(tmp, "i386"))
                listAddNodeInEnd(libc_dbg, listDataToNode(listCreateNode(), tmp, strlen(tmp) + 1, true));
        }
        // libc文件
        else if (tmp = strstr(line, "libc6_"))
        {
            // 检测是否是AMD64或者i386
            if (strstr(tmp, "amd64") || strstr(tmp, "i386"))
                listAddNodeInEnd(libc, listDataToNode(listCreateNode(), tmp, strlen(tmp) + 1, true));
        }

        line = strtok(NULL, "\n");
    }

    // free
    free(text);

    return true;
}

bool list_from_url(const char *url, list *libc, list *libc_dbg)
{
    char ls_url[1024] = {0};

    if (url == NULL || libc == NULL || libc_dbg == NULL)
        return false;

    // 获取ls-lR.gz的URL
    if (url[strlen(url) - 1] == '/')
        snprintf(ls_url, sizeof(ls_url), "%sls-lR.gz", url);
    else
        snprintf(ls_url, sizeof(ls_url), "%s/ls-lR.gz", url);

    // 下载ls-lR.gz
    mem *m = NULL;
    if (webGet(ls_url, &m) == false)
    {
        ERR("下载ls-lR.gz失败\n");
        return false;
    }

    // 解压ls-lR.gz
    mem *out = NULL;
    if (gz(m, &out) == false)
    {
        ERR("解压ls-lR.gz失败\n");
        Free(m);
        return false;
    }

    // 解析ls-lR
    if (parse(out->m, libc, libc_dbg) == false)
    {
        ERR("解析ls-lR失败\n");
        Free(m);
        Free(out);
        return false;
    }

    Free(m);
    Free(out);

    return true;
}

bool list_from_file(const char *file, list *libc, list *libc_dbg)
{
    if (file == NULL || libc == NULL || libc_dbg == NULL)
        return false;

    // 检查文件是否能打开
    FILE *fp = fopen(file, "r");
    if (fp == NULL)
    {
        ERR("无法打开文件 %s\n", file);
        return false;
    }

    // 初始化libc和libc_sym
    listDeleteList(libc, free);
    listDeleteList(libc_dbg, free);

    // 读取配置
    while (!feof(fp))
    {
        char line[1024] = {0};
        char key[1024] = {0};
        char value[1024] = {0};

        fgets(line, sizeof(line), fp);
        sscanf(line, "[%[^]]] %s\n", key, value);

        if (strcmp(key, "libc") == 0)
        {
            listAddNodeInEnd(libc, listDataToNode(listCreateNode(), value, strlen(value) + 1, true));
        }
        else if (strcmp(key, "libc_dbg") == 0)
        {
            listAddNodeInEnd(libc_dbg, listDataToNode(listCreateNode(), value, strlen(value) + 1, true));
        }
    }
    fclose(fp);
    return true;
}

bool list_to_file(const char *file, list *libc, list *libc_dbg)
{
    if (file == NULL || libc == NULL || libc_dbg == NULL)
        return false;

    // 检查文件是否能打开
    FILE *fp = fopen(file, "w");
    if (fp == NULL)
    {
        ERR("无法打开文件 %s\n", file);
        return false;
    }

    // 写入libc
    list *node = libc->fd;
    while (node != libc)
    {
        fprintf(fp, "[libc] %s\n", (char *)node->data);
        node = node->fd;
    }

    // 写入libc_dbg
    node = libc_dbg->fd;
    while (node != libc_dbg)
    {
        fprintf(fp, "[libc_dbg] %s\n", (char *)node->data);
        node = node->fd;
    }

    fclose(fp);
    return true;
}

bool list_update(const config *conf, list *libc, list *libc_sym)
{
    // 更新list
    if(list_from_url(conf->listCurl, libc, libc_sym) == false)
        return false;
    
    // 写入list文件
    if(list_to_file(conf->listFile, libc, libc_sym) == false)
        return false;

    return true;
}