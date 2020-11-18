#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "MD5.h"
#include "HMAC_MD5.h"

const uint8_t *M1, *M2, *M3; // 明文
const uint8_t *K1, *K2, *K3; // 密钥
uint64_t MLen1, KLen1, MLen2, KLen2, MLen3, KLen3; // 明文、密文的长度
uint8_t digest1[16], digest2[16], digest3[16]; // MD5输出得到的128位消息摘要
uint8_t hmac1[16], hmac2[16], hmac3[16]; // HMAC-MD5输出得到的结果
int err1 = 0; int err2 = 0; int err3 = 0; // 判断能否解密成功


// 初始化明文、密钥
void init(){
    // test1: 正常情况
    M1 = (uint8_t *)"iloveyou"; K1 = (uint8_t *)"mykey";
    MLen1 = strlen((const char *)M1); KLen1 = strlen((const char *)K1);

    // test2: M的位数余512大于448
    M2 = (uint8_t *)"serxjvhcvxkcvxwerxjvhcvxkcvxwerxjvhcvxkcvxwerxjvhcvxksse"; K2 = (uint8_t *)"mykey";
    MLen2 = strlen((const char *)M2); KLen2 = strlen((const char *)K2);

    // test3: K的位长度大于512
    M3 = (uint8_t *)"iloveyou"; K3 = (uint8_t *)"serxjvhcvxkcvxwerxjvhcvxkcvxwerxjvhcvxkcvxwerxjvhcvxkssewesdsfddf";
    MLen3 = strlen((const char *)M3); KLen3 = strlen((const char *)K3);
}

// 密钥长度过长超过512位
void print_ERROR(){
    printf("Error: 密钥长度过长！");
    printf("\n\n");
}

// 输出MD5的结果1
void print_MD5(){
    printf("------------------------ MD5 ------------------------");
    printf("\n\n");
    printf("----------- TEST1 -----------");
    printf("\n\n");
    printf("明文: %s\n\n", M1);
    printf("MD5 加密后得到的信息摘要: ");
    for (int i = 0; i < 16; ++i) printf("%02x", digest1[i]);
    printf("\n\n");

    printf("----------- TEST2 -----------");
    printf("\n\n");
    printf("明文: %s\n\n", M2);
    printf("MD5 加密后得到的信息摘要: ");
    for (int i = 0; i < 16; ++i) printf("%02x", digest2[i]);
    printf("\n\n");

    printf("----------- TEST3 -----------");
    printf("\n\n");
    printf("明文: %s\n\n", M3);
    printf("MD5 加密后得到的信息摘要: ");
    for (int i = 0; i < 16; ++i) printf("%02x", digest3[i]);
    printf("\n\n");
}

// 输出HMAC-MD5的结果
void print_HMAC_MD5(){
    printf("-------------------- HMAC-MD5 --------------------");
    printf("\n\n");
    printf("----------- TEST1 -----------");
    printf("\n\n");
    printf("明文: %s\n\n", M1);
    printf("密钥: %s\n\n", K1);
    if (err1) print_ERROR();
    else {
        printf("HMAC-MD5 加密后得到的结果: ");
        for (int i = 0; i < 16; ++i) printf("%02x", hmac1[i]);
        printf("\n\n");
    }

    printf("----------- TEST2 -----------");
    printf("\n\n");
    printf("明文: %s\n\n", M2);
    printf("密钥: %s\n\n", K2);
    if (err2) print_ERROR();
    else {
        printf("HMAC-MD5 加密后得到的结果: ");
        for (int i = 0; i < 16; ++i) printf("%02x", hmac2[i]);
        printf("\n\n");
    }

    printf("----------- TEST3 -----------");
    printf("\n\n");
    printf("明文: %s\n\n", M3);
    printf("密钥: %s\n\n", K3);
    if (err3) print_ERROR();
    else {  
        printf("HMAC-MD5 加密后得到的结果: ");
        for (int i = 0; i < 16; ++i) printf("%02x", hmac3[i]);
        printf("\n\n");
    }
}

int main() {
    init(); // 初始化数据

    // ----------- MD5 -----------
    MD5(digest1, M1, MLen1); // MD5加密算法
    MD5(digest2, M2, MLen2); // MD5加密算法
    MD5(digest3, M3, MLen3); // MD5加密算法
    print_MD5(); // 输出MD5加密算法结果

    // ----------- HMAC-MD5 -----------
    HMAC_MD5(hmac1, (uint8_t*)M1, MLen1, (uint8_t*)K1, KLen1, &err1); // HMAC-MD5算法
    HMAC_MD5(hmac2, (uint8_t*)M2, MLen2, (uint8_t*)K2, KLen2, &err2); // HMAC-MD5算法
    HMAC_MD5(hmac3, (uint8_t*)M3, MLen3, (uint8_t*)K3, KLen3, &err3); // HMAC-MD5算法
    print_HMAC_MD5(); // 输出HMAC-MD5算法结果
    return 0;
}
