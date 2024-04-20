#ifndef _FILE_H
#define _FILE_H

#include <zlib.h>
#include <data.h>
#include <math.h>
#include <color.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <archive.h>
#include <curl/curl.h>
#include <archive_entry.h>


#define GGLIBC_VERSION  "gglibc/2.0"
#define GLIBC_LS        "https://mirror.tuna.tsinghua.edu.cn/ubuntu/ls-lR.gz"
#define GLIBC_DIR       "https://mirror.tuna.tsinghua.edu.cn/ubuntu/pool/main/g/glibc/"

#define PROCESS_N       50              // 进度条长度
#define MEM_INIT_SIZE   4096            // 内存初始化大小为4KB
#define MEM_ADD_SIZE    MEM_INIT_SIZE   // 内存增加大小

/**
 * \brief 解压缩一个gz文件
 * \param min   输入数据
 * \param mout  输出数据
 * \return 成功返回true，失败返回false
*/
bool gz(const struct mem *min, struct mem *mout);

/**
 * \brief 通过CURL获取文件数据
 * \param url  文件URL
 * \param data 文件数据
 * \return 成功返回true，失败返回false
*/
bool getdatafromcurl(const char *url, struct mem *data);

/**
 * \brief 从归档文件中获取所有目录和文件
 * \param data 归档文件数据
 * \param dir  目录
 * \param file 文件
 * \return 成功返回目录和文件的数量，失败返回-1
*/
int getpathfromarchive(const struct mem *data, struct string **dir, struct string **file);

/**
 * \brief 从归档文件中根据文件名获取数据
 * \param data 归档文件数据
 * \param file 文件名
 * \param head 数据链表
 * \return 成功返回true，失败返回false
*/
bool getdatafromarchive(struct mem *data, struct string *file, struct list **head);

#endif // _FILE_H