/*
 * Minimal Rijndael/AES compatibility implementation for RakNet.
 * 128-bit block size, AES-128/192/256 key support.
 */
#ifndef RAKNET_RIJNDAEL_H
#define RAKNET_RIJNDAEL_H

#include <stdint.h>
#include <string.h>

class Rijndael
{
public:
    enum Direction { Encrypt, Decrypt };
    enum KeyLength { Key16Bytes = 16, Key24Bytes = 24, Key32Bytes = 32 };

    Rijndael() : Nr(0) {}

    bool init(Direction dir, KeyLength keyLen, const unsigned char *key)
    {
        if (!key) return false;
        int nk = keyLen / 4;
        Nr = nk + 6;
        unsigned int words = 4 * (Nr + 1);
        for (int i = 0; i < nk; ++i)
            rk[i] = ((uint32_t)key[4*i] << 24) |
                    ((uint32_t)key[4*i+1] << 16) |
                    ((uint32_t)key[4*i+2] << 8) |
                    (uint32_t)key[4*i+3];

        static const uint8_t sbox[256] = {
            0x63,0x7c,0x77,0x7b,0xf2,0x6b,0x6f,0xc5,0x30,0x01,0x67,0x2b,0xfe,0xd7,0xab,0x76,
            0xca,0x82,0xc9,0x7d,0xfa,0x59,0x47,0xf0,0xad,0xd4,0xa2,0xaf,0x9c,0xa4,0x72,0xc0,
            0xb7,0xfd,0x93,0x26,0x36,0x3f,0xf7,0xcc,0x34,0xa5,0xe5,0xf1,0x71,0xd8,0x31,0x15,
            0x04,0xc7,0x23,0xc3,0x18,0x96,0x05,0x9a,0x07,0x12,0x80,0xe2,0xeb,0x27,0xb2,0x75,
            0x09,0x83,0x2c,0x1a,0x1b,0x6e,0x5a,0xa0,0x52,0x3b,0xd6,0xb3,0x29,0xe3,0x2f,0x84,
            0x53,0xd1,0x00,0xed,0x20,0xfc,0xb1,0x5b,0x6a,0xcb,0xbe,0x39,0x4a,0x4c,0x58,0xcf,
            0xd0,0xef,0xaa,0xfb,0x43,0x4d,0x33,0x85,0x45,0xf9,0x02,0x7f,0x50,0x3c,0x9f,0xa8,
            0x51,0xa3,0x40,0x8f,0x92,0x9d,0x38,0xf5,0xbc,0xb6,0xda,0x21,0x10,0xff,0xf3,0xd2,
            0xcd,0x0c,0x13,0xec,0x5f,0x97,0x44,0x17,0xc4,0xa7,0x7e,0x3d,0x64,0x5d,0x19,0x73,
            0x60,0x81,0x4f,0xdc,0x22,0x2a,0x90,0x88,0x46,0xee,0xb8,0x14,0xde,0x5e,0x0b,0xdb,
            0xe0,0x32,0x3a,0x0a,0x49,0x06,0x24,0x5c,0xc2,0xd3,0xac,0x62,0x91,0x95,0xe4,0x79,
            0xe7,0xc8,0x37,0x6d,0x8d,0xd5,0x4e,0xa9,0x6c,0x56,0xf4,0xea,0x65,0x7a,0xae,0x08,
            0xba,0x78,0x25,0x2e,0x1c,0xa6,0xb4,0xc6,0xe8,0xdd,0x74,0x1f,0x4b,0xbd,0x8b,0x8a,
            0x70,0x3e,0xb5,0x66,0x48,0x03,0xf6,0x0e,0x61,0x35,0x57,0xb9,0x86,0xc1,0x1d,0x9e,
            0xe1,0xf8,0x98,0x11,0x69,0xd9,0x8e,0x94,0x9b,0x1e,0x87,0xe9,0xce,0x55,0x28,0xdf,
            0x8c,0xa1,0x89,0x0d,0xbf,0xe6,0x42,0x68,0x41,0x99,0x2d,0x0f,0xb0,0x54,0xbb,0x16
        };
        for (unsigned int i = nk; i < words; ++i) {
            uint32_t t = rk[i-1];
            if (i % nk == 0) {
                t = (t << 8) | (t >> 24);
                t = ((uint32_t)sbox[(t>>24)&255] << 24) |
                    ((uint32_t)sbox[(t>>16)&255] << 16) |
                    ((uint32_t)sbox[(t>>8)&255] << 8) |
                    sbox[t&255];
                t ^= (uint32_t)rcon(i/nk) << 24;
            } else if (nk > 6 && i % nk == 4) {
                t = ((uint32_t)sbox[(t>>24)&255] << 24) |
                    ((uint32_t)sbox[(t>>16)&255] << 16) |
                    ((uint32_t)sbox[(t>>8)&255] << 8) |
                    sbox[t&255];
            }
            rk[i] = rk[i-nk] ^ t;
        }
        decrypt = (dir == Decrypt);
        return true;
    }

