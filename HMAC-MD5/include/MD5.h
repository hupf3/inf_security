#ifndef MD5_H
#define MD5_H
#include <stdint.h>

// 轮函数定义
#define F(a, b, c) ((a & b) | (~a & c))
#define G(a, b, c) ((a & c) | (b & ~c))
#define H(a, b, c) (a ^ b ^ c)
#define I(a, b, c) (b ^ (a | ~c))

// 512位的消息分组
typedef struct Block{
    uint32_t buffer[16];
}block;

block *blocks; // 块数组
uint64_t blocksNum; // 512位的块数
uint64_t bits; // 明文的位数
uint32_t T[64]; // T表
uint32_t reg[4]; // 初始寄存器A、B、C、D
uint32_t regCur[4]; // 当前寄存器A、B、C、D

// 初始化T表
void init_T();

// 填充
void fill(const uint8_t *M, uint64_t MLen);

// 分块
void devideIntoBlock(const uint8_t *M, uint64_t MLen);

// 循环压缩
void cyclicCompression();

// 缓冲区初始化
void bufferInit();

// 计算得到输出摘要
void computeDigest(uint8_t *digest);

// MD5算法
void MD5(uint8_t *digest, const uint8_t *M, uint64_t MLen);

#endif
