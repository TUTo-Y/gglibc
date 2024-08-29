/**
 * \brief 从网络获取数据
 */
#ifndef WEB_H
#define WEB_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include <curl/curl.h>

#include "config.h"
#include "mem.h"

/**
 * \brief 通过curl获取数据
 * \param url 网络地址
 * \param data 数据
 */
bool webGet(const char *url, mem **data);

#endif // WEB_H