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
#include "file.h"
#include "config.h"

#include "update.h"
#include "get.h"

#if 0

#else

int main(char argc, char *argv[])
{
    setlocale(LC_ALL, "zh_CN.UTF-8");

    // 获取当前目录
    char cwd[PATH_MAX];
    getcwd(cwd, sizeof(cwd));
    if (cwd[strlen(cwd) - 1] != '/')
        strcat(cwd, "/");

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
        int ret = scanf("%d", &choice);
        setvbuf(stdin, NULL, _IONBF, 0);
        if (ret == 1)
        {
            if (choice == 0)
                quit = 0;
            else if (choice > 0 && choice <= listGetCount(&libc))
            {
                node = libc.bk;
                for (int i = 1; i < choice; i++, node = node->bk);
                if (getlibc(&conf, cwd, (char *)node->data) == false)
                {
                    ERROR("下载失败\n");
                    DEBUG(BYEL("按任意键继续\n"));
                    getchar();
                }
                else
                {
                    SUCESS("下载成功\n");
                    DEBUG(BYEL("按任意键继续\n"));
                    getchar();
                }
            }
        }
    }

    listDeleteList(&libc, free);
    return 0;
}
#endif