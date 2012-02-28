#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "md5.h"
typedef struct
{
	uint32 _state[4];       /* state (ABCD) */
        uint32 _count[2];       /* number of bits, modulo 2^64 (low-order word first) */
        byte _buffer[64];       /* input buffer */
        byte _digest[16];       /* message digest */
        unsigned char _finished;         /* calculate finished ? */
}md5var_t;

char * md5sum (const char* file);
void decode(const byte* input, uint32* output, size_t length);
void update(const byte* input, size_t length,  md5var_t * var);
void transform(const byte * block, md5var_t * var);
char* bytesToHexString(const byte* input, size_t length, char* szret); 
const byte* digest(md5var_t * var);


#ifdef _TEST_
int main (int argc, char * argv[])
{
	if (argc > 1) 
	printf ("%s %s\n", md5sum(argv[1]), argv[1]);
	return 1;
}
#endif
char * md5sum (const char* file)
{
	static char szret[50] = {0};
	int fd = 0;
	unsigned char szbuf [1024] = {0};
	int len = 0;
	memset (szret, 0, 50);
	fd = open (file, O_RDONLY);
	if (fd == -1) return "";
	md5var_t var;
	memset (&var, 0, sizeof(md5var_t));
	var._finished = 0;
        /* reset number of bits. */
        var._count[0] = var._count[1] = 0;
        /* Load magic initialization constants. */
        var._state[0] = 0x67452301;
        var._state[1] = 0xefcdab89;
       	var._state[2] = 0x98badcfe;
        var._state[3] = 0x10325476;
	
	while ((len = read (fd, szbuf, 1024)) > 0)
	{
		//printf ("%s", szbuf);
		update (szbuf, len, &var);
		memset (szbuf, 0, len);
	}
	close (fd);
	bytesToHexString(digest(&var), 16, szret);
	return szret;
}

