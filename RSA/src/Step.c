//
// Created by 胡鹏飞 on 2020/10/21.
//

#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "Step.h"

const int MAXN = 255;
int vis[260];

// EM = 0x00 || 0x02 || PS || 0x00 || M.
void getEM(OS *str, int PSLen, char *M, int mLen){
    add_int(str, 0); add_int(str, 2);

    // 生成 PS
    srand((unsigned)time(NULL));
    memset(vis, 0, sizeof(vis));
    for (int i = 0; i < PSLen; i++){
        int t = rand() % MAXN + 1; // 不能为0
        while (vis[t] == 1){
            t = rand() % MAXN + 1;
        }
        vis[t] = 1;
        add_int(str, t);
    }

    add_int(str, 0);

    // 加入明文 M
    for (int i = 0; i < mLen; i++) add_char(str, M[i]);
}

void OS2IP(OS *str, mpz_t m){
    mpz_t base; mpz_init(base);
    mpz_set_ui(base, 256);
    mpz_t tmp; mpz_init(tmp);
    for (int i = str->len - 1, e = 0; i >= 0; i--, e++){
        mpz_pow_ui(tmp, base, e);
        mpz_mul_ui(tmp, tmp, str->os[i]);
        mpz_add(m, m, tmp);
    }

    mpz_clear(base);
    mpz_clear(tmp);
}



int I2OSP(mpz_t c, int k, OS *C){
    mpz_t base; mpz_init(base); mpz_set_ui(base, 256);
    mpz_t tmp; mpz_init(tmp);
    mpz_pow_ui(tmp, base, k);
    if (mpz_cmp(c, tmp) >= 0){
        printf("整数太大！\n");

        mpz_clear(base);
        mpz_clear(tmp);
        return 0;
    }
    mpz_t pre; mpz_init(pre); mpz_set(pre, c);
    while (mpz_cmp_ui(pre, 0) != 0){
//      gmp_printf("%Zd ", base);
//      gmp_printf("%d ", e);
//      gmp_printf("%Zd ", pre);
//      gmp_printf("%Zd\n", tmp);
        mpz_mod(tmp, pre, base);
        mpz_div(pre, pre, base);
        add_int(C, mpz_get_ui(tmp));
//      printf("%d ", mpz_get_ui(tmp));
//      gmp_printf("%Zd ", q);
//      printf("%d ", (int)(q));
    }
    int pad_0 = k - C->len;
    for (int i = 0; i < pad_0; i++) add_int(C, 0);

    OS_inverse(C);
    // printf("\n");
    mpz_clear(base);
    mpz_clear(tmp);
    mpz_clear(pre);
    return 1;
}

void RSADP(mpz_t n, mpz_t d, mpz_t c, mpz_t m){
    mpz_powm(m, c, d, n);
}