#include "test_str_func.h"
#include <strings.h>
#include <string.h>
#include "../global_def.h"
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

size_t m_strlcpy(char* dst, const char* src, unsigned int count) {
    size_t src_len = strlen(src);

    if (src_len) {
        size_t copy_len = src_len < count? src_len:count-1;
        memcpy(dst, src, copy_len);
        dst[copy_len] = '\0';
    }

    return src_len;
}

// 不考虑重合;
char* m_strcat(char* dst, const char* src) {
    char* tmp = dst;

    while (*tmp) tmp++;  // while(*tmp++)?

    while((*tmp++ = *src++)!= '\0');

    // while(*tmp++ = *src++);

    return dst;
}

char* m_strncat1(char* dst, const char* src, size_t count) {
    char* tmp_dst = dst;

    if (count) {
        while(*tmp_dst) tmp_dst++;

        while (count) {
            if (*tmp_dst = *src) {
                src++;
            } 
            tmp_dst++;
            count--;
        }
    }

    return dst;
}

char* m_strncat2(char* dst, const char* src, size_t count) {
    char* tmp_dst = dst;

    if (count) {
        while(*tmp_dst) tmp_dst++;

        while (*tmp_dst++ = *src++) {
            if (!--count) {
                *tmp_dst = '\0';
                break;
            }
        }
    }

    return dst;
}

size_t m_strlcat(char* dst, const char* src, size_t count) {
    size_t dst_len = strlen(dst);
    size_t src_len = strlen(src);
    size_t target_len = dst_len + src_len;

    if (dst_len > count) throw;

    size_t cpy_len = target_len > count ? count - dst_len -1: src_len;

    char* dst_tail = dst + dst_len;

    memcpy(dst_tail, src, cpy_len);

    *(dst_tail + cpy_len) = '\0';

    return dst_len + cpy_len;
}

void* m_memcpy(void* dst, const void* src, size_t count) {
    char* c_dst = (char *)dst;
    const char* c_src = (const char*)src;

    while (count--) {
        *c_dst++ = *c_src++;
    }    
}

// 保证 dst 的完整性;
void* m_memmove(void* dst, const void* src, size_t count) {
    char* c_dst = (char *)dst;
    const char* c_src = (const char*)src;

    if (dst < src) {

        while(count--) {
            *c_dst++ = *c_src++;
        }
    } else if (src < dst) {
        c_dst += count;
        c_src += count;

        while (count--) {
            *c_dst-- = *c_src--;
        }
    }
    return dst;
}

int m_strcmp(const char* ca, const char* cb) {
    unsigned char a, b;

    while (true) {
        a = *ca++;
        b = *cb++;

        if (a!=b) { 
            return a < b? -1:1;
        }

        if (!a) break;
    } 
    return 0;
}

size_t m_strlen1(const char* s) {
    size_t res = 0;
    while(*s++) res++;
    return res;
}

size_t m_strlen2(const char* s) {
    const char* sc = s;
    while (*sc != '\0') sc++;

    return sc - s;
}


void TestCmp() {

    void* t = nullptr;


}

void TestCpy() {

}


void TestCat() {
    char* a = "abcdefg";

    char a1[20] = {"ha"};
    char a2[20] = {"hb"};

    // cout << "a1: " << m_strcat(a1, a) << endl;
    cout << "a1: " << m_strncat1(a1, a, 2) << endl;
    cout << "a2: " << m_strncat2(a2, a, 3) << endl;

    cout << "a3: " << m_strlcat(a2, a1, 8) << ", " << a2 << endl;

    // cout << "a3: " << strlcat(a2, a1, 8) << ", " << a2 << endl;
}

void TestChar() {
    char* a = "abcdefg";

    char* a1 = a;
    char* a2 = a;

    cout << "*a1++: " << endl;
    while(*a1++) cout << *a1 << endl;

    cout << "\n*a2++" << endl;
    while(*a2) { 
        a2++;
        cout << *a2 << endl;
    }
}


void TestMem() {
    const char* a = "abcdefg";

    char a1[20] = {"ha"};
    // char a2[20] = {"hb"};   

    cout << "a1: " << m_memcpy(a1, a, 2) << endl; // 不会 加0, 直接拷贝字符串 很危险;   
}

void TestSearch() {
    const char* a = "abcdefg";
    const char* b = "ce";

    cout << "res: " << strpbrk(a, b) << endl;
}

void TestStrFuncMain() {
    // TestChar();
    
    // TestCat();

    // TestMem();

    TestSearch();
}