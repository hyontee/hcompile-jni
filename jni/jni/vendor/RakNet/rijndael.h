#pragma once

#include <stdint.h>

#define MAX_KEY_COLUMNS (256/32)
#define MAX_ROUNDS      14
#define MAX_IV_SIZE     16

#define TRUE  1
#define FALSE 0

#define DIR_ENCRYPT 0
#define DIR_DECRYPT 1

#define MODE_ECB 1
#define MODE_CBC 2
#define MODE_CFB1 3

#define BAD_KEY_DIR       -1
#define BAD_KEY_MAT       -2
#define BAD_KEY_INSTANCE  -3
#define BAD_CIPHER_MODE   -4
#define BAD_CIPHER_STATE  -5
#define BAD_BLOCK_LENGTH  -6
#define BAD_CIPHER_INSTANCE -7
#define BAD_DATA          -8
#define BAD_OTHER         -9

#define MAX_KEY_SIZE     64
#define MAX_IV_SIZE      16

typedef struct
{
    int   keyLen;
    char  keyMaterial[MAX_KEY_SIZE + 4];
    int   Nr;
    uint32_t rk[4 * (MAX_ROUNDS + 1)];
    uint32_t ek[4 * (MAX_ROUNDS + 1)];
} keyInstance;

typedef struct
{
    int   mode;
    char  IV[MAX_IV_SIZE];
    int   blockLen;
} cipherInstance;

#ifdef __cplusplus
extern "C" {
#endif

int makeKey(keyInstance *key, int direction, int keyLen, char *keyMaterial);
int cipherInit(cipherInstance *cipher, int mode, char *IV);
int blockEncrypt(cipherInstance *cipher, keyInstance *key, unsigned char *input, int inputOctets, unsigned char *outBuffer);
int blockDecrypt(cipherInstance *cipher, keyInstance *key, unsigned char *input, int inputOctets, unsigned char *outBuffer);

#ifdef __cplusplus
}
#endif
