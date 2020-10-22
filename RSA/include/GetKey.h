//
// Created by 胡鹏飞 on 2020/10/21.
//
#ifndef GETKEY_H
#define GETKEY_H
#include "gmpp.h"

gmp_randstate_t state; // 产生随机数用

// 获取大素数 p 和 q
void getPQ(mpz_t t, int n);

// 判断 p 和 q 是否符和标准
int checkPQ(mpz_t p, mpz_t q, int k);

// 判断 N 的位数是否为 k
int checkN(mpz_t N, int k);

// 获取密钥
void getKey(int k, mpz_t n, mpz_t d, mpz_t e);

#endif