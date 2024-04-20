#include <file.h>
#include <update.h>
#include <get.h>

void ch_get(struct string *hand)
{
    int choice = 0;
    struct string *node = hand;
    // enum
    printf(BWHT("[  0 ] 返回\n"));
    for (int i = 1; node; i++)
    {
        printf(BWHT("[%3d ] %s\n"), i, node->str);
        node = node->next;
    }

    // 选择
    printf(BWHT("请选择: "));
    scanf("%d", &choice);
    if (choice == 0)
        return;

    // 下载对应的文件
    node = hand;
    for (int i = 1; i < choice; i++)
        node = node->next;
    if (getlibc(node->str) == false)
        printf(BRED("下载失败\n"));
    else
        printf(BGRN("下载成功\n"));
}

int main(int argc, char *argv[], char *envp[])
{
    int choice = 0;
    // 检查更新情况
    if (argc == 2 && (!strcmp(argv[1], "update") || !strcmp(argv[1], "u") || !strcmp(argv[1], "-u")))
    {
        update();
        return 0;
    }

    // 读取list
    if (getcontentfromlist() == false)
        return -1;

    while (1)
    {
        // 打印菜单
        printf(BCYN("0. 退出\n"));
        printf(BCYN("1. 获取libc\n"));
        printf(BCYN("2. 获取libc-dbg\n"));
        printf(BWHT("请选择: "));
        scanf("%d", &choice);
        switch (choice)
        {
        case 0:
            return -1;
        case 1:
            ch_get(ls_libc);
            break;
        case 2:
            ch_get(ls_libc_dbg);
            break;
        default:
        return 0;
        }
    }

    return 0;
}