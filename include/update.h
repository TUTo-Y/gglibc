#ifndef _UPDATE_H
#define _UPDATE_H

#include <file.h>

extern struct string *ls_libc;
extern struct string *ls_libc_dbg;

/**
 * \brief 写入文件信息到list
 * \return 是否成功
*/
bool putscontenttolist();

/**
 * \brief 从list读取文件信息
 * \return 是否成功
*/
bool getcontentfromlist();

/**
 * \brief 从ls中获取文件信息
 * \param str 字符串
 * \return 是否成功
*/
bool getcontenttolsfile(const char *str);

/**
 * \brief 更新数据
 * \return 是否成功
*/
bool update();

#endif  // _UPDATE_H