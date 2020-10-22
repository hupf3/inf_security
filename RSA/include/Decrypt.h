//
// Created by 胡鹏飞 on 2020/10/21.
//

#ifndef DECRYPT_H
#define DECRYPT_H
#include <gmp.h>
#include "Octetstring.h"

int Decrypt(mpz_t n, mpz_t d, OS *C, int k, int mLen);

#endif
