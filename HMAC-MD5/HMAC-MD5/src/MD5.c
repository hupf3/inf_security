#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdio.h>
#include "MD5.h"

// 初始化T表
void init_T(){
    for (int i = 1; i <= 64; i++)
        T[i - 1] = (uint32_t)((double)(((uint64_t)1) << 32) * fabs(sin(i)));
}

// 填充
void fill(const uint8_t *M, uint64_t MLen){
    bits = MLen * 8; // 明文的位数
    uint64_t r = bits % 512; // 取余
    // 计算分成的块数
    if (r <= 448) blocksNum = bits / 512 + 1;
    else blocksNum = bits / 512 + 2;
}

// 分块
void devideIntoBlock(const uint8_t *M, uint64_t MLen) {
    // 初始化分组消息块
    blocks = (block *)malloc(blocksNum * sizeof(block)); 
    for (uint64_t i = 0; i < blocksNum; ++i) memset(blocks + i, 0, sizeof(block));

    uint64_t index = 0; // 扫描明文的索引
    int flag = 1; // 跳出循环标志
    for (int i = 0; i < blocksNum && flag; ++i){
        for (int j = 0; j < 16 && flag; ++j){
            for (int k = 0; k < 4 && flag; ++k){
                uint32_t val = M[index];

                // 如果扫描到了明文的最后一个字符跳出循环
                if (index++ == MLen){
                    val = (1 << 7);
                    flag = 0;
                }

                // 将明文信息填入到block中的buffer
                blocks[i].buffer[j] |= (val << (k * 8));
            }
        }
    }

    // 将明文的位数补充到块中
    blocks[blocksNum - 1].buffer[16 - 2] = (uint32_t)bits;
}

// 缓冲区初始化
void bufferInit(){
    reg[0] = 0x67452301;
    reg[1] = 0xefcdab89;
    reg[2] = 0x98badcfe;
    reg[3] = 0x10325476;
}

// 循环移位
uint32_t cyclicShift(uint32_t a, uint32_t b){
    return (a << b) | (a >> (32 - b));
}

// 获取寄存器当前值
uint32_t getCurReg(int index, int offset){
    return regCur[(index + offset) % 4];
}

// 循环压缩
void cyclicCompression(uint8_t *digest){
    int k, index;
    for (uint64_t i = 0; i < blocksNum; ++i) {
        // 提取到消息分组的位
        const uint32_t *X = blocks[i].buffer;

        // 初始化当前寄存器中的值
        memcpy(regCur, reg, sizeof(regCur));
        index = 0;

        // 第1轮16次迭代
        k = 0; const uint32_t s1[4] = {7, 12, 17, 22}; // 设置k值和移位值
        for (int j = 0; j < 16; ++j) {
            regCur[index] = getCurReg(index, 1) + cyclicShift((getCurReg(index, 0) + F(getCurReg(index, 1), getCurReg(index, 2), getCurReg(index, 3)) + X[k] + T[j]), s1[j & 3]);
            index = ((index + 3) % 4);
            k ++;
        }

        // 第2轮16次迭代
        k = 1; const uint32_t s2[4] = {5, 9, 14, 20}; // 设置k值和移位值
        for (int j = 16; j < 32; ++j) {
            regCur[index] = getCurReg(index, 1) + cyclicShift((getCurReg(index, 0) + G(getCurReg(index, 1), getCurReg(index, 2), getCurReg(index, 3)) + X[k] + T[j]), s2[j & 3]);
            index = ((index + 3) % 4);
            k = (k + 5) % 16;
        }

        // 第3轮16次迭代
        k = 5; const uint32_t s3[4] = {4, 11, 16, 23}; // 设置k值和移位值
        for (int j = 32; j < 48; ++j) {
            regCur[index] = getCurReg(index, 1) + cyclicShift((getCurReg(index, 0) + H(getCurReg(index, 1), getCurReg(index, 2), getCurReg(index, 3)) + X[k] + T[j]), s3[j & 3]);
            index = ((index + 3) % 4);
            k = (k + 3) % 16;
        }

        // 第4轮16次迭代
        k = 0; const uint32_t s4[4] = {6, 10, 15, 21}; // 设置k值和移位值
        for (int j = 48; j < 64; ++j) {
            regCur[index] = getCurReg(index, 1) + cyclicShift((getCurReg(index, 0) + I(getCurReg(index, 1), getCurReg(index, 2), getCurReg(index, 3)) + X[k] + T[j]), s4[j & 3]);
            index = ((index + 3) % 4);
            k = (k + 7) % 16;
        }

        // 更新寄存器中的值
        for (int j = 0; j < 4; ++j) reg[j] += regCur[j];
    }
}

// 计算得到输出摘要
void computeDigest(uint8_t *digest){
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j)
            digest[(i << 2) | j] = ((reg[i] >> (j * 8)) & 255);
}

// MD5 算法
void MD5(uint8_t *digest, const uint8_t *M, uint64_t MLen) {
    // 初始化T表
    init_T();

    // MD5 算法过程
    fill(M, MLen);              // 填充
    devideIntoBlock(M, MLen);   // 分块
    bufferInit();               // 缓冲区初始化
    cyclicCompression(digest);  // 循环移位

    // 计算得到输出摘要
    computeDigest(digest);

    // 释放内存
    free(blocks);
}
