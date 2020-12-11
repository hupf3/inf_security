#ifndef DATABASE_H
#define DATABASE_H

#include <stdio.h>
#include <string.h>

// 保存用户ID和密码的数据结构
typedef struct Client{
    char ID[100];
    char key[100];
}Client;

Client c[10];

// 判断数据库中是否有该数据 
// 0 : 不存在   
// 1 : 存在
int isInclude(char ClientID[]){
    for (int i = 0; i < 10; ++i)
        if (strcmp(ClientID, c[i].ID) == 0) return 1;
    return 0;
}

// 获取该用户信息
Client getClient(char ClientID[]){
    for (int i = 0; i < 10; ++i){
        if (strcmp(ClientID, c[i].ID) == 0) return c[i];        
}

#endif