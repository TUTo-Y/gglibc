#include "file.h"

void Init()
{
    // 初始化libcurl
    curl_global_init(CURL_GLOBAL_ALL);
}
void Quit()
{
    // 退出libcurl
    curl_global_cleanup();
}

// 进度条回调函数
void processcallback(const char *msg, size_t total, size_t now)
{
    int part1, part2;
    
    part1 =  (int)round((((double)now)/((double)total)) * PROCESS_N);
    part2 = PROCESS_N - part1;
    
    printf("\r%s[", msg);
    while(part1--)
        putchar('#');
    while(part2--)
        putchar(' ');
    putchar(']');
    fflush(stdout);
}

size_t dprocesscallback(void *p, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow)
{
    if(dltotal)
        processcallback("下载ing : ", (size_t)dltotal, (size_t)dlnow);
    return 0;
}

size_t readcallback(void *data, size_t size, size_t n, void *m)
{
    mem    *memory   = (mem *)m;
    size_t  realsize = size * n;
    // 重新申请内存
    memory->data = (char*)realloc(memory->data, memory->size + realsize + 1);
    // 将数据拷贝到内存中
    memcpy(((char*)memory->data) + memory->size, data, realsize);
    // 设置文件末
    ((char*)memory->data)[memory->size + realsize] = 0;
    // 设置新的内存大小
    memory->size += realsize;

    return realsize;
}

int getdatafromcurl(const char *url, mem *data)
{
    CURL *curl  = NULL; // URL信息

    // 检测参数
    if(!(url && data))
        return -1;

    // 初始化内存空间
    data->data = malloc(0);
    data->size = 0;
    
    // 初始化curl
    curl = curl_easy_init();
    // 设置URL
    curl_easy_setopt(curl, CURLOPT_URL, url);
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
    // 获取文件
    if(curl_easy_perform(curl) != CURLE_OK)
    {
        // 清除curl
        curl_easy_cleanup(curl);
        fprintf(stderr, "\n下载%s错误!\n", url);
        return -1;
    }

    // 清除curl
    curl_easy_cleanup(curl);
    putchar('\n');
    return 0;
}

int gz(const char *inData, const size_t inLen, char **outData, size_t *outLen)
{
    z_stream infstream;

    // 是否提前分配内存
    if(*outData == NULL)
    {
        *outLen = MEM_INIT_SIZE;
        *outData = (char *)malloc(*outLen);
    }

    infstream.zalloc    = Z_NULL;
    infstream.zfree     = Z_NULL;
    infstream.opaque    = Z_NULL;
    infstream.avail_in  = (uInt)inLen;          // 剩余未解压的数据大小
    infstream.next_in   = (Bytef *)inData;      // 未解压的数据
    infstream.avail_out = (uInt)*outLen;        // 解压的数据块大小
    infstream.next_out  = (Bytef *)*outData;    // 解压的数据块

    inflateInit2(&infstream, 16 + MAX_WBITS);

    while (infstream.avail_in)
    {
        // 如果解压的数据块不够大，重新分配内存
        if (infstream.avail_out == 0)
        {
            *outLen += MEM_ADD_SIZE;
            *outData = (char *)realloc(*outData, *outLen);
            infstream.next_out = (Bytef *)(*outData + *outLen - MEM_ADD_SIZE);
            infstream.avail_out = (uInt)MEM_ADD_SIZE;
        }

        // 解压数据
        if(inflate(&infstream, Z_NO_FLUSH) < Z_OK)
        {
            inflateEnd(&infstream);
            return -1;
        }
        processcallback("解压ing : ", inLen, inLen-infstream.avail_in);
    }
    *outLen = infstream.total_out;

    inflateEnd(&infstream);
    putchar('\n');
    return 0;
}

int getdatafromtar(const char *filename, const char *inData, const size_t inLen, char **outData, size_t *outLen)
{
    struct archive *a;
    struct archive_entry *entry;
    size_t offset = 0;
    int len = 0, len_old = 0;
    // 初始化archive
    a = archive_read_new();
    archive_read_support_filter_all(a);
    archive_read_support_format_all(a);

    // 读取数据
    if (archive_read_open_memory(a, inData, inLen) != ARCHIVE_OK)
        return -1;

    // 获取归档文件条目
    while (archive_read_next_header(a, &entry) == ARCHIVE_OK)
    {
        // 匹配成功
        if(!strcmp(filename, archive_entry_pathname(entry)))
        {
            *outLen = MEM_INIT_SIZE;
            *outData = (char *)malloc(*outLen);
            if (*outData == NULL)
                return -1;
            // 解压
            while((len = archive_read_data(a, *outData + *outLen - MEM_ADD_SIZE, MEM_ADD_SIZE)) > 0)
            {
                *outLen += MEM_ADD_SIZE;
                char *new_data = (char *)realloc(*outData, *outLen);
                if (new_data == NULL)
                {
                    free(*outData);
                    return -1;
                }
                *outData = new_data;
                len_old = len;
            }
            if (len < 0)
            {
                free(*outData);
                return -1;
            }
            *outLen -= MEM_ADD_SIZE - len_old;

            break;
        }
        archive_read_data_skip(a);
    }

    // 清除archive
    if (archive_read_free(a) != ARCHIVE_OK)
        return -1;
    return 0;
}

void add_str(const char *str, mem *m, int *n)
{
    // 检测字符串数组是否已满
    if(*n >= m->size)
    {
        m->size += 0x10;
        m->data = realloc(m->data, m->size * sizeof(char *));
    }
    // 添加字符串
    ((char**)m->data)[(*n)++] = strdup(str);
}

