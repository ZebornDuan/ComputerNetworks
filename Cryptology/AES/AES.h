#ifndef AES_H
#define AES_H

/*
@parameters
p/c : plain text/cipher text
length : length of p/c
key : cipher key
*/

void aes_encrypt(char *p, int length, char *key);
void aes_decrypt(char *c, int length, char *key);

#endif