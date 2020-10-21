//
// Created by 胡鹏飞 on 2020/10/21.
//

#include "GetKey.h"
#include <time.h>

// 获取大素数 p 和 q
void getPQ(mpz_t t, int n){
    mpz_rrandomb(t, state, n);
    mpz_nextprime(t, t);
}

// 判断 p 和 q 是否符和标准
int checkPQ(mpz_t p, mpz_t q, int k){
    mpz_t t; mpz_init(t);
    mpz_sub(t, p, q);
    mpz_abs(t, t);

    // log|p - q|> k/2-100
    mpz_t t1; mpz_init(t1);
    int e1 = k / 2 - 100;
    mpz_t b1; mpz_init(b1); mpz_set_ui(b1, 2);
    mpz_pow_ui(t1, b1, e1);
    if (mpz_cmp(t, t1) <= 0) return 0;

    // log|p - q|> k/3
    mpz_t t2; mpz_init(t2);
    int e2 = k / 3;
    mpz_t b2; mpz_init(b2); mpz_set_ui(b2, 2);
    mpz_pow_ui(t2, b2, e2);
    if (mpz_cmp(t, t2) <= 0) return 0;

    return 1;
}

// 判断 N 的位数是否为 k
int checkN(mpz_t N, int k){
    if (mpz_sizeinbase(N, 2) == k) return 1;
    return 0;
}

// 获取密钥
void getKey(int k, mpz_t n, mpz_t d, mpz_t e){
    // 获取随机值
    gmp_randinit_default(state);
    gmp_randseed_ui(state, time(NULL));

    mpz_t phiN; mpz_init(phiN);
    while (1){
        mpz_t p; mpz_init(p);
        mpz_t q; mpz_init(q);
        mpz_t N; mpz_init(N);

        getPQ(p, (k + 1) / 2);
        getPQ(q, (k - 1) / 2);
        mpz_mul(N, p, q);

        if (checkPQ(p, q, k) && checkN(N, k)){

            mpz_sub_ui(p, p, 1); mpz_sub_ui(q, q, 1);
            mpz_mul(phiN, p, q);
            mpz_set(n, N);

            // 进行销毁
            mpz_clear(N);
            mpz_clear(p);
            mpz_clear(q);
            break;
        }

        // 进行销毁
        mpz_clear(N);
        mpz_clear(p);
        mpz_clear(q);
    }

    // PKCS#1 建议
    mpz_set_ui(e, 65537);

    // d 是 e 的模phiN 逆元
    mpz_invert(d, e, phiN);
}