#include "file.h"

// 写入list
void writetolist(FILE*file, const char *str)
{
    char *start;
    char *end;
    char *glibc = NULL;
    char *p = NULL;
    char *q = NULL;
    size_t size = 0;
    int n = 0;
    
    // 定位glibc区间
    start = strstr(str, "./pool/main/g/glibc:");
    start = strstr(start, "libc6_");
    end   = strchr(start, '/');

    // 拷贝区域内的数据
    size = end - start;
    p = glibc = (char*)malloc(size + 1);
    strncpy(glibc, start, size);

    while(p = strstr(p, "libc6"))
    {
        q = strstr(p, ".deb");
        if((!strncmp(q - 5, "amd64", 5)) || (!strncmp(q - 4, "i386", 4)))
        {
            fprintf(file, "[%3d ] ", n++);
            fwrite(p, 1, q - p, file);
            fputc('\n', file);
        }
        p++;
    }

    free(glibc);
}


int main()
{
    mem ls_data = { 0 };
    mem ls = { 0 };

    
    Init();
    // 获取ls_lR.gz文件
    printf("正在获取文件:%s\n", LS);
    if(getdatafromcurl(LS, &ls_data))
    {
        perror("获取文件失败!\n");
        return -1;
    }
    printf("成功获取文件:%s\n", LS);

    // 解压ls_lR.gz文件
    printf("正在解压文件列表\n");
    if(gz(ls_data.data, ls_data.size, (char**)&ls.data, &ls.size))
    {
        perror("解压文件失败!\n");
        return -1;
    }
    printf("成功解压文件列表\n");
    FREE(ls_data);

    // 解析出需要的数据放入list中
    printf("正在写入list\n");
    FILE *fp = fopen("list.txt", "w");
    if(!fp)
    {
        perror("打开文件失败!\n");
        return -1;
    }
    writetolist(fp, (const char*)ls.data);
    FREE(ls);
    fclose(fp);

    printf("list更新成功\n");
    Quit();
    
    return 0;
}