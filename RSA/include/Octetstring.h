//
// Created by 胡鹏飞 on 2020/10/21.
//

#ifndef OCTETSTRING_H
#define OCTETSTRING_H

// 字节串的定义
typedef struct Octetstring{
    int *os;
    int len;
}OS;

// 初始化
void OSinit(OS *str);

// 通过 int 类型加入到字节组中
void add_int(OS *str, int t);

// 通过 char 类型加入到字节组中
void add_char(OS *str, char c);

// 反转
void OS_inverse(OS *str);

// 释放空间
void OS_free(OS *str);
#endif
