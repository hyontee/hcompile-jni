#pragma once

#define MAX_KEY_COLUMNS (256/32)
#define MAX_ROUNDS      14
#define MAX_IV_SIZE     16

class CRijndael
{
public:
    enum { ECB=0, CBC=1, CFB1=2 };

    CRijndael();
    ~CRijndael();

    void MakeKey(const char* key, const char* iv, int keyLen, int blockLen);
    void Encrypt(const char* in, char* result, unsigned long numBytes, int iMode=ECB);
    void Decrypt(const char* in, char* result, unsigned long numBytes, int iMode=ECB);

private:
    int     m_keylength;
    int     m_blockSize;
    int     m_state;
    unsigned long   m_encKey[MAX_ROUNDS+1][8];
    unsigned long   m_decKey[MAX_ROUNDS+1][8];
    char    m_initVector[MAX_IV_SIZE];

    void KeySched(unsigned char key[][4]);
    void KeyEncToDec();
    void Encrypt(unsigned long a[8]);
    void Decrypt(unsigned long a[8]);
};
