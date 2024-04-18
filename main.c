#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main()
{
    char str[10] = { 0 };
    printf(">");
    scanf("%s", str);
    printf("num = %d", atoi(str));
    return 0;
}