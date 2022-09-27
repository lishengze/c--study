#include "test_str_func.h"
#include <strings.h>

/*
Data: 2022.09.27, 10:30;
Author: lsz;
Target: Test Basic String Func
Info:  
strcpy(p, p1) 复制字符串
strncpy(p, p1, n) 复制指定长度字符串
strcat(p, p1) 附加字符串
strncat(p, p1, n) 附加指定长度字符串
strlen(p) 取字符串长度
strcmp(p, p1) 比较字符串
strcasecmp 忽略大小写比较字符串
strncmp(p, p1, n) 比较指定长度字符串
strchr(p, c) 在字符串中查找指定字符
strrchr(p, c) 在字符串中反向查找
strstr(p, p1) 查找字符串
strpbrk(p, p1) 以目标字符串的所有字符作为集合，在当前字符串查找该集合的任一元素
strspn(p, p1) 以目标字符串的所有字符作为集合，在当前字符串查找不属于该集合的任一元素的偏移
strcspn(p, p1) 以目标字符串的所有字符作为集合，在当前字符串查找属于该集合的任一元素的偏移 
*/


char* m_strcpy(char* dst, const char* src) {
    char* tmp = dst;
    while((*tmp++ = *src++) != '\0') 
    return tmp;
}

char* m_strncpy(char* dst, const char* src, unsigned int count) {
    char* tmp = dst;
    
    while(count) {
        if ((*tmp = *src) != 0)
            src++;
        tmp++;
        count--;
    }
    return tmp;
}

void TestCmp() {

    void* t = nullptr;


}

void TestCpy() {

}

void TestStrFuncMain() {

}