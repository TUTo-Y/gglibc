#include "gz.h"

/**
 * \brief 解压缩一个gz文件
 * \param in   输入数据
 * \param out  输出数据
 * \return 成功返回true，失败返回false
 */
bool gz(const mem *in, mem **out)
{
    z_stream strm;

    // 预分配输出缓冲区
    *out = Malloc(in->size);

    // 初始化zlib
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    strm.avail_in = (uInt)(in->size);      // 剩余未解压的数据大小
    strm.next_in = (Bytef *)(in->m);       // 未解压的数据
    strm.avail_out = (uInt)((*out)->size); // 解压的数据块大小
    strm.next_out = (Bytef *)((*out)->m);  // 解压的数据块

    // 初始化解压缩
    if (inflateInit2(&strm, 16 + MAX_WBITS) != Z_OK)
    {
        ERR("初始化zlib时失败");
        Free(*out);
        return false;
    }

    // 解压
    while (strm.avail_in)
    {
        // 输出缓冲区不够大
        if (strm.total_out >= (*out)->size)
        {
            Realloc(*out, (*out)->size + MEM_ADD_SIZE);
        }

        strm.avail_out = (*out)->size - strm.total_out;
        strm.next_out = (Bytef *)((*out)->m + strm.total_out);

        // 解压缩
        int ret = inflate(&strm, Z_NO_FLUSH);
        printf(BBLU("\r正在解压 : %5.2f%%"), (1.0 - (double)strm.avail_in / (double)in->size) * 100);
        fflush(stdout);

        if (ret == Z_STREAM_END)
            break;
        if (ret != Z_OK)
        {
            ERR("解压时出现错误 : %s", strm.msg);
            inflateEnd(&strm);
            Free(*out);
            return false;
        }
    }
    (*out)->size = strm.total_out;

    printf(BGRN("\r解压完成              \n"));

    inflateEnd(&strm);
    return true;
}