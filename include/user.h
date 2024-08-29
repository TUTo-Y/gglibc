#ifndef USER_H
#define USER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
// 配置信息
typedef struct
{
    char listFile[1024];
    char listCurl[1024];
}config;

// 初始化配置
bool confInit(config *conf);

#endif // USER_H