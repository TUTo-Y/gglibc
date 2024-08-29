/**
 * 更新列表
 */
#ifndef UPDATE_H
#define UPDATE_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "gz.h"
#include "web.h"
#include "list.h"
#include "user.h"
#include "config.h"

/**
 * \brief 解析ls-lR文件
 * \param str ls-lR文件内容
 * \param libc libc列表
 * \param libc_dbg libc符号列表
 * \return 成功返回true，失败返回false
 */
bool parse(const char *str, list *libc, list *libc_dbg);

/**
 * \brief 获取list从URL
 * \param url 源网址地址
 * \param libc libc列表
 * \param libc_dbg libc符号列表
 * \return 成功返回true，失败返回false
 */
bool list_from_url(const char *url, list *libc, list *libc_dbg);

/**
 * \brief 获取list从文件
 * \param file 文件名
 * \param libc libc列表
 * \param libc_dbg libc符号列表
 * \return 成功返回true，失败返回false
 */
bool list_from_file(const char *file, list *libc, list *libc_dbg);

/**
 * \brief 写入list到文件
 * \param file 文件名
 * \param libc libc列表
 * \param libc_dbg libc符号列表
 * \return 成功返回true，失败返回false
 */
bool list_to_file(const char *file, list *libc, list *libc_dbg);

/**
 * \brief 更新列表
 * \param conf 配置
 * \param libc libc列表
 * \param libc_sym libc符号列表
 * \return 成功返回true，失败返回false
 */
bool list_update(const config *conf, list *libc, list *libc_sym);

#endif // UPDATE_H