#include <stdio.h>
#include <gmp.h>
#include <stdlib.h>
#include <string.h>
#include "GetKey.h"
#include "Decrypt.h"
#include "Encrypt.h"
#include "Step.h"

const int LEN_MAXN = 120; // 最长输入117个字符
char M[LEN_MAXN]; // 待输入的消息

int main(){
    mpz_t n; mpz_init(n);
    mpz_t d; mpz_init(d);
    mpz_t e; mpz_init(e);
    getKey(1024, n, d, e);

    printf("# 请输入需要加密的密文 M #：");
    scanf("%s", M); printf("\n");
    gmp_printf("# 模数 n 为 #：%Zd\n", n); printf("\n");
    gmp_printf("# 公钥 d 为 #：%Zd\n", d); printf("\n");
    gmp_printf("# 私钥 e 为 #：%Zd\n", e); printf("\n");

    int mLen = strlen(M); // 明文的字节长度
    // printf("%d\n", mLen);
    int k = 1024 / 8; // 总的字节长度
    int PSLen = k - mLen - 3; // PS 的字节长度
    // RSAES-PKCS1-v1_5 规定
    if (mLen > k - 11){
        printf("消息太长！\n");
        mpz_clear(n);
        mpz_clear(d);
        mpz_clear(e);
        return 1;
    }

    // 加密
    printf("\n【正在进行加密】\n");
    OS *str; str = (OS *)malloc(sizeof(OS));
    OSinit(str);
    int flag;
    flag = Encrypt(n, e, M, PSLen, mLen, k, str); // 加密并输出密文
    if (flag == 0) {
        OS_free(str);
        mpz_clear(n);
        mpz_clear(d);
        mpz_clear(e);
        return 1;
    }

    // 解密
    printf("\n【正在进行解密】\n");
    if (str->len != k || k < 11){
        printf("解密错误\n");
        return 1;
    }
    flag = Decrypt(n, d, str, k, mLen); // 解密并输出消息
    if (flag == 0) {
        OS_free(str);
        mpz_clear(n);
        mpz_clear(d);
        mpz_clear(e);
        return 1;
    }

    // 释放空间
    OS_free(str);
    mpz_clear(n);
    mpz_clear(d);
    mpz_clear(e);
    return 0;
}