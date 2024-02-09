#ifndef _FILE_H
#define _FILE_H

#include <zlib.h>
#include <math.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <archive.h>
#include <curl/curl.h>
#include <archive_entry.h>


#define GGLIBC_VERSION  "gglibc/1.0"
#define LS              "https://mirror.tuna.tsinghua.edu.cn/ubuntu/ls-lR.gz"
#define GLIBC_DIR       "https://mirror.tuna.tsinghua.edu.cn/ubuntu/pool/main/g/glibc/"

#define PROCESS_N       50
#define MEM_INIT_SIZE   4096
#define MEM_ADD_SIZE    MEM_INIT_SIZE
#define FREE(x)         (free((x).data), (x).data = NULL, (x).size = 0)


/* 内存结构 */
typedef struct
{
    void    *data;
    size_t  size;
}mem;

/* list */
typedef struct _list list;
struct _list
{
    int id;
    char name[256];
    list *next;
};


/* 初始化 */
void Init();
/* 清除 */
void Quit();

/**
 * \brief 通过URL获取文件数据
 * \param url  文件URL
 * \param data 文件数据
 * \return 成功返回0，失败返回-1
*/
int getdatafromcurl(const char *url, mem *data);

/**
 * \brief 解压缩一个gz文件
 * \param inData  输入数据
 * \param inLen   输入数据长度
 * \param outData 输出数据
 * \param outLen  输出数据长度
 * \return 成功返回0，失败返回-1
*/
int gz(const char *inData, const size_t inLen, char **outData, size_t *outLen);

/**
 * \brief 从归档文件中获取文件数据
 * \param filename 条目名
 * \param inData   输入数据
 * \param inLen    输入数据长度
 * \param outData  输出数据
 * \param outLen   输出数据长度
 * \return 成功返回0，失败返回-1
*/
int getdatafromtar(const char *filename, const char *inData, const size_t inLen, char **outData, size_t *outLen);

/**
 * \brief 从归档文件中获取条目名
 * \param inData   输入数据
 * \param inLen    输入数据长度
 * \param file     存放文件名
 * \param dir      存放目录名
 * \return 成功返回0，失败返回-1
*/
int getnamefromtar(const char *inData, const size_t inLen, mem *file, mem *dir);

/**
 * \brief 读取list文件到list
*/
void readlist(list **l, FILE *file);

/**
 * \brief 释放list
*/
void freelist(list **l);

/**
 * \brief 获取tar某个目录下的全部文件
 * \param post 条目的目录
 * \param dir 保存的目录
 * \param inData 输入的数据
 * \param inLen 输入的数据长度
*/
int getfilefromtar(const char *post, const char *dir, const char *inData, const size_t inLen);

/**
 * \brief 通过文件名下载文件到对应目录
 * \param name   要下载的文件
 * \return 成功返回0，失败返回-1
*/
int getfile(const char *name);

#endif  // _FILE_H