#include "get.h"

/**
 * \brief 获取libc
 * \param conf 配置
 * \param dir 目标目录
 * \param libc libc文件名
 */
bool _getlibc(const config *conf, const char *dir, const char *libc)
{
    int ret = true;
    int r = 0;
    // 合成url
    char url[PATH_MAX];
    strncpy(url, conf->listCurl, sizeof(url) - 1);
    strncat(url, "/pool/main/g/glibc/", sizeof(url) - 1);
    strncat(url, libc, sizeof(url) - 1);

    // 下载文件
    mem *deb = NULL;
    if (webGet(url, &deb) == false)
    {
        ERROR("无法下载文件\n");
        return false;
    }

    // 解压第一层
    mem *data = NULL;
    struct archive *tar = archive_read_new();
    struct archive_entry *entry = NULL;
    archive_read_support_filter_all(tar);
    archive_read_support_format_all(tar);
    if (archive_read_open_memory(tar, deb->m, deb->size) != ARCHIVE_OK)
    {
        ERROR("读取第一层归档文件时出现错误：%s\n", archive_error_string(tar));
        ret = false;
        goto error2;
    }
    while ((r = archive_read_next_header(tar, &entry)) == ARCHIVE_OK || r == ARCHIVE_WARN || r == ARCHIVE_RETRY)
    {
        if (r == ARCHIVE_WARN || r == ARCHIVE_RETRY)
        {
            ERROR("读取第一层归档文件时出现警告或需要重试：%s\n", archive_error_string(tar));
            continue;
        }

        // 获取文件或目录名
        char *pathname = (char *)archive_entry_pathname(entry);
        if ((!pathname) && (archive_entry_filetype(entry) != AE_IFMT))
            continue;

        // 检查这个文件是否时自己要提取的文件
        if (strstr(pathname, "data"))
        {
            // 提取data
            data = Malloc(archive_entry_size(entry));
            archive_read_data(tar, data->m, data->size);
            break;
        }
    }
    if (data == NULL)
    {
        ERROR("解析data失败\n");
        ret = false;
        goto error3;
    }

    // 关闭归档文件
    archive_read_close(tar);
    archive_read_free(tar);

    // 重新打开归档文件
    tar = archive_read_new();
    entry = NULL;
    archive_read_support_filter_all(tar);
    archive_read_support_format_all(tar);

    // 解压第二层
    if (archive_read_open_memory(tar, data->m, data->size) != ARCHIVE_OK)
    {
        ERROR("打开data.tar文件时出现错误：%s\n", archive_error_string(tar));
        archive_read_free(tar);
        ret = false;
        goto error4;
    }

    // 提取debug文件
    bool libc_exist = false;
    bool ld_exist = false;
    bool debug = false;
    while ((r = archive_read_next_header(tar, &entry)) == ARCHIVE_OK || r == ARCHIVE_WARN || r == ARCHIVE_RETRY)
    {
        if (r == ARCHIVE_WARN || r == ARCHIVE_RETRY)
        {
            ERROR("读取data.tar文件时出现警告或需要重试：%s\n", archive_error_string(tar));
            continue;
        }

        // 检查是否为常规文件
        mode_t mode = archive_entry_mode(entry);
        if ((mode & AE_IFMT) != AE_IFREG)
            continue;

        // 获取文件或目录名
        char *pathname = (char *)archive_entry_pathname(entry);
        if (!pathname)
            continue;

        // 检查是否为libc文件
        char *filename = NULL;
        if ((filename = strstr(pathname, "/libc-")) ||
            (filename = strstr(pathname, "/libc.so.6")) ||
            (filename = strstr(pathname, "/ld-")))
        {
            // 创建文件
            char file[PATH_MAX];
            strcpy(file, dir);
            strcat(file, filename);

            FILE *f = createFile(file);
            if (f == NULL)
            {
                ERROR("无法创建文件: %s\n", file);
                continue;
            }

            // 写入文件
            mem *content = Malloc(archive_entry_size(entry));
            archive_read_data(tar, content->m, content->size);
            fwrite(content->m, 1, content->size, f);
            fclose(f);
            Free(content);

            // 修改权限
            chmod(file, 0777);

            // 设置标志
            if (strstr(filename, "libc-") || strstr(filename, "libc.so.6"))
                libc_exist = true;
            else if (strstr(filename, "ld-"))
                ld_exist = true;

            SUCESS("提取成功: %s -> %s\n", filename, file);
        }
        else if (filename = strstr(pathname, "debug"))
        {
            filename = strchr(filename, '/');
            if (filename++ == NULL)
                continue;
            char file[PATH_MAX];
            strcpy(file, dir);
            strcat(file, filename);

            FILE *f = createFile(file);
            if (f == NULL)
            {
                ERROR("无法创建文件: %s\n", file);
                continue;
            }

            // 写入文件
            mem *content = Malloc(archive_entry_size(entry));
            archive_read_data(tar, content->m, content->size);
            fwrite(content->m, 1, content->size, f);
            fclose(f);
            Free(content);

            // 修改权限
            chmod(file, 0777);

            debug = true;
        }
    }
    if (((libc_exist == false) || (ld_exist == false)) && (debug == false))
    {
        ERROR("提取glibc文件失败, 未找到libc或者ld, 或者未找到debug\n");
        ret = false;
    }

error4:
    // 释放data内存
    Free(data);
error3:
    // 关闭归档文件
    archive_read_close(tar);
error2:
    // 释放内存
    archive_read_free(tar);
error1:
    // 释放deb内存
    Free(deb);
    return ret;
}

