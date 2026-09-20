/// ile
/// rief RakNet-compatible AES/Rijndael API used by DataBlockEncryptor.
///
/// This implementation intentionally keeps the legacy RakNet API/structures so
/// the bundled RakNet sources compile without any external Rijndael dependency.
#ifndef __RIJNDAEL_ALG_H
#define __RIJNDAEL_ALG_H

#include <stddef.h>

#define MAXKC       (256/32)
#define MAXROUNDS   14

typedef unsigned char  word8;
typedef unsigned short word16;
typedef unsigned int   word32;

#define DIR_ENCRYPT     0
#define DIR_DECRYPT     1
#define MODE_ECB        1
#define MODE_CBC        2
#define MODE_CFB1       3
#ifndef TRUE
#define TRUE            1
#endif
#ifndef FALSE
#define FALSE           0
#endif
#define BITSPERBLOCK    128
#define BAD_KEY_DIR     -1
#define BAD_KEY_MAT     -2
#define BAD_KEY_INSTANCE -3
#define BAD_CIPHER_MODE -4
#define BAD_CIPHER_STATE -5
#define BAD_BLOCK_LENGTH -6
#define BAD_CIPHER_INSTANCE -7
#define MAX_KEY_SIZE    32
#define MAX_IV_SIZE     16

typedef unsigned char BYTE;

typedef struct {
    BYTE  direction;
    int   keyLen; // bits
    char  keyMaterial[MAX_KEY_SIZE + 1];
    int   blockLen;
    word8 keySched[MAXROUNDS + 1][4][4];
} keyInstance;

typedef struct {
    BYTE mode;
    BYTE IV[MAX_IV_SIZE];
    int blockLen;
} cipherInstance;

int makeKey(keyInstance *key, BYTE direction, int keyLen, char *keyMaterial);
int cipherInit(cipherInstance *cipher, BYTE mode, char *IV);
int blockEncrypt(cipherInstance *cipher, keyInstance *key, BYTE *input,
                 int inputLen, BYTE *outBuffer);
int blockDecrypt(cipherInstance *cipher, keyInstance *key, BYTE *input,
                 int inputLen, BYTE *outBuffer);
int cipherUpdateRounds(cipherInstance *cipher, keyInstance *key, BYTE *input,
                       int inputLen, BYTE *outBuffer, int rounds);

#endif