    void blockEncrypt(const unsigned char *in, int, unsigned char *out) const
    {
        crypt(in, out, false);
    }

    void blockDecrypt(const unsigned char *in, int, unsigned char *out) const
    {
        crypt(in, out, true);
    }

private:
    uint32_t rk[60];
    int Nr;
    bool decrypt;

    static uint8_t rcon(int n) {
        uint8_t c=1;
        while(--n) c=(uint8_t)((c<<1)^((c&0x80)?0x1b:0));
        return c;
    }
    static uint8_t gm(uint8_t a, uint8_t b) {
        uint8_t p=0;
        while(b) { if(b&1)p^=a; a=(uint8_t)((a<<1)^((a&0x80)?0x1b:0)); b>>=1; }
        return p;
    }
    static uint8_t inv(uint8_t a) {
        if (!a) return 0;
        uint8_t r=1, p=a;
        int e=254;
        while(e){ if(e&1) r=gm(r,p); p=gm(p,p); e>>=1; }
        return r;
    }
    static uint8_t sb(uint8_t x) {
        uint8_t y=inv(x), s=y ^ (uint8_t)((y<<1)|(y>>7)) ^
            (uint8_t)((y<<2)|(y>>6)) ^ (uint8_t)((y<<3)|(y>>5)) ^
            (uint8_t)((y<<4)|(y>>4)) ^ 0x63;
        return s;
    }
    static void add(uint8_t *s,const uint32_t *k) {
        for(int c=0;c<4;c++){uint32_t w=k[c];s[4*c]^=w>>24;s[4*c+1]^=w>>16;s[4*c+2]^=w>>8;s[4*c+3]^=w;}
    }
    void crypt(const unsigned char *in,unsigned char *out,bool dec) const {
        uint8_t s[16]; memcpy(s,in,16);
        add(s,rk);
        for(int round=1;round<Nr;round++){
            uint8_t t[16];
            if(!dec){
                for(int i=0;i<16;i++) t[i]=sb(s[i]);
                uint8_t u[16];
                for(int c=0;c<4;c++){u[4*c]=t[4*c];u[4*c+1]=t[4*((c+1)%4)+1];u[4*c+2]=t[4*((c+2)%4)+2];u[4*c+3]=t[4*((c+3)%4)+3];}
                for(int c=0;c<4;c++){uint8_t*a=&u[4*c];t[4*c]=gm(a[0],2)^gm(a[1],3)^a[2]^a[3];t[4*c+1]=a[0]^gm(a[1],2)^gm(a[2],3)^a[3];t[4*c+2]=a[0]^a[1]^gm(a[2],2)^gm(a[3],3);t[4*c+3]=gm(a[0],3)^a[1]^a[2]^gm(a[3],2);}
                memcpy(s,t,16);
                add(s,rk+4*round);
            } else {
                // inverse cipher
                uint8_t u[16];
                for(int c=0;c<4;c++){u[4*c]=s[4*c];u[4*c+1]=s[4*((c+3)%4)+1];u[4*c+2]=s[4*((c+2)%4)+2];u[4*c+3]=s[4*((c+1)%4)+3];}
                for(int i=0;i<16;i++) u[i]=inv(sb(u[i]));
                add(u,rk+4*(Nr-round));
                for(int c=0;c<4;c++){uint8_t*a=&u[4*c];s[4*c]=gm(a[0],14)^gm(a[1],11)^gm(a[2],13)^gm(a[3],9);s[4*c+1]=gm(a[0],9)^gm(a[1],14)^gm(a[2],11)^gm(a[3],13);s[4*c+2]=gm(a[0],13)^gm(a[1],9)^gm(a[2],14)^gm(a[3],11);s[4*c+3]=gm(a[0],11)^gm(a[1],13)^gm(a[2],9)^gm(a[3],14);}
            }
        }
        if(!dec){
            uint8_t t[16];for(int i=0;i<16;i++)t[i]=sb(s[i]);uint8_t u[16];
            for(int c=0;c<4;c++){u[4*c]=t[4*c];u[4*c+1]=t[4*((c+1)%4)+1];u[4*c+2]=t[4*((c+2)%4)+2];u[4*c+3]=t[4*((c+3)%4)+3];}
            memcpy(s,u,16);add(s,rk+4*Nr);
        } else {
            uint8_t u[16];for(int c=0;c<4;c++){u[4*c]=s[4*c];u[4*c+1]=s[4*((c+3)%4)+1];u[4*c+2]=s[4*((c+2)%4)+2];u[4*c+3]=s[4*((c+1)%4)+3];}
            for(int i=0;i<16;i++)s[i]=inv(sb(u[i]));
            add(s,rk);
        }
        memcpy(out,s,16);
    }
};

#endif
