#ifndef MD4_H
#define MD4_H

/***************************************************************************
 Copyright (C) 2003-2004 by RUSBACK
***************************************************************************/

#define S11 3
#define S12 7
#define S13 11
#define S14 19
#define S21 3
#define S22 5
#define S23 9
#define S24 13
#define S31 3
#define S32 9
#define S33 11
#define S34 15
#define MD4_memcpy memcpy
#define MD4_memset memset
#define UINT4   unsigned long
#define POINTER unsigned char*
#define MD4_DIGEST_LENGTH 16

typedef struct
{
   unsigned long state[4];                                   /* state (ABCD) */
   unsigned long count[2];        /* number of bits, modulo 2^64 (lsb first) */
   unsigned char buffer[64];                         /* input buffer */
} MD4_CTX;

void MD4Transform(UINT4[4], unsigned char[64]);
void Encode(unsigned char *, UINT4 *, unsigned int);
void Decode(UINT4 *, unsigned char *, unsigned int);
void MD4Init(MD4_CTX *);
void MD4Update(MD4_CTX *, unsigned char *, unsigned int);
void MD4Final(unsigned char digest[16], MD4_CTX *);
const unsigned char *md4(const char *);

#define F(x, y, z) (((x) & (y)) | ((~x) & (z)))
#define G(x, y, z) (((x) & (y)) | ((x) & (z)) | ((y) & (z)))
#define H(x, y, z) ((x) ^ (y) ^ (z))
#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32-(n))))
#define FF(a, b, c, d, x, s) { \
    (a) += F ((b), (c), (d)) + (x); \
    (a) = ROTATE_LEFT ((a), (s)); \
  }
#define GG(a, b, c, d, x, s) { \
    (a) += G ((b), (c), (d)) + (x) + (UINT4)0x5a827999; \
    (a) = ROTATE_LEFT ((a), (s)); \
  }
#define HH(a, b, c, d, x, s) { \
    (a) += H ((b), (c), (d)) + (x) + (UINT4)0x6ed9eba1; \
    (a) = ROTATE_LEFT ((a), (s)); \
  }

#endif /* MD4.h */
