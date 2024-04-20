#include <file.h>

/**
 * \brief 读取数据回调函数
 * \param data 数据
 * \param size 大小
 * \param n    数量
 * \param m    mem
 * \return 读取的大小
 */
size_t readcallback(void *data, size_t size, size_t n, void *m)
{
    struct mem *memory = (struct mem *)m;
    size_t realsize = size * n;
    // 重新申请内存
    memory->data = (char *)realloc(memory->data, memory->size + realsize);
    if (memory->data == NULL)
    {
        fprintf(stderr, BRED("\n致命错误：重新申请 0x%lx 字节内存时，内存分配失败!\n"), memory->size + realsize);
        exit(-1);
    }
    // 将数据拷贝到内存中
    memcpy(((char *)memory->data) + memory->size, data, realsize);
    // 设置新的内存大小
    memory->size += realsize;

    return realsize;
}

/**
 * /brief 进度条回调函数
 * /param msg   消息
 * /param total 总大小
 * /param now   当前大小
 * /param msg_color 消息颜色
 * /param bar_color 进度条颜色
 * /return 无
 */
void processcallback(const char *msg, size_t total, size_t now, const char *msg_color, const char *bar_color)
{
    int part1, part2;

    part1 = (int)round((((double)now) / ((double)total)) * PROCESS_N);
    part2 = PROCESS_N - part1;

    printf("\r%s%s%s[", msg_color, msg, bar_color); // 输出消息
    while (part1--)
        putchar('#'); // 输出进度条已完成部分
    while (part2--)
        putchar(' ');     // 输出进度条未完成部分
    printf("]%s", RESET); // 重置颜色

    fflush(stdout); // 刷新输出
}

size_t dprocesscallback(void *p, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow)
{
    if (dltotal)
        processcallback("正在下载 : ", (size_t)dltotal, (size_t)dlnow, _BMAG, _BYEL);
    return 0;
}

/**
 * \brief 解压缩一个gz文件
 * \param min   输入数据
 * \param mout  输出数据
 * \return 成功返回true，失败返回false
*/
bool gz(const struct mem *min, struct mem *mout)
{
    z_stream strm;

    // 分配初始输出缓冲区
    mout->size = min->size;
    mout->data = (char *)MALLOC(mout->size);

    // 初始化zlib
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    strm.avail_in = (uInt)min->size;     // 剩余未解压的数据大小
    strm.next_in = (Bytef *)min->data;   // 未解压的数据
    strm.avail_out = (uInt)mout->size;   // 解压的数据块大小
    strm.next_out = (Bytef *)mout->data; // 解压的数据块

    // 初始化解压缩
    if (inflateInit2(&strm, 32 + MAX_WBITS) != Z_OK)
    {
        fprintf(stderr, BRED("初始化zlib时失败"));
        return false;
    }

    while (strm.avail_in)
    {
        // 如果输出缓冲区不够大，就扩大它
        if (strm.total_out >= mout->size)
        {
            mout->size += MEM_ADD_SIZE;
            mout->data = (BYTE *)realloc(mout->data, mout->size);
            if (mout->data == NULL)
            {
                fprintf(stderr, BRED("解压时出现错误 : 申请 0x%lx 字节内存时，内存分配失败!\n"), mout->size);
                inflateEnd(&strm);
                return false;
            }
        }

        strm.avail_out = mout->size - strm.total_out;
        strm.next_out = (Bytef *)(mout->data + strm.total_out);

        // 解压缩
        int ret = inflate(&strm, Z_NO_FLUSH);
        processcallback("正在解压 : ", (size_t)min->size, (size_t)(min->size - strm.avail_in), _BMAG, _BYEL);

        if (ret == Z_STREAM_END)
            break;
        if (ret != Z_OK)
        {
            fprintf(stderr, BRED("解压时出现错误 : %s\n"), zError(ret));
            inflateEnd(&strm);
            FREE(*mout);
            return false;
        }
    }
    mout->size = strm.total_out;
    processcallback("解压完成 : ", 1, 1, _BGRN, _GRN);
    putchar('\n');

    // 清理并返回
    inflateEnd(&strm);
    return true;
}

/**
 * \brief 通过CURL获取文件数据
 * \param url  文件URL
 * \param data 文件数据
 * \return 成功返回true，失败返回false
*/
bool getdatafromcurl(const char *url, struct mem *data)
{
    CURL *curl = NULL; // URL信息
    CURLcode res;

    // 检测参数不能为空
    if (!(url && data))
        return false;

    // 初始化内存空间
    data->data = MALLOC(0);
    data->size = 0;

    // 初始化curl
    if (!(curl = curl_easy_init()))
    {
        fprintf(stderr, BRED("初始化CURL时出现错误!\n"));
        FREE(*data);
        return false;
    }

    // 设置URL
    res = curl_easy_setopt(curl, CURLOPT_URL, url);
    if (res != CURLE_OK)
    {
        fprintf(stderr, BRED("设置CURL时出现错误: %s\n"),
                curl_easy_strerror(res));
        curl_easy_cleanup(curl);
        FREE(*data);
        return false;
    }
    // 设置回调函数
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, readcallback);
    // 设置调用的指针
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)data);
    // 设置User-Agent
    curl_easy_setopt(curl, CURLOPT_USERAGENT, GGLIBC_VERSION);
    // 设置进度条
    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
    // 设置进度条回调函数
    curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, dprocesscallback);
    // 设置进度条调用指针
    curl_easy_setopt(curl, CURLOPT_XFERINFODATA, &curl);
    if (curl_easy_perform(curl) != CURLE_OK)
    {
        fprintf(stderr, BRED("\n下载%s错误!\n"), url);
        // 清除curl
        curl_easy_cleanup(curl);
        FREE(*data);
        return false;
    }

    // 打印进度条
    processcallback("下载完成 : ", 1, 1, _BGRN, _GRN);
    putchar('\n');

    // 清除curl
    curl_easy_cleanup(curl);
    return true;
}

