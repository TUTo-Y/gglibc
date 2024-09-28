#include "user.h"

// 初始化配置
bool confInit(config *conf)
{
    FILE *fp = NULL;

    if (!conf)
        return false;

    // 获取默认用户配置文件目录
    char *home = getenv("HOME");
    if (!home)
    {
        ERROR("无法获取 HOME 环境变量\n");
        return false;
    }

    char config_file[PATH_MAX] = {0};
    strncpy(config_file, home, sizeof(config_file) - 1);
    strncat(config_file, "/.gglibc.conf", sizeof(config_file) - strlen(config_file) - 1);

    // 如果配置文件不存在，则创建
    fp = fopen(config_file, "r");
    if (fp == NULL)
    {
        fp = fopen(config_file, "w+");
        if (fp == NULL)
            return false;

        // 写入配置
        fprintf(fp, "[listFile] /home/tuto/github/gglibc/list\n");
        fprintf(fp, "[listCurl] https://mirrors.tuna.tsinghua.edu.cn/ubuntu/\n");
        fprintf(fp, "[debugDir] /home/tuto/pwn/glibc/glibc-dbg/\n");

        fseek(fp, 0l, SEEK_SET);
    }

    // 读取配置
    while (!feof(fp))
    {
        char line[1024] = {0};
        char key[1024] = {0};
        char value[1024] = {0};

        if (fgets(line, sizeof(line), fp) == NULL)
        {
            if (feof(fp))
                break;
            ERROR("读取配置文件时出错: %s\n", config_file);
            fclose(fp);
            return false;
        }

        if (sscanf(line, "[%1023[^]]] %1023s\n", key, value) != 2)
        {
            ERROR("解析配置文件时出错: %s\n", line);
            continue;
        }

        if (strcmp(key, "listFile") == 0)
        {
            strncpy(conf->listFile, value, sizeof(conf->listFile) - 1);
            conf->listFile[sizeof(conf->listFile) - 1] = '\0';
        }
        else if (strcmp(key, "listCurl") == 0)
        {
            strncpy(conf->listCurl, value, sizeof(conf->listCurl) - 1);
            conf->listCurl[sizeof(conf->listCurl) - 1] = '\0';
            if (conf->listCurl[strlen(conf->listCurl) - 1] != '/')
            {
                strncat(conf->listCurl, "/", sizeof(conf->listCurl) - strlen(conf->listCurl) - 1);
            }
        }
        else if (strcmp(key, "debugDir") == 0)
        {
            strncpy(conf->debugDir, value, sizeof(conf->debugDir) - 1);
            conf->debugDir[sizeof(conf->debugDir) - 1] = '\0';
            if (conf->debugDir[strlen(conf->debugDir) - 1] != '/')
            {
                strncat(conf->debugDir, "/", sizeof(conf->debugDir) - strlen(conf->debugDir) - 1);
            }
        }
    }

    fclose(fp);
    return true;
}
