#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <locale.h>

#include <archive.h>

#include "gz.h"
#include "web.h"
#include "user.h"
#include "list.h"
#include "config.h"

#include "update.h"
#include "get.h"

#if 1
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
    {
        perror("strdup");
        return -1;
    }

    char *p = temp_path;
    while (*p != '\0')
    {
        if (*p == '/')
        {
            *p = '\0';
            if (strlen(temp_path) > 0 && mkdir(temp_path, mode) != 0 && errno != EEXIST)
            {
                perror("mkdir");
                free(temp_path);
                return -1;
            }
            *p = '/';
        }
        p++;
    }

    if (mkdir(temp_path, mode) != 0 && errno != EEXIST)
    {
        perror("mkdir");
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
    // 创建文件
    FILE *fp = fopen(path, "wb");
    if (fp == NULL)
    {
        // 提取目录路径
        char *dir_path = strdup(path);
        if (dir_path == NULL)
        {
            perror("strdup");
            return NULL;
        }

        char *last_slash = strrchr(dir_path, '/');
        if (last_slash != NULL)
        {
            *last_slash = '\0';
            // 创建目录
            if (mkdir_p(dir_path, 0755) != 0)
            {
                free(dir_path);
                return NULL;
            }
        }

        free(dir_path);

        // 创建文件
        fp = fopen(path, "wb");
        if (fp == NULL)
        {
            perror("fopen");
            return NULL;
        }
    }

    return fp;
}

int main()
{
    const char *path = "/home/tuto/github/gglibc/a/b/c/d/file.txt";

    FILE *fp = createFile(path);
    if (fp != NULL)
    {
        printf("文件 %s 创建成功\n", path);
        fclose(fp);
    }
    else
    {
        printf("文件 %s 创建失败\n", path);
    }

    return 0;
}

#else

int main(char argc, char *argv[])
{
    setlocale(LC_ALL, "zh_CN.UTF-8");

    // 获取当前目录
    char cwd[PATH_MAX];
    getcwd(cwd, sizeof(cwd));

    // 读取配置
    config conf;
    confInit(&conf);

    list libc;
    listInitList(&libc);

    // 检查是否更新
    if (argc == 2 && (strchr(argv[1], 'u') || strchr(argv[1], 'U')))
    {
        if (list_update(&conf, &libc) == false)
        {
            listDeleteList(&libc, free);
            return 1;
        }
    }
    else
    {
        if (list_from_file(conf.listFile, &libc) == false)
        {
            if (list_update(&conf, &libc) == false)
            {
                listDeleteList(&libc, free);
                return 1;
            }
        }
    }

    int quit = 1;
    while (quit)
    {
        list *node = libc.fd;
        for (int i = listGetCount(&libc); node != &libc; i--, node = node->fd)
            DEBUG(BCYN("[ %2d ] %s\n"), i, (char *)node->data);
        DEBUG(BCYN("[  0 ] 退出\n"));
        DEBUG(BYEL("请输入选择 : "));

        int choice = 0;
        if (scanf("%d", &choice) == 1)
        {
            if (choice == 0)
                quit = 0;
            else if (choice > 0 && choice <= listGetCount(&libc))
            {
                node = libc.fd;
                for (int i = 1; i < choice; i++)
                    node = node->fd;
                if (getlibc(cwd, &conf, (char *)node->data) == false)
                {
                    ERROR("下载失败\n");
                    DEBUG(BYEL("按任意键继续\n"));
                    pause();
                }
                else
                {
                    SUCESS("下载成功\n");
                    DEBUG(BYEL("按任意键继续\n"));
                    pause();
                }
            }
        }
    }

    listDeleteList(&libc, free);
    return 0;
}
#endif