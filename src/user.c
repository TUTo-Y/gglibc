#include "user.h"

// 初始化配置
bool confInit(config *conf)
{
    FILE *fp = NULL;

    if (!conf)
        return false;

    // 获取用户根目录
    char config_file[1024] = {0};
    strcpy(config_file, getenv("HOME"));
    strcat(config_file, "/.gglibc.conf");
    
    // 如果配置文件不存在，则创建
    fp = fopen(config_file, "r");
    if (fp == NULL)
    {
        fp = fopen(config_file, "w+");
        if (fp == NULL)
            return false;

        // 写入配置
        fprintf(fp, "[listFile] /home/tuto/github/gglibc/list\n");
        fprintf(fp, "[listCurl] https://mirrors.tuna.tsinghua.edu.cn/ubuntu/");

        fseek(fp, 0l, SEEK_SET);
    }

    // 读取配置
    while (!feof(fp))
    {
        char line[1024] = {0};
        char key[1024] = {0};
        char value[1024] = {0};

        fgets(line, sizeof(line), fp);
        sscanf(line, "[%[^]]] %s\n", key, value);

        if (strcmp(key, "listFile") == 0)
        {
            strcpy(conf->listFile, value);
        }
        else if (strcmp(key, "listCurl") == 0)
        {
            strcpy(conf->listCurl, value);
            if(conf->listCurl[strlen(conf->listCurl) - 1] != '/')
            {
                strcat(conf->listCurl, "/");
            }
        }
    }
    fclose(fp);
}
