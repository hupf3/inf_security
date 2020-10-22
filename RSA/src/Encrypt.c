//
// Created by 胡鹏飞 on 2020/10/21.
//
#include <stdlib.h>
#include <stdio.h>
#include "Step.h"
#include "Encrypt.h"

int Encrypt(mpz_t n, mpz_t e, char *M, int PSLen, int mLen, int k, OS *str){
    mpz_t m; mpz_init(m); mpz_set_ui(m, 0);

    getEM(str, PSLen, M, mLen);
    printf("# 加密后得到的 EM 字节串为 #：");
    for (int i = 0; i < str->len; i++) printf("%d ", str->os[i]);
    printf("\n\n");
    OS2IP(str, m);
    printf("# 加密后得到的整数消息代表 m 为 #：");
    gmp_printf("%Zd\n", m);
    printf("\n");

    mpz_t tmp; mpz_init(tmp); mpz_set(tmp, n);
    // if (mpz_cmp(m, tmp) >= 0){
    //     printf("消息代表超出范围！\n");
    //     return 0;
    // }

    mpz_t c; mpz_init(c);
    RSAEP(n, e, m, c);
    printf("# 加密后得到的密文代表 c 为 #：");
    gmp_printf("%Zd\n", c);
    printf("\n");
    OS *C; C = (OS *)malloc(sizeof(OS));
    OSinit(C);
    if (I2OSP(c, k, C) == 0){
        OS_free(C);
        mpz_clear(m);
        mpz_clear(c);
        mpz_clear(tmp);
        return 0;
    }
    printf("# 加密后得到的密文 C 字节串为 #：");
    for (int i = 0; i < C->len; i++) printf("%d ", (int)C->os[i]);
    printf("\n");

    OS_free(C);
    mpz_clear(m);
    mpz_clear(c);
    mpz_clear(tmp);
    return 1;
}