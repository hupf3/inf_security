//
// Created by 胡鹏飞 on 2020/10/21.
//
#include <stdio.h>
#include <stdlib.h>
#include "Step.h"
#include "Decrypt.h"

int Decrypt(mpz_t n, mpz_t d, OS *C, int k, int mLen){
    mpz_t c; mpz_init(c);
    OS2IP(C, c);
    // gmp_printf("c : %Zd\n", c);
    if (mpz_cmp(c, n) >= 0){
        printf("密文代表超出范围，解密出错！\n");
        mpz_clear(c);
        return 0;
    }

    mpz_t m; mpz_init(m); mpz_set(m, c);
    // RSADP(n, d, c, m);
    printf("# 解密后得到的整数消息代表 m 为 #：");
    gmp_printf("%Zd\n", m);
    printf("\n");

    OS *EM; EM = (OS *)malloc(sizeof(OS));
    OSinit(EM);
    I2OSP(m, k, EM);
//  printf("%d\n", EM->len);
    printf("# 解密后得到的编码信息 EM 字节串为 #：");
    for (int i = 0; i < EM->len; i++){
        printf("%d ", EM->os[i]);
    }
    printf("\n\n");
    if (EM->os[0] != 0 || EM->os[1] != 2 || EM->os[EM->len - mLen - 1] != 0){
        printf("解密出错！\n");
        mpz_clear(m);
        OS_free(EM);
        mpz_clear(c);
        return 0;
    }

    printf("# 解密后的明文 M 为 #：");
    for (int i = EM->len - mLen; i < EM->len; i++) printf("%c", (char)(EM->os[i]));
    printf("\n");

    OS_free(EM);
    mpz_clear(m);
    mpz_clear(c);
    return 1;
}