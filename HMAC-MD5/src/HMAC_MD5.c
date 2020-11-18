#include <stdlib.h>
#include <string.h>
#include "MD5.h"
#include "HMAC_MD5.h"

// 设置IPAD、OPAD
const uint8_t IPAD = 0B00110110;
const uint8_t OPAD = 0B01011100;

// 初始化数据
void initData(uint64_t MLen){
    KPlus = (uint8_t*)malloc(64 * sizeof(uint8_t));
    Si = (uint8_t*)malloc((64 + (MLen > 16 ? MLen : 16)) * sizeof(uint8_t));
    So = (uint8_t*)malloc(80 * sizeof(uint8_t));
}

// 检测密钥K的长度，如果大于512位则无法加密
void checkKLen(int *err, int KLen){ if (KLen > 64) *err = 1; }

// 补零
void fillZero(uint64_t MLen, const uint8_t *K, uint64_t KLen){
    // 如果KLen长度大于512需要先进行摘要算法
    if (KLen > 64){
        uint8_t *digestT = (uint8_t*)malloc(16 * sizeof(uint8_t));
        MD5(digestT, K, KLen);
        memcpy(KPlus, digestT, 16);
        memset(KPlus + 16, 0, 48 * sizeof(uint8_t)); // 后方补零
    }
    else {
        memcpy(KPlus, K, KLen);
        memset(KPlus + KLen, 0, (64 - KLen) * sizeof(uint8_t)); // 后方补零
    }
}

// K+与ipad异或生成Si，并生成(Si ‖ M)
void getSi(const uint8_t *M, uint64_t MLen){ 
    for (int i = 0; i < 64; ++i) Si[i] = KPlus[i] ^ IPAD;  
    memcpy(Si + 64, M, MLen); // 得到(Si ‖ M)
}

// K+与opad异或生成Si
void getSo(){ 
    for (int i = 0; i < 64; ++i) So[i] = KPlus[i] ^ OPAD; 
    memcpy(So + 64, Si + 64, 16); // 得到 So ‖ H(Si ‖ M)
}

// HMAC-MD5 算法
void HMAC_MD5(uint8_t *hmac, const uint8_t *M, uint64_t MLen, const uint8_t *K, uint64_t KLen, int *err) {
    initData(MLen);                 // 数据初始化
    // checkKLen(err, KLen);           // 检测密钥K的长度
    // if (*err == 1) return ;         // 不符合退出算法
    
    // HMAC-MD5 算法过程
    fillZero(MLen, K, KLen);        // 补零
    getSi(M, MLen);                 // K+与ipad异或生成Si
    MD5(Si + 64, Si, 64 + MLen);    // hash 压缩得到 H(Si ‖ M)
    getSo();                        // K+与opad生成So
    MD5(hmac, So, 80);              // hash 压缩得到 hmac

    // 释放内存
    free(KPlus);
    free(Si);
    free(So);
}