/**
 * \brief 获取libc
 * \param conf 配置
 * \param cwd 当前目录
 * \param libc libc文件名
 */
bool getlibc(const config *conf, const char *cwd, const char *libc)
{
    char libc_debug[PATH_MAX] = {"libc6-dbg_"};
    strcat(libc_debug, &libc[6]);

    // 获取glibc文件
    if (_getlibc(conf, cwd, libc) == false)
        return false;

    // 获取glibc-debug文件
    if (_getlibc(conf, conf->debugDir, libc_debug) == false)
        return false;

    return true;
}

// bool getlibc(const char *cwd, const config *conf, const char *libc)
// {
//     bool ret = true;
//     // 创建目录
//     char path[1024];
//     strcpy(path, cwd);
//     strcat(path, "/");
//     strncat(path, libc, strlen(libc) - 4);

//     // 创建目录
//     if (mkdir(path, 0755) != 0 && errno != EEXIST)
//     {
//         ERROR("无法创建目录\n");
//         return false;
//     }

//     // 下载文件
//     mem *m = NULL;
//     char url[2048]; // 增加缓冲区大小
//     // snprintf(url, sizeof(url), "%s/pool/main/g/glibc/%s", conf->listCurl, libc);
//     if (webGet(url, &m) == false)
//     {
//         ERROR("无法下载文件\n");
//         return false;
//     }

//     // 解压第一层
//     struct archive *a = archive_read_new();
//     archive_read_support_filter_all(a);
//     archive_read_support_format_all(a);
//     if (archive_read_open_memory(a, m->m, m->size) != ARCHIVE_OK)
//     {
//         ERROR("打开归档文件时出现错误：%s\n", archive_error_string(a));
//         archive_read_free(a);
//         ret = false;
//         goto error1;
//     }
//     struct archive_entry *entry = NULL;
//     int r;
//     while ((r = archive_read_next_header(a, &entry)) == ARCHIVE_OK || r == ARCHIVE_WARN || r == ARCHIVE_RETRY)
//     {
//         if (r == ARCHIVE_WARN || r == ARCHIVE_RETRY)
//         {
//             ERROR("读取归档文件时出现警告或需要重试：%s\n", archive_error_string(a));
//             continue;
//         }

//         // 获取文件或目录名
//         char *pathname = (char *)archive_entry_pathname(entry);
//         if ((!pathname) && (archive_entry_filetype(entry) != AE_IFMT))
//             continue;

//         // 检查这个文件是否时自己要提取的文件
//         if (strstr(pathname, "data"))
//         {
//             char libcPath[1024] = {0};

//             // 提取data
//             mem *data = Malloc(archive_entry_size(entry));
//             archive_read_data(a, data->m, data->size);

