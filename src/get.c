#include "get.h"

bool getlibc(const char *cwd, const config *conf, const char *libc)
{
    bool ret = true;
    // 创建目录
    char path[1024];
    strcpy(path, cwd);
    strcat(path, "/");
    strncat(path, libc, strlen(libc) - 4);

    // 创建目录
    if (mkdir(path, 0755) != 0 && errno != EEXIST)
    {
        ERROR("无法创建目录\n");
        return false;
    }

    // 下载文件
    mem *m = NULL;
    char url[2048]; // 增加缓冲区大小
    snprintf(url, sizeof(url), "%s/pool/main/g/glibc/%s", conf->listCurl, libc);
    if (webGet(url, &m) == false)
    {
        ERROR("无法下载文件\n");
        return false;
    }

    // 解压第一层
    struct archive *a = archive_read_new();
    archive_read_support_filter_all(a);
    archive_read_support_format_all(a);
    if (archive_read_open_memory(a, m->m, m->size) != ARCHIVE_OK)
    {
        ERROR("打开归档文件时出现错误：%s\n", archive_error_string(a));
        archive_read_free(a);
        ret = false;
        goto error1;
    }
    struct archive_entry *entry = NULL;
    int r;
    while ((r = archive_read_next_header(a, &entry)) == ARCHIVE_OK || r == ARCHIVE_WARN || r == ARCHIVE_RETRY)
    {
        if (r == ARCHIVE_WARN || r == ARCHIVE_RETRY)
        {
            ERROR("读取归档文件时出现警告或需要重试：%s\n", archive_error_string(a));
            continue;
        }

        // 获取文件或目录名
        char *pathname = (char *)archive_entry_pathname(entry);
        if ((!pathname) && (archive_entry_filetype(entry) != AE_IFMT))
            continue;

        // 检查这个文件是否时自己要提取的文件
        if (strstr(pathname, "data"))
        {
            char libcPath[1024] = {0};

            // 提取data
            mem *data = Malloc(archive_entry_size(entry));
            archive_read_data(a, data->m, data->size);

            // 获取文件目录
            struct archive *a = archive_read_new();
            archive_read_support_filter_all(a);
            archive_read_support_format_all(a);
            if (archive_read_open_memory(a, data->m, data->size) != ARCHIVE_OK)
            {
                ERROR("打开归档文件时出现错误：%s\n", archive_error_string(a));
                archive_read_free(a);
                ret = false;
                goto error1;
            }
            struct archive_entry *entry = NULL;
            int r;
            while ((r = archive_read_next_header(a, &entry)) == ARCHIVE_OK || r == ARCHIVE_WARN || r == ARCHIVE_RETRY)
            {
                if (r == ARCHIVE_WARN || r == ARCHIVE_RETRY)
                {
                    ERROR("读取归档文件时出现警告或需要重试：%s\n", archive_error_string(a));
                    continue;
                }

                // 获取文件或目录名
                char *pathname = (char *)archive_entry_pathname(entry);
                if ((!pathname) && (archive_entry_filetype(entry) != AE_IFMT))
                    continue;

                // 搜索glibc文件
                char *s;
                if ((s = strstr(pathname, "libc-")) ||
                    (s = strstr(pathname, "libc.so.6")))
                {
                    strncpy(libcPath, pathname, s - pathname);
                    break;
                }
            }
            // 关闭归档文件
            archive_read_close(a);
            archive_read_free(a);

            // 检查是否找到glibc文件
            if (libcPath[0] == '\0')
            {
                ERROR("解析data失败, 没有找到glibc文件\n");
                Free(data);
                ret = false;
                goto error2;
            }

            // 提取所有文件
            a = archive_read_new();
            archive_read_support_filter_all(a);
            archive_read_support_format_all(a);
            if (archive_read_open_memory(a, data->m, data->size) != ARCHIVE_OK)
            {
                ERROR("打开归档文件时出现错误：%s\n", archive_error_string(a));
                archive_read_free(a);
                ret = false;
                goto error1;
            }

            
            entry = NULL;
            while ((r = archive_read_next_header(a, &entry)) == ARCHIVE_OK || r == ARCHIVE_WARN || r == ARCHIVE_RETRY)
            {
                if (r == ARCHIVE_WARN || r == ARCHIVE_RETRY)
                {
                    ERROR("读取归档文件时出现警告或需要重试：%s\n", archive_error_string(a));
                    continue;
                }

                // 获取文件或目录名
                char *pathname = (char *)archive_entry_pathname(entry);
                if ((!pathname) && (archive_entry_filetype(entry) != AE_IFMT))
                    continue;
                if (archive_entry_filetype(entry) == AE_IFDIR)
                    continue;

                // 找到同目录文件
                if (!strncmp(pathname, libcPath, strlen(libcPath)))
                {
                    // 不用子目录
                    if(strstr(pathname + strlen(libcPath), "/"))
                        continue;

                    // 创建文件
                    char file[2048];
                    strcpy(file, path);
                    strcat(file, "/");
                    strcat(file, pathname + strlen(libcPath));

                    FILE *f = fopen(file, "wb");
                    if (f == NULL)
                    {
                        ERROR("无法创建文件: %s\n", file);
                        continue;
                    }

                    // 写入文件
                    mem *data = Malloc(archive_entry_size(entry));
                    archive_read_data(a, data->m, data->size);
                    fwrite(data->m, 1, data->size, f);
                    fclose(f);
                    Free(data);

                    // 修改权限
                    chmod(file, 0755);
                }
            }
            // 关闭归档文件
            archive_read_close(a);
            archive_read_free(a);

            Free(data);

            break;
        }
    }
error2:
    // 关闭归档文件
    archive_read_close(a);
    archive_read_free(a);

error1:
    Free(m);
    return ret;
}