#include<stdio.h>
#include<string.h>
#include "DES.h"
#include "Data.h"

// 获取
int get(long long data, int index) { return (data >> (63 - index)) & 1; }

// 设值
void set(long long *data, int index, int v) {
    *data &= ~(1ll << (63 - index));
    *data |= (((long long)v) << (63 - index));
}

// 置换
void permute(long long input, const int* table, const int bits, long long *output) {
    *output = 0ll;
    for (int i = 0; i < bits; ++i) set(output, i, get(input, table[i] - 1));
}

// IP置换
void permuteIP(long long input, long long *output) {
    permute(input, (int*)tableIP, 64, output);
}

// IP逆置换
void permuteRIP(long long input, long long *output) {
    permute(input, (int*)tableRIP, 64, output);
}

// 循环左移一个位置
void rotateLeft1Bit(long long *keyLL, int k) {
    int tmp = get(*keyLL, 0 + k * 28);
    for (int i = 0 + k * 28; i < 27 + k * 28; ++i) set(keyLL, i, get(*keyLL, i + 1));
    set(keyLL, 27 + k * 28, tmp);
}

// 循环左移
void rotateLeft(int i, long long *keyLL, int k) {
    rotateLeft1Bit(keyLL, k);
    if (i == 1 || i == 2 || i == 9 || i == 16) return;
    else rotateLeft1Bit(keyLL, k);
}

// 生成子密钥
void generateSubKey(const long long keyLL, long long* subKey) {
    long long tmp;
    permute(keyLL, (int*)tablePC1, 56, &tmp);
    for (int i = 1; i <= 16; ++i) {
        rotateLeft(i, &tmp, 0);
        rotateLeft(i, &tmp, 1);
        permute(tmp, (int*)tablePC2, 48, &subKey[i - 1]);
    }
}

// Feistel轮函数
void feistel(long long R,const long long K,long long *output) {
    long long tmp = 0ll;

    // E-扩展
    permute(R, (int*)tableE, 48, output);

    // 按位异或
    for (int i = 0; i < 48; ++i) set(output, i, get(*output, i) ^ get(K, i));
    
    // S盒转换
    for (int i = 0; i < 8; ++i){
        int a = (get(*output,0 + i * 6) << 1) + get(*output, 5 + i * 6);
        int b = (get(*output,1 + i * 6) << 3) + (get(*output,2 + i * 6) << 2) + (get(*output, 3 + i * 6) << 1) + get(*output,4 + i * 6);
        a = tableS[i][a * 16 + b];
        for (int j = 0 ; j < 4 ; j++){
            b = a & 1;
            a = a >> 1;
            set(&tmp,i*4+3-j,b);
        }
    }
    permute(tmp, (int*)tableP, 32, output);
}

// 16轮迭代序列T
void Iterative_seq(long long data, const long long keyLL, long long *output) {
    long long high = 0ll;
    for (int i = 0; i < 32; ++i) set(&high, i, get(data, 32 + i)); 
    feistel(high, keyLL, output);
    for (int i = 0; i < 32; ++i) {
        set(output, 32 + i, get(*output, i) ^ get(data, i));
        set(output, i, get(high, i));
    }
}

void DES(long long input, const long long keyLL, long long *output, int flag) {
    // 生成子密钥
    long long subKey[16];
    generateSubKey(keyLL, (long long*)subKey);

    // IP置换
    permuteIP(input, output);

    //迭代序列 T
    for (int i = 0; i < 16; ++i) {
        // 判断是加密或者解密
        if (flag) Iterative_seq(*output, subKey[15 - i], output);
        else Iterative_seq(*output, subKey[i], output);
    }

    // 交换置换W
    long long low = *output & ((1ll << 32) - 1);
    *output = (*output >> 32) & ((1ll << 32) - 1);
    *output |= low << 32;

    // IP逆置换
    permuteRIP(*output,output);
}

// 将字符串输入转化成LL
void stringToLL(const char* input, long long *data, int len){
    char buf[9];
    int i; 
    for (i = 0; i < len && i < 8; ++i) buf[i] = input[i];
    i = 8 - len;
    while (len < 8) buf[len++] = i;
    long long tmp = *((long long*)buf);
    for (i = 0; i < 64; ++i) set(data, i, (tmp >> (63 - (56 - i / 8 * 8 + i % 8))) & 1);
}

// 将LL转变为字符串
void LLToString(long long data, char* buf){
    long long *tmp = (long long*)buf;
    int i, j;
    for (i = 0; i < 64; ++i) set(tmp, 56 - i / 8 * 8 + i % 8, get(data, i));
    for (i = buf[7], j = 1; j < i; ++j){
        if (buf[7 - j] != i) i = 0;
    }
    if (!!i){ 
        for (i = buf[7], j = 0; j < i; ++j) buf[7 - j] = '\0';
    }
    buf[8] = '\0';
}

// 加密
int Encrypt(const char *key, const char *msg, int len, char *output){
    char input[200];
    memcpy(input, msg, (len + 1)*sizeof(char));
    long long keyLL; 
    stringToLL(key, (long long*)&keyLL, strlen(key));
    long long data, encoded;
    for (int i = 0 ; i < len / 8 + 1 ; ++i){
        stringToLL(&input[8 * i], &data, len - i * 8);
        DES(data, keyLL, &encoded, 0);
        LLToString(encoded, &output[8 * i]);
    }
    output[(len / 8 + 1) * 64] = '\0';
    return (len / 8 + 1) * 8;
}

// 解密
void Decrypt(const char *key, const char *msg, int len, char *output){
    char input[200];
    memcpy(input, msg, (len + 1)*sizeof(char));
    long long keyLL; 
    stringToLL(key, (long long*)&keyLL, strlen(key));
    long long data, decoded;
    for (int i = 0 ; i < len / 8; ++i){
        stringToLL(&input[8 * i], &data, len - i * 8);
        DES(data, keyLL, &decoded, 1);
        LLToString(decoded, &output[8 * i]);
    }
    output[len / 8 * 64] = '\0';
}
