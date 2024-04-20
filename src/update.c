#include <file.h>

struct string *ls_libc = NULL;
struct string *ls_libc_dbg = NULL;

/**
 * \brief 写入文件信息到list
 * \return 是否成功
*/
bool putscontenttolist()
{
    FILE *file = NULL;          // list文件
    struct string *node = NULL; // 链表节点
    
    file = fopen("list", "w");
    if (file == NULL)
    {
        fprintf(stderr, BRED("无法打开list文件\n"));
        return false;
    }
    // 写入libc信息
    fprintf(file, "libc:\n");
    node = ls_libc;
    while (node)
    {
        fprintf(file, "%s\n", node->str);
        node = node->next;
    }
    // 写入libc-dbg信息
    fprintf(file, "libc-dbg:\n");
    node = ls_libc_dbg;
    while (node)
    {
        fprintf(file, "%s\n", node->str);
        node = node->next;
    }
    fclose(file);
    return true;
}

/**
 * \brief 从list读取文件信息
 * \return 是否成功
*/
bool getcontentfromlist()
{
    FILE *file  = NULL; // list文件
    char *text  = NULL; // 读取的行
    size_t size = 0;    // 文件大小
    char *libc  = NULL; // libc行
    char *libc_dbg = NULL; // libc-dbg行
    char *line  = NULL; // 行
    struct string *tmp = NULL;

    // 读取list文件
    file = fopen("list", "r");
    if (file == NULL)
    {
        fprintf(stderr, BRED("无法打开list文件, 请先更新list\n"));
        return false;
    }
    fseek(file, 0, SEEK_END);
    size = ftell(file);
    rewind(file);
    text = (char *)MALLOC(size + 1);
    fread(text, size, 1, file);
    text[size] = '\0';
    fclose(file);

    // 初始化ls_libc和ls_libc_dbg
    strFree(&ls_libc);
    strFree(&ls_libc_dbg);

    // 搜索libc和libc_dbg部分
    libc = strstr(text, "libc:\n") + strlen("libc:\n");
    libc_dbg = strstr(text, "libc-dbg:\n");
    *libc_dbg = '\0';
    libc_dbg += strlen("libc-dbg:\n");

    // 写入libc
    line = strtok(libc, "\n");
    while (line)
    {
        strAdd(&tmp, line, true);
        line = strtok(NULL, "\n");
    }
    while(tmp)
        strAdd(&ls_libc, strGet(&tmp), false);
    // 写入libc_dbg
    line = strtok(libc_dbg, "\n");
    while (line)
    {
        strAdd(&tmp, line, true);
        line = strtok(NULL, "\n");
    }
    while(tmp)
        strAdd(&ls_libc_dbg, strGet(&tmp), false);

    // free
    free(text);

    return true;
}

/**
 * \brief 从ls中获取文件信息
 * \param str 字符串
 * \return 是否成功
*/
bool getcontenttolsfile(const char *str)
{
    char *head = strchr(strstr(strstr(str, "pool/main/g/glibc:"), "total"), '\n') + 1;
    char *end = strstr(head, "\n\n");
    if (head == NULL)
    {
        fprintf(stderr, BRED("无法找到 pool/main/g/glibc 目录\n"));
        return false;
    }

    // 初始化ls_libc和ls_libc_dbg
    strFree(&ls_libc);
    strFree(&ls_libc_dbg);

    // 拷贝pool/main/g/glibc目录
    char *text = (char *)MALLOC(end - head + 1);
    memcpy(text, head, end - head);
    text[end - head] = '\0';

    // 循环写入数据
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
                strAdd(&ls_libc_dbg, tmp, true);
        }
        // libc文件
        else if (tmp = strstr(line, "libc6_"))
        {
            // 检测是否是AMD64或者i386
            if (strstr(tmp, "amd64") || strstr(tmp, "i386"))
                strAdd(&ls_libc, tmp, true);
        }

        line = strtok(NULL, "\n");
    }

    // free
    free(text);
    return true;
}

/**
 * \brief 更新数据
 * \return 是否成功
*/
bool update()
{
    struct mem ls_in;  // 解压前的ls文件
    struct mem ls_out; // 解压后的ls文件

    // 获取ls-lR.gz
    printf(BYEL("准备下载文件: ") BLU("%s\n"), GLIBC_LS);
    if(getdatafromcurl(GLIBC_LS, &ls_in) == false)
    {
        fprintf(stderr, BRED("获取%s失败!\n"), GLIBC_LS);
        return false;
    }

    // 解压ls-lR.gz
    printf(BYEL("准备解压文件: ") BLU("%s\n"), GLIBC_LS);
    if (gz(&ls_in, &ls_out) == false)
    {
        FREE(ls_in);
        fprintf(stderr, BRED("解压%s失败!\n"), GLIBC_LS);
        return false;
    }

    // 解析ls-lR.gz
    printf(BYEL("正在解析文件...\n"));
    if (getcontenttolsfile((const char *)ls_out.data) == false)
    {
        FREE(ls_in);
        FREE(ls_out);
        fprintf(stderr, BRED("解析文件失败!\n"));
        return false;
    }
    printf(BGRN("解析文件完成\n"));

    // 写入list
    printf(BYEL("正在更新list...\n"));
    if (putscontenttolist() == false)
    {
        FREE(ls_in);
        FREE(ls_out);
        fprintf(stderr, BRED("写入list失败!\n"));
        return false;
    }
    printf(BGRN("更新list完成\n"));

    FREE(ls_in);
    FREE(ls_out);

    return true;
}
