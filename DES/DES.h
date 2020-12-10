#ifndef DES_H
#define DES_H

// DES加密,返回明文的长度
int Encrypt(const char *key, const char *msg, int len, char *output);

// DES解密
void Decrypt(const char *key, const char *msg, int len, char *output);

#endif