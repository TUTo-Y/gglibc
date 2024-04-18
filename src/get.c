#include "file.h"

int main(int argc, char **argv)
{
    list *hand = NULL;
    list *tmp  = NULL;
    FILE *file = NULL;
    char str[256]   = { 0 };
    char str2[256]  = { 0 };

    Init();

    // 读取list
    file = fopen("./list.txt", "r");
    if(file == NULL)
    {
        perror("无法打开list.txt\n");
        return -1;
    }
    readlist(&hand, file);
    fclose(file);

    if(hand == NULL)
    {
        perror("list.txt为空\n请先运行update\n");
        return -1;
    }

    // 选择文件下载
    if(argc <= 1)
    {
        // 要求用户输入
        tmp = hand;
        while(tmp)
        {
            printf("[%3d ] %s\n", tmp->id, tmp->name);
            tmp = (list*)tmp->next;
        }
        printf("你也可以选择:all\n> ");
        scanf("%s", str);
        sprintf(str2, "%s", str);
        
        // 全部下载
        if(!strcmp(str, "all"))
            goto ALL;

        // 通过输入的字符串或者序号获取文件
        tmp = hand;
        while(tmp)
        {
            if((!strncmp(tmp->name, str2, strlen(tmp->name))) || (tmp->id == atoi(str)))
            {
                getfile(tmp->name);
                break;
            }
            else
                tmp = (list*)tmp->next;
        }
    }
    // 全部下载
    else if(!strcmp("all", argv[1]))
    {
ALL:
        tmp = hand;
        while(tmp)
        {
            getfile(tmp->name);
            tmp = (list*)tmp->next;
        }
    }
    // 输入序号或者字符串
    else
    {
        // 通过输入的字符串或者序号获取文件
        for(int i = 1; i<argc; i++)
        {
            tmp = hand;
            while(tmp)
            {
                if((!strcmp(tmp->name, argv[i])) || (tmp->id == atoi(argv[i])))
                {
                    getfile(tmp->name);
                    break;
                }
                else
                    tmp = (list*)tmp->next;
            }
            if(!tmp)
                printf("未知参数:%s\n", argv[i]);
        }
    }

    // 释放链表
    freelist(&hand);
    Quit();
    return 0;
}