/***************************************************************************
 Bourriquet MorphOS
 2024-2025 digitally
 
***************************************************************************/
#include <stdlib.h>    // strtol
#include <string.h>

#include "MD4.h"

unsigned char PADDING[64] = {
  0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

void MD4Init(MD4_CTX *context)
{
  context->count[0] = context->count[1] = 0;
  context->state[0] = 0x67452301;
  context->state[1] = 0xefcdab89;
  context->state[2] = 0x98badcfe;
  context->state[3] = 0x10325476;
}

void MD4Update(MD4_CTX *context, unsigned char *input, unsigned int inputLen)
{
   	unsigned int i, index, partLen;

   	index = (unsigned int)((context->count[0] >> 3) & 0x3F);
   	if((context->count[0] += ((UINT4)inputLen << 3)) < ((UINT4)inputLen << 3)) context->count[1]++;
   	context->count[1] += ((UINT4)inputLen >> 29);
   	partLen = 64 - index;
   	if(inputLen >= partLen)
      	{
         	MD4_memcpy((POINTER)&context->buffer[index], (POINTER)input, partLen);
         	MD4Transform(context->state, context->buffer);
         	for(i = partLen; i + 63 < inputLen; i += 64) MD4Transform(context->state, &input[i]);
         	index = 0;
      	}
   	else i = 0;
   	MD4_memcpy((POINTER)&context->buffer[index], (POINTER)&input[i], inputLen-i);
}

void MD4Final(unsigned char digest[16], MD4_CTX *context)
{
   	unsigned char bits[8];
   	unsigned int index, padLen;

   	Encode(bits, context->count, 8);
   	index = (unsigned int)((context->count[0] >> 3) & 0x3f);
   	padLen = (index < 56) ? (56 - index) : (120 - index);
   	MD4Update(context, PADDING, padLen);
   	MD4Update(context, bits, 8);
   	Encode(digest, context->state, 16);
   	MD4_memset((POINTER)context, 0, sizeof (*context));
}            

void MD4Transform(UINT4 state[4], unsigned char block[64])
{
   	UINT4 a = state[0], b = state[1], c = state[2], d = state[3], x[16];
   
	Decode(x, block, 64);
   	FF(a, b, c, d, x[ 0], S11); /* 1 */
   	FF(d, a, b, c, x[ 1], S12); /* 2 */
   	FF(c, d, a, b, x[ 2], S13); /* 3 */
   	FF(b, c, d, a, x[ 3], S14); /* 4 */
   	FF(a, b, c, d, x[ 4], S11); /* 5 */
   	FF(d, a, b, c, x[ 5], S12); /* 6 */
   	FF(c, d, a, b, x[ 6], S13); /* 7 */
   	FF(b, c, d, a, x[ 7], S14); /* 8 */
   	FF(a, b, c, d, x[ 8], S11); /* 9 */
   	FF(d, a, b, c, x[ 9], S12); /* 10 */
   	FF(c, d, a, b, x[10], S13); /* 11 */
   	FF(b, c, d, a, x[11], S14); /* 12 */
   	FF(a, b, c, d, x[12], S11); /* 13 */
   	FF(d, a, b, c, x[13], S12); /* 14 */
   	FF(c, d, a, b, x[14], S13); /* 15 */
   	FF(b, c, d, a, x[15], S14); /* 16 */

   	GG(a, b, c, d, x[ 0], S21); /* 17 */
   	GG(d, a, b, c, x[ 4], S22); /* 18 */
   	GG(c, d, a, b, x[ 8], S23); /* 19 */
   	GG(b, c, d, a, x[12], S24); /* 20 */
   	GG(a, b, c, d, x[ 1], S21); /* 21 */
   	GG(d, a, b, c, x[ 5], S22); /* 22 */
   	GG(c, d, a, b, x[ 9], S23); /* 23 */
   	GG(b, c, d, a, x[13], S24); /* 24 */
   	GG(a, b, c, d, x[ 2], S21); /* 25 */
   	GG(d, a, b, c, x[ 6], S22); /* 26 */
   	GG(c, d, a, b, x[10], S23); /* 27 */
   	GG(b, c, d, a, x[14], S24); /* 28 */
   	GG(a, b, c, d, x[ 3], S21); /* 29 */
   	GG(d, a, b, c, x[ 7], S22); /* 30 */
   	GG(c, d, a, b, x[11], S23); /* 31 */
   	GG(b, c, d, a, x[15], S24); /* 32 */

   	HH(a, b, c, d, x[ 0], S31); /* 33 */
   	HH(d, a, b, c, x[ 8], S32); /* 34 */
   	HH(c, d, a, b, x[ 4], S33); /* 35 */
   	HH(b, c, d, a, x[12], S34); /* 36 */
   	HH(a, b, c, d, x[ 2], S31); /* 37 */
   	HH(d, a, b, c, x[10], S32); /* 38 */
   	HH(c, d, a, b, x[ 6], S33); /* 39 */
   	HH(b, c, d, a, x[14], S34); /* 40 */
   	HH(a, b, c, d, x[ 1], S31); /* 41 */
   	HH(d, a, b, c, x[ 9], S32); /* 42 */
   	HH(c, d, a, b, x[ 5], S33); /* 43 */
   	HH(b, c, d, a, x[13], S34); /* 44 */
   	HH(a, b, c, d, x[ 3], S31); /* 45 */
   	HH(d, a, b, c, x[11], S32); /* 46 */
   	HH(c, d, a, b, x[ 7], S33); /* 47 */
   	HH(b, c, d, a, x[15], S34); /* 48 */

   	state[0] += a;
   	state[1] += b;
   	state[2] += c;
   	state[3] += d;
   	MD4_memset((POINTER)x, 0, sizeof(x));
}

const unsigned char *md4(const char *txt) 
{
   	static unsigned char hash[MD4_DIGEST_LENGTH];
   	MD4_CTX ctx;

   	if(txt == NULL) return(NULL);   
   	MD4Init(&ctx);
   	MD4Update(&ctx, (unsigned char *)txt, strlen (txt));
   	MD4Final(hash, &ctx);
   	return(hash);
}

void Encode(unsigned char *output, UINT4 *input, unsigned int len)
{
   	unsigned int i, j;

   	for(i = 0, j = 0; j < len; i++, j += 4)
      	{
         	output[j] = (unsigned char)(input[i] & 0xff);
         	output[j + 1] = (unsigned char)((input[i] >> 8) & 0xff);
         	output[j + 2] = (unsigned char)((input[i] >> 16) & 0xff);
         	output[j + 3] = (unsigned char)((input[i] >> 24) & 0xff);
      	}
}

void Decode(unsigned long *output, unsigned char *input, unsigned int len)
{
   	unsigned int i, j;
   
	for(i = 0, j = 0; j < len; i++, j += 4) output[i] = ((UINT4)input[j]) | (((UINT4)input[j+1]) << 8) | (((UINT4)input[j+2]) << 16) | (((UINT4)input[j+3]) << 24);
}