void update(const byte* input, size_t length, md5var_t * var)
{
	uint32 i, index, partLen;

        var->_finished = 0;

        /* Compute number of bytes mod 64 */
        index = (uint32)((var->_count[0] >> 3) & 0x3f);

        /* update number of bits */
        if ((var->_count[0] += ((uint32)length << 3)) < ((uint32)length << 3)) 
        {   
                ++(var->_count[1]);
        }   
        var->_count[1] += ((uint32)length >> 29);

        partLen = 64 - index;

        /* transform as many times as possible. */
        if (length >= partLen) 
        {   

                memcpy(&(var->_buffer[index]), input, partLen);
                transform(var->_buffer, var);	
		for (i = partLen; i + 63 < length; i += 64)
                {
                        transform(&input[i], var);
                }
                index = 0;

        } else
        {
                i = 0;
        }

        /* Buffer remaining input */
        memcpy(&(var->_buffer[index]), &input[i], length - i);
}
void transform(const byte * block, md5var_t * var)
{
	uint32 a = var->_state[0], b =var->_state[1], c = var->_state[2], d = var->_state[3], x[16];

	decode(block, x, 64);

	/* Round 1 */
	FF (a, b, c, d, x[ 0], S11, 0xd76aa478); /* 1 */
	FF (d, a, b, c, x[ 1], S12, 0xe8c7b756); /* 2 */
	FF (c, d, a, b, x[ 2], S13, 0x242070db); /* 3 */
	FF (b, c, d, a, x[ 3], S14, 0xc1bdceee); /* 4 */
	FF (a, b, c, d, x[ 4], S11, 0xf57c0faf); /* 5 */
	FF (d, a, b, c, x[ 5], S12, 0x4787c62a); /* 6 */
	FF (c, d, a, b, x[ 6], S13, 0xa8304613); /* 7 */
	FF (b, c, d, a, x[ 7], S14, 0xfd469501); /* 8 */
	FF (a, b, c, d, x[ 8], S11, 0x698098d8); /* 9 */
	FF (d, a, b, c, x[ 9], S12, 0x8b44f7af); /* 10 */
	FF (c, d, a, b, x[10], S13, 0xffff5bb1); /* 11 */
	FF (b, c, d, a, x[11], S14, 0x895cd7be); /* 12 */
	FF (a, b, c, d, x[12], S11, 0x6b901122); /* 13 */
	FF (d, a, b, c, x[13], S12, 0xfd987193); /* 14 */
	FF (c, d, a, b, x[14], S13, 0xa679438e); /* 15 */
	FF (b, c, d, a, x[15], S14, 0x49b40821); /* 16 */

	/* Round 2 */
	GG (a, b, c, d, x[ 1], S21, 0xf61e2562); /* 17 */
	GG (d, a, b, c, x[ 6], S22, 0xc040b340); /* 18 */
	GG (c, d, a, b, x[11], S23, 0x265e5a51); /* 19 */
	GG (b, c, d, a, x[ 0], S24, 0xe9b6c7aa); /* 20 */
	GG (a, b, c, d, x[ 5], S21, 0xd62f105d); /* 21 */
	GG (d, a, b, c, x[10], S22,  0x2441453); /* 22 */
	GG (c, d, a, b, x[15], S23, 0xd8a1e681); /* 23 */
	GG (b, c, d, a, x[ 4], S24, 0xe7d3fbc8); /* 24 */
	GG (a, b, c, d, x[ 9], S21, 0x21e1cde6); /* 25 */
	GG (d, a, b, c, x[14], S22, 0xc33707d6); /* 26 */
	GG (c, d, a, b, x[ 3], S23, 0xf4d50d87); /* 27 */
	GG (b, c, d, a, x[ 8], S24, 0x455a14ed); /* 28 */
	GG (a, b, c, d, x[13], S21, 0xa9e3e905); /* 29 */
	GG (d, a, b, c, x[ 2], S22, 0xfcefa3f8); /* 30 */
	GG (c, d, a, b, x[ 7], S23, 0x676f02d9); /* 31 */
	GG (b, c, d, a, x[12], S24, 0x8d2a4c8a); /* 32 */

	/* Round 3 */
	HH (a, b, c, d, x[ 5], S31, 0xfffa3942); /* 33 */
	HH (d, a, b, c, x[ 8], S32, 0x8771f681); /* 34 */
	HH (c, d, a, b, x[11], S33, 0x6d9d6122); /* 35 */
	HH (b, c, d, a, x[14], S34, 0xfde5380c); /* 36 */
	HH (a, b, c, d, x[ 1], S31, 0xa4beea44); /* 37 */
	HH (d, a, b, c, x[ 4], S32, 0x4bdecfa9); /* 38 */
	HH (c, d, a, b, x[ 7], S33, 0xf6bb4b60); /* 39 */
	HH (b, c, d, a, x[10], S34, 0xbebfbc70); /* 40 */
	HH (a, b, c, d, x[13], S31, 0x289b7ec6); /* 41 */
	HH (d, a, b, c, x[ 0], S32, 0xeaa127fa); /* 42 */
	HH (c, d, a, b, x[ 3], S33, 0xd4ef3085); /* 43 */
	HH (b, c, d, a, x[ 6], S34,  0x4881d05); /* 44 */
	HH (a, b, c, d, x[ 9], S31, 0xd9d4d039); /* 45 */
	HH (d, a, b, c, x[12], S32, 0xe6db99e5); /* 46 */
	HH (c, d, a, b, x[15], S33, 0x1fa27cf8); /* 47 */
	HH (b, c, d, a, x[ 2], S34, 0xc4ac5665); /* 48 */

	/* Round 4 */
	II (a, b, c, d, x[ 0], S41, 0xf4292244); /* 49 */
	II (d, a, b, c, x[ 7], S42, 0x432aff97); /* 50 */
	II (c, d, a, b, x[14], S43, 0xab9423a7); /* 51 */
	II (b, c, d, a, x[ 5], S44, 0xfc93a039); /* 52 */
	II (a, b, c, d, x[12], S41, 0x655b59c3); /* 53 */
	II (d, a, b, c, x[ 3], S42, 0x8f0ccc92); /* 54 */
	II (c, d, a, b, x[10], S43, 0xffeff47d); /* 55 */
	II (b, c, d, a, x[ 1], S44, 0x85845dd1); /* 56 */
	II (a, b, c, d, x[ 8], S41, 0x6fa87e4f); /* 57 */
	II (d, a, b, c, x[15], S42, 0xfe2ce6e0); /* 58 */
	II (c, d, a, b, x[ 6], S43, 0xa3014314); /* 59 */
	II (b, c, d, a, x[13], S44, 0x4e0811a1); /* 60 */
	II (a, b, c, d, x[ 4], S41, 0xf7537e82); /* 61 */
	II (d, a, b, c, x[11], S42, 0xbd3af235); /* 62 */
	II (c, d, a, b, x[ 2], S43, 0x2ad7d2bb); /* 63 */
	II (b, c, d, a, x[ 9], S44, 0xeb86d391); /* 64 */

	var->_state[0] += a;
	var->_state[1] += b;
	var->_state[2] += c;
	var->_state[3] += d;
}
void encode(const uint32* input, byte* output, size_t length)
{

      		size_t i, j;
	  for (i = 0, j = 0; j < length; ++i, j += 4) {
                output[j]= (byte)(input[i] & 0xff);
                output[j + 1] = (byte)((input[i] >> 8) & 0xff);
                output[j + 2] = (byte)((input[i] >> 16) & 0xff);
                output[j + 3] = (byte)((input[i] >> 24) & 0xff);
        }
}
void decode(const byte* input, uint32* output, size_t length) 
{
	size_t i = 0, j = 0;
        for (i = 0, j = 0; j < length; ++i, j += 4) {
                output[i] = ((uint32)input[j]) | (((uint32)input[j + 1]) << 8) |
                (((uint32)input[j + 2]) << 16) | (((uint32)input[j + 3]) << 24);
        }   
}

char* bytesToHexString(const byte* input, size_t length, char* szret) 
{

        memset (szret, 0, strlen(szret));
	int len = 0;
        size_t i = 0;
	//str.reserve(length << 1); 
        for (i = 0; i < length; ++i) 
        {   
                int t = input[i];
                int a = t / 16; 
                int b = t % 16; 
                szret[len++] = HEX[a];
		szret[len++] = HEX[b];
		//str.append(1, HEX[a]);
                //str.append(1, HEX[b]);
        }   
        return szret;
}
void final(md5var_t * var) 
{

	byte bits[8];
	uint32 oldState[4];
	uint32 oldCount[2];
	uint32 index, padLen;

	/* Save current state and count. */
	memcpy(oldState, var->_state, 16);
	memcpy(oldCount, var->_count, 8);

	/* Save number of bits */
	encode(var->_count, bits, 8);

	/* Pad out to 56 mod 64. */
	index = (uint32)((var->_count[0] >> 3) & 0x3f);
	padLen = (index < 56) ? (56 - index) : (120 - index);
	update(PADDING, padLen, var);

	/* Append length (before padding) */
	update(bits, 8, var);

	/* Store state in digest */
	encode(var->_state, var->_digest, 16);

	/* Restore current state and count. */
	memcpy(var->_state, oldState, 16);
	memcpy(var->_count, oldCount, 8);
}

const byte* digest(md5var_t * var)
{

        if (!var->_finished)
        {
                var->_finished = 1;
                final(var);
        }
        return var->_digest;
}
