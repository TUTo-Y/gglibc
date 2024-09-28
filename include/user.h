#ifndef USER_H
#define USER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <linux/limits.h>

#include "config.h"

// 配置信息
typedef struct
{
    char listFile[PATH_MAX];
    char listCurl[PATH_MAX];
    char debugDir[PATH_MAX];
}config;

// 初始化配置
bool confInit(config *conf);

#endif // USER_H