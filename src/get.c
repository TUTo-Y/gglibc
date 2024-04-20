#include <file.h>

/**
 * \brief 获取glibc
 * \param name 名称
 * \return 是否成功
*/
bool getlibc(const char *name)
{
    char *s = NULL;
    struct string *node     = NULL;
    struct string **_node   = NULL;

    /* 创建目录 */
    char path[PATH_MAX] =   { 0 };                                      // libc的保存路径
    if (mkdir("glibc", 0755) == -1 && errno != EEXIST)
    {
        fprintf(stderr, BRED("无法创建glibc目录\n"));
        return false;
    }
    snprintf(path, sizeof(path), "glibc/%s", name);
    path[strlen(path) - 4] = '\0';
    if (mkdir(path, 0755) == -1 && errno != EEXIST)
    {
        fprintf(stderr, BRED("无法创建glibc目录\n"));
        return false;
    }
    path[strlen(path)+1] = '\0';
    path[strlen(path)] = '/';
    
    /* 获取deb */
    struct mem deb;
    char curl[PATH_MAX] =   { 0 };
    snprintf(curl, sizeof(curl), "%s%s", GLIBC_DIR, name);
    printf(BYEL("准备下载文件: ") BLU("%s\n"), curl);
    if(getdatafromcurl(curl, &deb) == false)                            // 获取deb归档文件到deb
    {
        fprintf(stderr, BRED("获取%s失败!\n"), curl);
        return false;
    }

    /* 解析deb */
    struct string *debpath = NULL;
    if(getpathfromarchive(&deb, NULL, &debpath) == false)               // 解析deb目录到debpath
    {
        fprintf(stderr, BRED("解析%s失败!\n"), curl);
        FREE(deb);
        return false;
    }
    _node = &debpath;
    while(*_node != NULL)                                               // 去掉不是data的文件
    {
        if(!strstr((*_node)->str, "data"))
        {
            free(strGet(_node));
            continue;
        }
        _node = &(*_node)->next;
    }
    if(!strstr(debpath->str, "data"))                                    // 未找到data
    {
        fprintf(stderr, BRED("解析%s失败：未找到data文件\n"), curl);
        FREE(deb);
        strFree(&debpath);
        return false;
    }

    /* 提取libc */
    char   libcpath[PATH_MAX] = { 0 };                                      // data中libc的路径
    struct mem libc;
    struct list *list = NULL;
    if(getdatafromarchive(&deb, debpath, &list) == false)                   // 从deb中提取data到libc
    {
        fprintf(stderr, BRED("提取%s失败!\n"), debpath->str);
        FREE(deb);
        strFree(&debpath);
        return false;
    }
    if(list == NULL)                                                        // 未找到data提取失败
    {
        fprintf(stderr, BRED("提取%s失败!\n"), debpath->str);
        FREE(deb);
        strFree(&debpath);
        return false;
    }
    libc.data = list->m->data;
    libc.size = list->m->size;
    free(list->m);                                                          // 清除无用的数据
    free(list);
    list = NULL;
    FREE(deb);
    strFree(&debpath);
    if(getpathfromarchive(&libc, NULL, &debpath) == false)                  // 解析data到debpath
    {
        fprintf(stderr, BRED("解析data失败!\n"));
        FREE(libc);
        return false;
    }
    node = debpath;
    while(node)
    {
        if((s = strstr(node->str, "libc-")) || (s = strstr(node->str, "libc.so.6")))
        {
            strncpy(libcpath, node->str, s - node->str);
            break;
        }
        node = node->next;
    }
    if(libcpath[0] == '\0')                                             // 未找到libc
    {
        fprintf(stderr, BRED("解析data失败, 没有找到glibc文件\n"));
        FREE(libc);
        strFree(&debpath);
        return false;
    }
    printf("目录:%s\n", libcpath);
    _node = &debpath;                                                   // 删除所有目录不在libc下的文件
    while(*_node != NULL)
    {
        if(strncmp((*_node)->str, libcpath, strlen(libcpath)))
        {
            free(strGet(_node));
            continue;
        }
        if(strchr((*_node)->str + strlen(libcpath), '/'))
        {
            free(strGet(_node));
            continue;
        }
        _node = &(*_node)->next;
    }
    // 解压文件数据
    if(getdatafromarchive(&libc, debpath, &list) == false)              // 解压libc到path
    {
        fprintf(stderr, BRED("获取glibc文件失败!\n"));
        FREE(libc);
        strFree(&debpath);
        return false;
    }
    FREE(libc);
    if(list == NULL)                                                    // 未找到libc提取失败
    {
        fprintf(stderr, BRED("获取glibc文件失败!\n"));
        FREE(libc);
        strFree(&debpath);
        return false;
    }

    /* 写入对应的文件 */
    while(s = strGet(&debpath))
    {
        char sopath[PATH_MAX] = { 0 };
        snprintf(sopath, sizeof(sopath), "%s%s", path, s+strlen(libcpath)); // 计算目录
        
        FILE *fp = fopen(sopath, "wb");                                     // 写入文件
        if(fp == NULL)
        {
            fprintf(stderr, BRED("忽略 : 无法创建文件%s\n"), sopath);
            FREE(*dataGet(&list));
            continue;
        }

        struct mem *tmp = NULL;
        tmp = dataGet(&list);
        fwrite(tmp->data, 1, tmp->size, fp);
        FREE(*tmp);

        fclose(fp);
        printf(BGRN("成功写入文件: ") GRN("%s\n"), sopath);
    }

    return true;
}