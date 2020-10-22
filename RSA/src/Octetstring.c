//
// Created by 胡鹏飞 on 2020/10/21.
//

#include <stdlib.h>
#include <stdio.h>
#include "Octetstring.h"

void swap(int *a, int *b){
    int t = *a;
    *a = *b;
    *b = t;
}

// 初始化
void OSinit(OS *str){
    str->len = 0;
    str->os = (int *) malloc(130 * sizeof(int));
}

// 通过 int 类型加入到字节组中
void add_int(OS *str, int t){
    str->os[str->len] = t;
    str->len ++;
}

// 通过 char 类型加入到字节组中
void add_char(OS *str, char c){
    str->os[str->len] = (int)c;
    str->len ++;
}

// 反转
void OS_inverse(OS *str){
    for (int i = 0, j = str->len - 1; i < (str->len) / 2; i++, j--){
        swap(&(str->os[i]), &(str->os[j]));
    }
}

// 释放空间
void OS_free(OS *str){
    free(str->os);
    free(str);
}