//             // 获取文件目录
//             struct archive *a = archive_read_new();
//             archive_read_support_filter_all(a);
//             archive_read_support_format_all(a);
//             if (archive_read_open_memory(a, data->m, data->size) != ARCHIVE_OK)
//             {
//                 ERROR("打开归档文件时出现错误：%s\n", archive_error_string(a));
//                 archive_read_free(a);
//                 ret = false;
//                 goto error1;
//             }
//             struct archive_entry *entry = NULL;
//             int r;
//             while ((r = archive_read_next_header(a, &entry)) == ARCHIVE_OK || r == ARCHIVE_WARN || r == ARCHIVE_RETRY)
//             {
//                 if (r == ARCHIVE_WARN || r == ARCHIVE_RETRY)
//                 {
//                     ERROR("读取归档文件时出现警告或需要重试：%s\n", archive_error_string(a));
//                     continue;
//                 }

//                 // 获取文件或目录名
//                 char *pathname = (char *)archive_entry_pathname(entry);
//                 if ((!pathname) && (archive_entry_filetype(entry) != AE_IFMT))
//                     continue;

//                 // 搜索glibc文件
//                 char *s;
//                 if ((s = strstr(pathname, "libc-")) ||
//                     (s = strstr(pathname, "libc.so.6")))
//                 {
//                     strncpy(libcPath, pathname, s - pathname);
//                     break;
//                 }
//             }
//             // 关闭归档文件
//             archive_read_close(a);
//             archive_read_free(a);

//             // 检查是否找到glibc文件
//             if (libcPath[0] == '\0')
//             {
//                 ERROR("解析data失败, 没有找到glibc文件\n");
//                 Free(data);
//                 ret = false;
//                 goto error2;
//             }

//             // 提取所有文件
//             a = archive_read_new();
//             archive_read_support_filter_all(a);
//             archive_read_support_format_all(a);
//             if (archive_read_open_memory(a, data->m, data->size) != ARCHIVE_OK)
//             {
//                 ERROR("打开归档文件时出现错误：%s\n", archive_error_string(a));
//                 archive_read_free(a);
//                 ret = false;
//                 goto error1;
//             }

//             entry = NULL;
//             while ((r = archive_read_next_header(a, &entry)) == ARCHIVE_OK || r == ARCHIVE_WARN || r == ARCHIVE_RETRY)
//             {
//                 if (r == ARCHIVE_WARN || r == ARCHIVE_RETRY)
//                 {
//                     ERROR("读取归档文件时出现警告或需要重试：%s\n", archive_error_string(a));
//                     continue;
//                 }

//                 // 获取文件或目录名
//                 char *pathname = (char *)archive_entry_pathname(entry);
//                 if ((!pathname) && (archive_entry_filetype(entry) != AE_IFMT))
//                     continue;
//                 if (archive_entry_filetype(entry) == AE_IFDIR)
//                     continue;

//                 // 找到同目录文件
//                 if (!strncmp(pathname, libcPath, strlen(libcPath)))
//                 {
//                     // 不用子目录
//                     if(strstr(pathname + strlen(libcPath), "/"))
//                         continue;

//                     // 创建文件
//                     char file[2048];
//                     strcpy(file, path);
//                     strcat(file, "/");
//                     strcat(file, pathname + strlen(libcPath));

//                     FILE *f = fopen(file, "wb");
//                     if (f == NULL)
//                     {
//                         ERROR("无法创建文件: %s\n", file);
//                         continue;
//                     }

//                     // 写入文件
//                     mem *data = Malloc(archive_entry_size(entry));
//                     archive_read_data(a, data->m, data->size);
//                     fwrite(data->m, 1, data->size, f);
//                     fclose(f);
//                     Free(data);

//                     // 修改权限
//                     chmod(file, 0755);
//                 }
//             }
//             // 关闭归档文件
//             archive_read_close(a);
//             archive_read_free(a);

//             Free(data);

//             break;
//         }
//     }
// error2:
//     // 关闭归档文件
//     archive_read_close(a);
//     archive_read_free(a);

// error1:
//     Free(m);
//     return ret;
// }