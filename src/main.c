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

void ch_get(const char *cwd, const config *conf, list *libc)
{
    list *node = libc->fd;

    // 输出菜单
    printf(BCYN("[  0 ] 返回\n"));
    for (int i = 1; node != libc; i++)
    {
        printf(BCYN("[%3d ] %s\n"), i, (char *)node->data);
        node = node->fd;
    }

    // 选择
    int choice = 0;
    printf(BWHT("请选择: "));
    scanf("%d", &choice);
    if (choice <= 0 || choice > listGetCount(libc))
        return;

    // 下载对应的文件
    node = libc->fd;
    for (int i = 1; i < choice; i++)
        node = node->fd;

    // printf("选择%s\n", (char*)node->data);
    if(getlibc(cwd, conf, (char *)node->data) == false)
        printf(BRED("下载失败\n"));
    else
        printf(BGRN("下载成功\n"));
}

int main(char argc, char *argv[])
{
    setlocale(LC_ALL, "zh_CN.UTF-8");

    char cwd[1024];
    getcwd(cwd, sizeof(cwd));

    config conf;
    confInit(&conf);

    list libc;
    list libc_dbg;
    listInitList(&libc);
    listInitList(&libc_dbg);

    // 检查是否更新
    if (argc == 2 && (!strcmp(argv[1], "update") || !strcmp(argv[1], "u") || !strcmp(argv[1], "-u")))
    {
        if (list_update(&conf, &libc, &libc_dbg) == true)
        {
            printf(BGRN("更新成功\n"));
            listDeleteList(&libc, free);
            listDeleteList(&libc_dbg, free);
            return 0;
        }
        ERR("无法更新glibc列表");
        listDeleteList(&libc, free);
        listDeleteList(&libc_dbg, free);
        return 1;
    }

    // 读取列表
    if (list_from_file(conf.listFile, &libc, &libc_dbg) == false)
    {
        if (list_update(&conf, &libc, &libc_dbg) == false)
        {
            ERR("无法更新glibc列表");
            return 1;
        }
    }

    // 目录
    int quit = 1;
    while (quit)
    {
        int choice = 0;
        // 打印菜单
        printf(BCYN("0. 退出\n"));
        printf(BCYN("1. 获取libc\n"));
        printf(BCYN("2. 获取libc-dbg\n"));
        printf(BWHT("请选择: "));
        scanf("%d", &choice);
        switch (choice)
        {
        case 0:
            quit = 0;
            break;
        case 1:
            ch_get(cwd, &conf, &libc);
            break;
        case 2:
            ch_get(cwd, &conf, &libc_dbg);
            break;
        default:
            printf(BRED("无效的选择\n"));
            putchar('\n');
        }
    }

    listDeleteList(&libc, free);
    listDeleteList(&libc_dbg, free);
    return 0;
}