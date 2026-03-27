#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#define LOG(fmt,...) printf("[%s:%d]" fmt "\n",__FILE__, __LINE__,##__VA_ARGS__);//##是在参数为空时，将前面的逗号去掉

void printnum(int n, ...)
{
    va_list al;
    va_start(al,n);
    for(int i=0;i<n;i++)
    {
        int num = va_arg(al,int);
        printf("pargm[%d]:%d\n",i,num);
    }
    va_end(al);
}

void myprint(const char *fmt,...)
{
    char * res;
    va_list ap;//可变参数的指针
    va_start(ap,fmt);//初始化指针，使指针指向fmt后的第一个参数
    int len = vasprintf(&res,fmt,ap);//将格式串中的占位符依次替换对应参数
    
    if(len != -1)
    {
        printf("%s\n",res);
        free(res);// 释放 vasprintf 内部 malloc 的内存
    }
    va_end(ap);
}

int main()
{
    LOG("紫洋葱popo");//本身就是格式化字符串，不需要占位符
    LOG("%s 是 %d 出生的","ziyangcon",2006);
    //这个语句在预处理阶段被展开为“printf("[%s:%d]" "%s 是 %d 出生的" "\n",__FILE__, __LINE__,"ziyangcon",2006)”

    printnum(3,3,6,9);
    myprint("%s未来资产:%d","张轶臣",90000000);

    return 0;
}
