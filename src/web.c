#include "web.h"

// 读取数据回调函数
static size_t readcallback(void *data, size_t size, size_t n, void *m)
{
    size_t realsize = size * n;
    // 重新申请内存
    Realloc((mem *)m, ((mem *)m)->size + realsize);
    // 将数据拷贝到内存中
    memcpy(((mem *)m)->m + ((mem *)m)->size - realsize, data, realsize);
    return realsize;
}

// 进度条回调函数
static size_t processcallback(void *p, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow)
{
    if (dltotal)
    {
        printf(BBLU("\r正在下载 : %5.2f%%"), (double)dlnow / (double)dltotal * 100);
        fflush(stdout);
    }
    return 0;
}

/**
 * \brief 通过curl获取数据
 * \param url 网络地址
 * \param data 数据
 */
bool webGet(const char *url, mem **data)
{
    CURL *curl = NULL; // URL信息
    CURLcode res;

    if (url == NULL || data == NULL)
        return false;

    // 初始化curl
    if (!(curl = curl_easy_init()))
    {
        ERR("初始化CURL时出现错误");
        return false;
    }

    // 初始化内存空间
    *data = Malloc(0);

    // 设置URL
    res = curl_easy_setopt(curl, CURLOPT_URL, url);
    if (res != CURLE_OK)
    {
        fprintf(stderr, BRED("设置CURL时出现错误: %s\n"),
                curl_easy_strerror(res));
        curl_easy_cleanup(curl);
        Free(*data);
        return false;
    }
    // 设置回调函数
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, readcallback);
    // 设置调用的指针
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)*data);
    // 设置User-Agent
    curl_easy_setopt(curl, CURLOPT_USERAGENT, GGLIBC_VERSION);
    // 设置进度条
    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
    // 设置进度条回调函数
    curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, processcallback);
    // 设置进度条调用指针
    curl_easy_setopt(curl, CURLOPT_XFERINFODATA, &curl);
    // 设置超时时间
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);
    // 设置最大重定向次数
    curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 5L);
    // 设置是否跟随重定向
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    
    fflush(stdout);
    if (curl_easy_perform(curl) != CURLE_OK)
    {
        ERR("\n下载%s错误\n", url);
        fflush(stdout);

        // 清除curl
        curl_easy_cleanup(curl);
        Free(*data);
        return false;
    }

    // 清除curl
    curl_easy_cleanup(curl);

    printf(BGRN("\r下载%s完成\n"), url);
    fflush(stdout);
    return true;
}