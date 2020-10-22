//
// Created by 胡鹏飞 on 2020/10/21.
//

#ifndef STEP_H
#define STEP_H
#include <gmp.h>
#include "Octetstring.h"

// EM = 0x00 || 0x02 || PS || 0x00 || M.
void getEM(OS *str, int PSLen, char *M, int mLen);

void OS2IP(OS *str, mpz_t m);

void RSAEP(mpz_t n, mpz_t e, mpz_t m, mpz_t c);

int I2OSP(mpz_t c, int k, OS *C);

void RSADP(mpz_t n, mpz_t d, mpz_t c, mpz_t m);

#endif