int getnamefromtar(const char *inData, const size_t inLen, mem *file, mem *dir)
{
    struct archive *a;
    struct archive_entry *entry;
    int file_name = 0;
    int dir_name = 0;

    // 初始化file和dir数据
    file->data = malloc(0x10 * sizeof(char *));
    file->size = 0x10;
    dir->data  = malloc(0x10 * sizeof(char *));
    dir->size  = 0x10;

    // 初始化archive
    a = archive_read_new();
    archive_read_support_filter_all(a);
    archive_read_support_format_all(a);

    // 读取数据
    if (archive_read_open_memory(a, inData, inLen) != ARCHIVE_OK)
        return -1;

    // 获取归档文件条目
    while (archive_read_next_header(a, &entry) == ARCHIVE_OK)
    {
        // 匹配成功
        const char *name = archive_entry_pathname(entry);
        if(name[strlen(name) - 1] == '/')
            add_str(name, dir, &dir_name);
        else
            add_str(name, file, &file_name);
        //archive_read_data_skip(a);
    }

    dir->size = dir_name;
    file->size = file_name;

    // 清除archive
    if (archive_read_free(a) != ARCHIVE_OK)
        return -1;
    return 0;
    
}

void readlist(list **l, FILE *file)
{
    char ch = fgetc(file);
    if(ch != '[')
    {
        *l = NULL;
        return;
    }
    else
    {
        *l = (list*)malloc(sizeof(list));
        fscanf(file, "%d %c %s\n", &(*l)->id, &ch, (*l)->name);
        readlist(&(*l)->next, file);
    }
}
void freelist(list **l)
{
    while (*l)
    {
        list *tmp = (*l)->next;
        free(*l);
        *l = tmp;
    }
}

int getfilefromtar(const char *post, const char *dir, const char *inData, const size_t inLen)
{
    struct archive *a;
    struct archive_entry *entry;
    size_t offset = 0;

    int len     = 0;
    char *outData = malloc(MEM_INIT_SIZE);

    FILE* file = NULL;
    char filename[256] = { 0 };

    // 初始化archive
    a = archive_read_new();
    archive_read_support_filter_all(a);
    archive_read_support_format_all(a);

    // 读取数据
    if (archive_read_open_memory(a, inData, inLen) != ARCHIVE_OK)
        return -1;

    // 获取归档文件条目
    while (archive_read_next_header(a, &entry) == ARCHIVE_OK)
    {
        const char *name = archive_entry_pathname(entry);
        // 匹配成功
        if( (!strncmp(name, post, strlen(post))) && 
            (name[strlen(name) - 1] != '/'))
        {
            // 创建文件
            strcpy(filename, dir);
            strcat(filename, strchr(strchr(strchr(name, '/') + 1, '/') + 1, '/') + 1);
            file = fopen(filename, "wb");
            if(!file)
            {
                printf("创建文件失败:%s\n", filename);
                break;
            }
            // 获取数据
            while((len = archive_read_data(a, outData, MEM_ADD_SIZE)) > 0)
                fwrite(outData, 1, len, file);
            if (len < 0)
            {
                archive_read_free(a);
                free(outData);
                return -1;
            }
            // 文件为空
            if(ftell(file) == 0)
            {
                fclose(file);
                remove(filename);
            }
            else
            {
                printf("%s\t提取完成!\n", filename);
                fclose(file);
            }
        }
        else
            archive_read_data_skip(a);
    }

    free(outData);
    // 清除archive
    if (archive_read_free(a) != ARCHIVE_OK)
        return -1;

    return 0;
}


/* 通过名称下载文件 */
int getfile(const char *name)
{
    FILE *file = NULL;

    char dir[256]      = { "./lib/" };
    char url[256]      = { 0 };
    char *tar_post = NULL;
    char *deb_post = NULL;

    mem deb = { 0 };
    mem tar = { 0 };
    mem f   = { 0 };
    mem d   = { 0 };

    // 创建目录
    mkdir(dir, 0777);
    strcat(dir, name);
    strcat(dir, "/");
    mkdir(dir, 0777);

    // 获取归档文件中的目录
    if(!strcmp(&name[strlen(name) - 2], "64"))
        tar_post = "./lib/x86_64-linux-gnu/";
    else
        tar_post = "./lib/i386-linux-gnu/";

    // 下载文件deb文件
    strcpy(url, GLIBC_DIR);
    strcat(url, name);
    strcat(url, ".deb");
    printf("准备下载文件:%s\n", url);
    if(getdatafromcurl(url, &deb) < 0)
    {
        perror("获取文件失败!\n");
        return -1;
    }
    printf("成功获取文件\n");

    // 获取deb条目
    getnamefromtar(deb.data, deb.size, &f, &d);
    for(int i = 0; i<f.size; i++)
    {
        if(!strncmp(((char**)f.data)[i], "data", 4))
        {
            deb_post = ((char**)f.data)[i];
            break;
        }
    }
    if(!deb_post)
    {
        printf("无法从%s.deb文件中获取data.tar条目\n", name);
        FREE(deb);
        FREE(f);
        FREE(d);
        return -1;
    }
    FREE(f);
    FREE(d);

    // 从deb中提取data
    printf("正在从%s.deb中提取\n", name);
    if(getdatafromtar(deb_post, (char*)deb.data, deb.size, (char**)&tar.data, &tar.size) < 0)
    {
        perror("提取文件失败!\n错误文件:data.tar.zst\n");
        FREE(deb);
        return -1;
    }

    if(getfilefromtar(tar_post, dir, (char*)tar.data, tar.size) < 0)
    {
        perror("提取错误!\n");
        FREE(deb);
        FREE(tar);
        return -1;
    }

    FREE(deb);
    FREE(tar);
    printf("提取完成!\n");
    return 0;
}