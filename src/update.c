#include "update.h"

bool parse(const char *str, list *libc)
{
    if (str == NULL || libc == NULL)
        return false;

    // 读取文件
    char *head = strchr(strstr(strstr((char *)str, "pool/main/g/glibc:"), "total"), '\n') + 1;
    if (head == NULL)
    {
        ERROR("无法找到 pool/main/g/glibc 目录\n");
        return false;
    }

    char *end = strstr(head, "\n\n");

    // 初始化libc
    listDeleteList(libc, free);

    // 拷贝pool/main/g/glibc目录
    char *text = (char *)malloc(end - head + 1);
    memcpy(text, head, end - head);
    text[end - head] = '\0';

    // 每一行进行分析
    int libc_count = 0;
    int libc_dbg_count = 0;
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
            {
                libc_dbg_count++;
            }
        }
        // libc文件
        else if (tmp = strstr(line, "libc6_"))
        {
            // 检测是否是AMD64或者i386
            if (strstr(tmp, "amd64") || strstr(tmp, "i386"))
            {
                listAddNodeInEnd(libc, listDataToNode(listCreateNode(), tmp, strlen(tmp) + 1, true));
                libc_count++;
            }
        }

        line = strtok(NULL, "\n");
    }

    // free
    free(text);

    // 检查libc和libc_dbg是否对应
    if (libc_count != libc_dbg_count)
        ERROR("libc和libc_dbg数量不对应\n");

    return true;
}

bool list_from_url(const char *url, list *libc)
{
    char ls_url[PATH_MAX] = {0};

    if (url == NULL || libc == NULL)
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
        return false;
    }

    // 解压ls-lR.gz
    mem *out = NULL;
    if (gz(m, &out) == false)
    {
        Free(m);
        return false;
    }

    // 解析ls-lR
    if (parse(out->m, libc) == false)
    {
        Free(m);
        Free(out);
        return false;
    }

    Free(m);
    Free(out);

    return true;
}

bool list_from_file(const char *file, list *libc)
{
    if (file == NULL || libc == NULL)
        return false;

    // 检查文件是否能打开
    FILE *fp = fopen(file, "r");
    if (fp == NULL)
    {
        ERROR("无法打开文件 %s\n", file);
        return false;
    }

    // 初始化libc和libc_sym
    listDeleteList(libc, free);

    // 读取配置
    char path[PATH_MAX] = {0};
    while (!feof(fp))
    {
        fscanf(fp, "%s", path);
        if(strstr(path, "libc"))
            listAddNodeInEnd(libc, listDataToNode(listCreateNode(), path, strlen(path) + 1, true));
    }
    fclose(fp);
    return true;
}

bool list_to_file(const char *file, list *libc)
{
    if (file == NULL || libc == NULL)
        return false;

    // 检查文件是否能打开
    FILE *fp = fopen(file, "w");
    if (fp == NULL)
    {
        ERROR("无法打开文件 %s\n", file);
        return false;
    }

    // 写入libc
    list *node = libc->fd;
    while (node != libc)
    {
        fputs((char *)node->data, fp);
        fputc('\n', fp);

        node = node->fd;
    }

    fclose(fp);
    return true;
}

bool list_update(const config *conf, list *libc)
{
    // 更新list
    if (list_from_url(conf->listCurl, libc) == false)
        return false;

    // 写入list文件
    if (list_to_file(conf->listFile, libc) == false)
        return false;

    return true;
}