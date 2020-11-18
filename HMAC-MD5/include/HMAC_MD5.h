#ifndef HMAC_MD5_H
#define HMAC_MD5_H
#include <stdint.h>

uint8_t *KPlus; // 数据块K+
uint8_t *Si; // 数据块Si
uint8_t *So; // 数据块So

// 初始化数据
void initData();

// 检测密钥K的长度，如果大于512位则无法加密
void checkKLen(int *err, int KLen);

// 补零
void fillZero();

// K+与ipad异或生成Si
void getSi();

// K+与opad异或生成Si
void getSo();

// HMAC-MD5 算法
void HMAC_MD5(uint8_t *hmac, const uint8_t *M, uint64_t MLen, const uint8_t *K, uint64_t KLen, int *err);

#endif
