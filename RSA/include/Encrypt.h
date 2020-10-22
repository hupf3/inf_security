//
// Created by 胡鹏飞 on 2020/10/21.
//

#ifndef ENCRYPT_H
#define ENCRYPT_H
#include "gmpp.h"
#include "Octetstring.h"

int Encrypt(mpz_t n, mpz_t e, char *M, int PSLen, int mLen, int k, OS *str);

#endif