/**
 * \brief 从归档文件中获取所有目录和文件
 * \param data 归档文件数据
 * \param dir  目录
 * \param file 文件
 * \return 成功返回目录和文件的数量，失败返回-1
 */
int getpathfromarchive(const struct mem *data, struct string **dir, struct string **file)
{
    size_t count = 0;
    struct archive *a = NULL;
    struct archive_entry *entry = NULL;

    // 初始化
    a = archive_read_new();
    if (!a)
    {
        fprintf(stderr, BRED("初始化归档文件时出现错误：%s\n"), archive_error_string(a));
        return -1;
    }
    archive_read_support_filter_all(a);
    archive_read_support_format_all(a);

    // 检查data和data->data是否为NULL
    if (!data || !data->data)
    {
        fprintf(stderr, BRED("获取文件目录时传入参数为NULL\n"));
        archive_read_free(a);
        return -1;
    }

    // 打开归档文件
    if (archive_read_open_memory(a, data->data, data->size) != ARCHIVE_OK)
    {
        fprintf(stderr, BRED("打开归档文件时出现错误：%s\n"), archive_error_string(a));
        archive_read_free(a);
        return -1;
    }

    // 读取归档文件
    int r;
    while ((r = archive_read_next_header(a, &entry)) == ARCHIVE_OK || r == ARCHIVE_WARN || r == ARCHIVE_RETRY)
    {
        if (r == ARCHIVE_WARN || r == ARCHIVE_RETRY)
        {
            fprintf(stderr, BRED("读取归档文件时出现警告或需要重试：%s\n"), archive_error_string(a));
            continue;
        }

        // 获取文件或目录名
        char *pathname = (char *)archive_entry_pathname(entry);
        if (!pathname)
            continue;
        if (archive_entry_filetype(entry) == AE_IFDIR)
            strAdd(dir, pathname, true);
        else
            strAdd(file, pathname, true);
        count++;
    }

    // 关闭归档文件
    archive_read_close(a);
    archive_read_free(a);

    return count;
}

/**
 * \brief 从归档文件中根据文件名获取数据
 * \param data 归档文件数据
 * \param file 文件名
 * \param head 数据链表
 * \return 成功返回true，失败返回false
*/
bool getdatafromarchive(struct mem *data, struct string *file, struct list **head)
{
    size_t count = 0;
    struct archive *a = NULL;
    struct archive_entry *entry = NULL;

    // 初始化
    a = archive_read_new();
    if (!a)
    {
        fprintf(stderr, BRED("初始化归档文件时出现错误：%s\n"), archive_error_string(a));
        return false;
    }
    archive_read_support_filter_all(a);
    archive_read_support_format_all(a);

    // 检查data和data->data是否为NULL
    if (!data || !data->data)
    {
        fprintf(stderr, BRED("获取文件数据时传入参数为NULL\n"));
        archive_read_free(a);
        return false;
    }

    // 打开归档文件
    if (archive_read_open_memory(a, data->data, data->size) != ARCHIVE_OK)
    {
        fprintf(stderr, BRED("打开归档文件时出现错误：%s\n"), archive_error_string(a));
        archive_read_free(a);
        return false;
    }

    // 读取归档文件
    int r;
    while ((r = archive_read_next_header(a, &entry)) == ARCHIVE_OK || r == ARCHIVE_WARN || r == ARCHIVE_RETRY)
    {
        if (r == ARCHIVE_WARN || r == ARCHIVE_RETRY)
        {
            fprintf(stderr, BRED("读取归档文件时出现警告或需要重试：%s\n"), archive_error_string(a));
            continue;
        }

        // 获取文件或目录名
        char *pathname = (char *)archive_entry_pathname(entry);
        if ((!pathname) && (archive_entry_filetype(entry) != AE_IFMT))
            continue;

        // 检查这个文件是否时自己要提取的文件
        if (strCheckAndGet(file, pathname) == true)
        {
            struct mem *mem = (struct mem *)MALLOC(sizeof(struct mem));
            mem->size = archive_entry_size(entry);
            mem->data = (BYTE *)MALLOC(mem->size);
            archive_read_data(a, mem->data, mem->size);
            dataAdd(head, mem, false);
            count++;
        }
    }

    // 关闭归档文件
    archive_read_close(a);
    archive_read_free(a);

    // 错误处理
    if (strCount(file) != count)
    {
        fprintf(stderr, BRED("提取文件数量与文件名数量不一致!\n"));
        fprintf(stderr, BRED("请检查需要提取的文件是否存在问题\n"));

        struct string *node = file;
        while (node)
        {
            fprintf(stderr, BRED("文件名: %s\n"), node->str);
            node = node->next;
        }
        return false;
    }
    return true;
}