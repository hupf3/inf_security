#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include "Data.h"

char M[65], M0[65]; // 初始的明文，经过IP置换得到的明文
char L[33], R[33]; // 经过IP置换后得到的L0和R0
char K[65], K0[57]; // K是密钥，K0是密钥的非检验位
char C[65]; // 得到的密文C
char k[16][49]; // 生成的子密钥
char CC[29], DD[29]; // 分别代表密钥的非检验位的前后28位
char to[16][5] = {
        "0000", "0001", "0010", "0011",
        "0100", "0101", "0110", "0111",
        "1000", "1001", "1010", "1011",
        "1100", "1101", "1110", "1111"
}; // 二进制转化表

// 字符串的交换
void swap(char *p, char *q){
    char t = *p;
    *p = *q;
    *q = t;
}

// IP置换
void IP_replace(){
    for (int i = 0; i < 64; i++) M0[i] = M[IP[i] - 1];
}

// Feistel轮函数
char* feistel(char r[33], int id){
    static char res[33];

    // E-扩展
    char EE[49];
    for (int i = 0; i < 48; i++) EE[i] = r[E[i] - 1];

    // 按位异或
    for (int i = 0; i < 48; i++){
        if (EE[i] == k[id][i]) EE[i] = '0';
        else EE[i] = '1';
    }

    // S盒转换
    char ss[8][7];
    for (int i = 0; i < 8; i++){
        for (int j = 0; j < 6; j++){
            ss[i][j] = EE[6 * i + j];
        }
    }
    char sss[8][5];
    for (int i = 0; i < 8; i++){
        int n = (ss[i][0] - '0') * 2 + (ss[i][5] - '0');
        int m = (ss[i][1] - '0') * 8 + (ss[i][2] - '0') * 4 + (ss[i][3] - '0') * 2 + (ss[i][4] - '0');
        if (i == 0) strcpy(sss[i], to[s1[n][m]]);
        if (i == 1) strcpy(sss[i], to[s2[n][m]]);
        if (i == 2) strcpy(sss[i], to[s3[n][m]]);
        if (i == 3) strcpy(sss[i], to[s4[n][m]]);
        if (i == 4) strcpy(sss[i], to[s5[n][m]]);
        if (i == 5) strcpy(sss[i], to[s6[n][m]]);
        if (i == 6) strcpy(sss[i], to[s7[n][m]]);
        if (i == 7) strcpy(sss[i], to[s8[n][m]]);
    }

    // 顺序连接
    char tt[33];
    for (int i = 0; i < 32; i++) tt[i] = sss[i / 4][i % 4];

    // P置换
    for (int i = 0; i < 32; i++) res[i] = tt[P[i] - 1];

    return res;
}

// 16轮迭代序列T
// 加密
void Iterative_seq(){
    char t_R[33], t_L[33];
    for (int i = 0; i < 16; i++){
        strcpy(t_L, L); strcpy(t_R, R);

        strcpy(L, t_R);

        char t[33];
        strcpy(t, feistel(t_R, i));
        for (int j = 0; j < 32; j++){
            if (t_L[i] == t[i]) R[i] = '0';
            else R[i] = '1';
        }
    }
}

// 解密
void Iterative_seq2(){
    char t_R[33], t_L[33];
    for (int i = 15; i >= 0; i--){
        strcpy(t_L, L); strcpy(t_R, R);

        strcpy(L, t_R);

        char t[33];
        strcpy(t, feistel(t_R, i));
        for (int j = 0; j < 32; j++){
            if (t_L[i] == t[i]) R[i] = '0';
            else R[i] = '1';
        }
    }
}

// 交换置换W
void M_swap(){
    for (int i = 0; i < 32; i++) swap(M0, M0 + 32);
}

// IP逆置换得到密文C
void IP_inverse_replace(){
    for (int i = 0; i < 64; i++) C[i] = M0[_IP[i] - 1];
}

int main() {
    // 解密或加密的操作
    printf("请选择想要进行的操作: 0--加密  1--解密\n");
    int flag;
    scanf("%d", &flag);

    // 输入64位明文数据,和密钥
    if(flag == 0) {printf("请输入想加密的64位明文：\n"); scanf("%s", &M);}
    else  {printf("请输入想解密的64位密文：\n"); scanf("%s", &M);}
    printf("请输入密钥：\n"); scanf("%s", &K);

    // 非检验位进行置换
    for (int i = 0; i < 56; i++) K0[i] = K[PC1[i] - 1];
    for (int i = 0; i < 56; i++){
        if (i < 28) CC[i] = K0[i];
        else DD[i - 28] = K0[i];
    }

    // 生成子密钥k_i
    for (int i = 0; i < 16; i++){
        // 循环移位
        if (i == 0 || i == 1 || i == 8 || i == 15){
            char t1[29], t2[29];
            for (int j = 0; j < 28; j++){
                t1[j] = CC[(j + 1) % 28]; t2[j] = DD[(j + 1) % 28];
            }
            strcpy(CC, t1); strcpy(DD, t2);
        }else {
            char t1[29], t2[29];
            for (int j = 0; j < 28; j++){
                t1[j] = CC[(j + 2) % 28]; t2[j] = DD[(j + 2) % 28];
            }
            strcpy(CC, t1); strcpy(DD, t2);
        }

        // 压缩置换
        char t[57];
        for (int i = 0; i < 56; i++){
            if (i < 28) t[i] = CC[i];
            else t[i] = DD[i - 28];
        }
        for (int j = 0; j < 48; j++) k[i][j] = t[PC2[j] - 1];
    }


    // 初始置换IP
    IP_replace();
    for (int i = 0; i < 32; i++){
        L[i] = M0[i]; R[i] = M0[i + 32];
    }

    // 16轮迭代T
    if (flag == 0) Iterative_seq();
    else Iterative_seq2();

    // 交换置换M
    for (int i = 0; i < 64; i++){
        if (i < 32) M0[i] = L[i];
        else M0[i] = R[i - 32];
    }
    M_swap();

    // 逆置换IP^-1
    IP_inverse_replace();

    // 输出64位密文
    printf("加密后的密文如下：\n"); printf("%s", C);

    return 0;
}
