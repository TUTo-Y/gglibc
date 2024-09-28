#include "file.h"

/**
 * \brief 递归创建多层目录
 * \param path 要创建的目录路径
 * \param mode 目录权限
 * \return 成功返回0，失败返回-1
 */
int mkdir_p(const char *path, mode_t mode)
{
    char *temp_path = strdup(path);
    if (temp_path == NULL)
        return -1;

    char *p = temp_path;
    while (*p != '\0')
    {
        if (*p == '/')
        {
            *p = '\0';
            if (strlen(temp_path) > 0 && mkdir(temp_path, mode) != 0 && errno != EEXIST)
            {
                free(temp_path);
                return -1;
            }
            *p = '/';
        }
        p++;
    }

    if (mkdir(temp_path, mode) != 0 && errno != EEXIST)
    {
        free(temp_path);
        return -1;
    }

    free(temp_path);
    return 0;
}

/**
 * \brief 创建文件，如果目录不存在则先依次创建目录
 * \param path 文件路径
 * \return 成功返回文件指针，失败返回NULL
 */
FILE *createFile(const char *path)
{
    // 尝试创建文件
    FILE *fp = fopen(path, "wb");
    if (fp != NULL)
    {
        return fp;
    }

    // 提取目录路径
    char *dir_path = strdup(path);
    if (dir_path == NULL)
    {
        ERROR("创建文件失败: %s\n", path);
        return NULL;
    }

    char *last_slash = strrchr(dir_path, '/');
    if (last_slash != NULL)
    {
        *last_slash = '\0';
        // 创建目录
        if (mkdir_p(dir_path, 0755) != 0)
        {
            ERROR("创建文件失败: %s\n", path);
            free(dir_path);
            return NULL;
        }
    }

    free(dir_path);

    // 再次尝试创建文件
    fp = fopen(path, "wb");
    if (fp == NULL)
    {
        ERROR("创建文件失败: %s\n", path);
    }

    return fp;
}
