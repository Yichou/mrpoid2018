/*-----------------------------------*/
// crc32, md5, unzip, c2u, isMr

#include <string.h>
#include <zlib.h>

#include "mr_helper.h"
#include "mrporting.h"
#include "mr_mem.h"
#include "mythroad.h"


/*-----------------------------------*/
// crc32

// crc表
static const unsigned long crc_32_tab[256] = {
    0x00000000UL, 0x77073096UL, 0xee0e612cUL, 0x990951baUL, 0x076dc419UL,
    0x706af48fUL, 0xe963a535UL, 0x9e6495a3UL, 0x0edb8832UL, 0x79dcb8a4UL,
    0xe0d5e91eUL, 0x97d2d988UL, 0x09b64c2bUL, 0x7eb17cbdUL, 0xe7b82d07UL,
    0x90bf1d91UL, 0x1db71064UL, 0x6ab020f2UL, 0xf3b97148UL, 0x84be41deUL,
    0x1adad47dUL, 0x6ddde4ebUL, 0xf4d4b551UL, 0x83d385c7UL, 0x136c9856UL,
    0x646ba8c0UL, 0xfd62f97aUL, 0x8a65c9ecUL, 0x14015c4fUL, 0x63066cd9UL,
    0xfa0f3d63UL, 0x8d080df5UL, 0x3b6e20c8UL, 0x4c69105eUL, 0xd56041e4UL,
    0xa2677172UL, 0x3c03e4d1UL, 0x4b04d447UL, 0xd20d85fdUL, 0xa50ab56bUL,
    0x35b5a8faUL, 0x42b2986cUL, 0xdbbbc9d6UL, 0xacbcf940UL, 0x32d86ce3UL,
    0x45df5c75UL, 0xdcd60dcfUL, 0xabd13d59UL, 0x26d930acUL, 0x51de003aUL,
    0xc8d75180UL, 0xbfd06116UL, 0x21b4f4b5UL, 0x56b3c423UL, 0xcfba9599UL,
    0xb8bda50fUL, 0x2802b89eUL, 0x5f058808UL, 0xc60cd9b2UL, 0xb10be924UL,
    0x2f6f7c87UL, 0x58684c11UL, 0xc1611dabUL, 0xb6662d3dUL, 0x76dc4190UL,
    0x01db7106UL, 0x98d220bcUL, 0xefd5102aUL, 0x71b18589UL, 0x06b6b51fUL,
    0x9fbfe4a5UL, 0xe8b8d433UL, 0x7807c9a2UL, 0x0f00f934UL, 0x9609a88eUL,
    0xe10e9818UL, 0x7f6a0dbbUL, 0x086d3d2dUL, 0x91646c97UL, 0xe6635c01UL,
    0x6b6b51f4UL, 0x1c6c6162UL, 0x856530d8UL, 0xf262004eUL, 0x6c0695edUL,
    0x1b01a57bUL, 0x8208f4c1UL, 0xf50fc457UL, 0x65b0d9c6UL, 0x12b7e950UL,
    0x8bbeb8eaUL, 0xfcb9887cUL, 0x62dd1ddfUL, 0x15da2d49UL, 0x8cd37cf3UL,
    0xfbd44c65UL, 0x4db26158UL, 0x3ab551ceUL, 0xa3bc0074UL, 0xd4bb30e2UL,
    0x4adfa541UL, 0x3dd895d7UL, 0xa4d1c46dUL, 0xd3d6f4fbUL, 0x4369e96aUL,
    0x346ed9fcUL, 0xad678846UL, 0xda60b8d0UL, 0x44042d73UL, 0x33031de5UL,
    0xaa0a4c5fUL, 0xdd0d7cc9UL, 0x5005713cUL, 0x270241aaUL, 0xbe0b1010UL,
    0xc90c2086UL, 0x5768b525UL, 0x206f85b3UL, 0xb966d409UL, 0xce61e49fUL,
    0x5edef90eUL, 0x29d9c998UL, 0xb0d09822UL, 0xc7d7a8b4UL, 0x59b33d17UL,
    0x2eb40d81UL, 0xb7bd5c3bUL, 0xc0ba6cadUL, 0xedb88320UL, 0x9abfb3b6UL,
    0x03b6e20cUL, 0x74b1d29aUL, 0xead54739UL, 0x9dd277afUL, 0x04db2615UL,
    0x73dc1683UL, 0xe3630b12UL, 0x94643b84UL, 0x0d6d6a3eUL, 0x7a6a5aa8UL,
    0xe40ecf0bUL, 0x9309ff9dUL, 0x0a00ae27UL, 0x7d079eb1UL, 0xf00f9344UL,
    0x8708a3d2UL, 0x1e01f268UL, 0x6906c2feUL, 0xf762575dUL, 0x806567cbUL,
    0x196c3671UL, 0x6e6b06e7UL, 0xfed41b76UL, 0x89d32be0UL, 0x10da7a5aUL,
    0x67dd4accUL, 0xf9b9df6fUL, 0x8ebeeff9UL, 0x17b7be43UL, 0x60b08ed5UL,
    0xd6d6a3e8UL, 0xa1d1937eUL, 0x38d8c2c4UL, 0x4fdff252UL, 0xd1bb67f1UL,
    0xa6bc5767UL, 0x3fb506ddUL, 0x48b2364bUL, 0xd80d2bdaUL, 0xaf0a1b4cUL,
    0x36034af6UL, 0x41047a60UL, 0xdf60efc3UL, 0xa867df55UL, 0x316e8eefUL,
    0x4669be79UL, 0xcb61b38cUL, 0xbc66831aUL, 0x256fd2a0UL, 0x5268e236UL,
    0xcc0c7795UL, 0xbb0b4703UL, 0x220216b9UL, 0x5505262fUL, 0xc5ba3bbeUL,
    0xb2bd0b28UL, 0x2bb45a92UL, 0x5cb36a04UL, 0xc2d7ffa7UL, 0xb5d0cf31UL,
    0x2cd99e8bUL, 0x5bdeae1dUL, 0x9b64c2b0UL, 0xec63f226UL, 0x756aa39cUL,
    0x026d930aUL, 0x9c0906a9UL, 0xeb0e363fUL, 0x72076785UL, 0x05005713UL,
    0x95bf4a82UL, 0xe2b87a14UL, 0x7bb12baeUL, 0x0cb61b38UL, 0x92d28e9bUL,
    0xe5d5be0dUL, 0x7cdcefb7UL, 0x0bdbdf21UL, 0x86d3d2d4UL, 0xf1d4e242UL,
    0x68ddb3f8UL, 0x1fda836eUL, 0x81be16cdUL, 0xf6b9265bUL, 0x6fb077e1UL,
    0x18b74777UL, 0x88085ae6UL, 0xff0f6a70UL, 0x66063bcaUL, 0x11010b5cUL,
    0x8f659effUL, 0xf862ae69UL, 0x616bffd3UL, 0x166ccf45UL, 0xa00ae278UL,
    0xd70dd2eeUL, 0x4e048354UL, 0x3903b3c2UL, 0xa7672661UL, 0xd06016f7UL,
    0x4969474dUL, 0x3e6e77dbUL, 0xaed16a4aUL, 0xd9d65adcUL, 0x40df0b66UL,
    0x37d83bf0UL, 0xa9bcae53UL, 0xdebb9ec5UL, 0x47b2cf7fUL, 0x30b5ffe9UL,
    0xbdbdf21cUL, 0xcabac28aUL, 0x53b39330UL, 0x24b4a3a6UL, 0xbad03605UL,
    0xcdd70693UL, 0x54de5729UL, 0x23d967bfUL, 0xb3667a2eUL, 0xc4614ab8UL,
    0x5d681b02UL, 0x2a6f2b94UL, 0xb40bbe37UL, 0xc30c8ea1UL, 0x5a05df1bUL,
    0x2d02ef8dUL
};

static uint32 crc_mr_updcrc_0 = 0xffffffff;

uint32 mr_updcrc(uint8 *s, uint32 n) {
	uint32 i;

	if (NULL != s) {
		for (i = crc_mr_updcrc_0; 0 != n; n--)
			i = (i >> 8) ^ crc_32_tab[(uint8) (*s++ ^ i)];
	} else {
		i = 0xffffffff;
	}

	crc_mr_updcrc_0 = i;

	return ~i;
}

/*-----------------------------------*/
// MD5

#define T1 0xd76aa478
#define T2 0xe8c7b756
#define T3 0x242070db
#define T4 0xc1bdceee
#define T5 0xf57c0faf
#define T6 0x4787c62a
#define T7 0xa8304613
#define T8 0xfd469501
#define T9 0x698098d8
#define T10 0x8b44f7af
#define T11 0xffff5bb1
#define T12 0x895cd7be
#define T13 0x6b901122
#define T14 0xfd987193
#define T15 0xa679438e
#define T16 0x49b40821
#define T17 0xf61e2562
#define T18 0xc040b340
#define T19 0x265e5a51
#define T20 0xe9b6c7aa
#define T21 0xd62f105d
#define T22 0x02441453
#define T23 0xd8a1e681
#define T24 0xe7d3fbc8
#define T25 0x21e1cde6
#define T26 0xc33707d6
#define T27 0xf4d50d87
#define T28 0x455a14ed
#define T29 0xa9e3e905
#define T30 0xfcefa3f8
#define T31 0x676f02d9
#define T32 0x8d2a4c8a
#define T33 0xfffa3942
#define T34 0x8771f681
#define T35 0x6d9d6122
#define T36 0xfde5380c
#define T37 0xa4beea44
#define T38 0x4bdecfa9
#define T39 0xf6bb4b60
#define T40 0xbebfbc70
#define T41 0x289b7ec6
#define T42 0xeaa127fa
#define T43 0xd4ef3085
#define T44 0x04881d05
#define T45 0xd9d4d039
#define T46 0xe6db99e5
#define T47 0x1fa27cf8
#define T48 0xc4ac5665
#define T49 0xf4292244
#define T50 0x432aff97
#define T51 0xab9423a7
#define T52 0xfc93a039
#define T53 0x655b59c3
#define T54 0x8f0ccc92
#define T55 0xffeff47d
#define T56 0x85845dd1
#define T57 0x6fa87e4f
#define T58 0xfe2ce6e0
#define T59 0xa3014314
#define T60 0x4e0811a1
#define T61 0xf7537e82
#define T62 0xbd3af235
#define T63 0x2ad7d2bb
#define T64 0xeb86d391

#define ARCH_IS_BIG_ENDIAN 0

#ifndef ARCH_IS_BIG_ENDIAN
# define ARCH_IS_BIG_ENDIAN 1 /* slower, default implementation */
#endif

static void md5_process(md5_state_t *pms, const md5_byte_t *data /*[64]*/)
{
    md5_word_t a = pms->abcd[0], b = pms->abcd[1], c = pms->abcd[2], d = pms->abcd[3];
    md5_word_t t;

#if ARCH_IS_BIG_ENDIAN

    /*
    * On big-endian machines, we must arrange the bytes in the right
    * order. (This also works on machines of unknown byte order.)
    */
    md5_word_t X[16];
    const md5_byte_t *xp = data;
    int i;

    for (i = 0; i < 16; ++i, xp += 4)
        X[i] = xp[0] + (xp[1] << 8) + (xp[2] << 16) + (xp[3] << 24);

#else/* !ARCH_IS_BIG_ENDIAN */

    /*
    * On little-endian machines, we can process properly aligned data
    * without copying it.
    */
    md5_word_t xbuf[16];
    const md5_word_t *X;

    if (!((data - (const md5_byte_t *)0) & 3)) {
        /* data are properly aligned */
        X = (const md5_word_t *)data;
    } else {
        /* not aligned */
        memcpy(xbuf, data, 64);
        X = xbuf;
    }
#endif

#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32 - (n))))

    /* Round 1. */
    /* Let [abcd k s i] denote the operation
    a = b + ((a + F(b,c,d) + X[k] + T[i]) <<< s). */
#define F(x, y, z) (((x) & (y)) | (~(x) & (z)))
#define SET(a, b, c, d, k, s, Ti) \
    t = a + F(b,c,d) + X[k] + Ti; \
    a = ROTATE_LEFT(t, s) + b

    /* Do the following 16 operations. */
    SET(a, b, c, d, 0, 7, T1);
    SET(d, a, b, c, 1, 12, T2);
    SET(c, d, a, b, 2, 17, T3);
    SET(b, c, d, a, 3, 22, T4);
    SET(a, b, c, d, 4, 7, T5);
    SET(d, a, b, c, 5, 12, T6);
    SET(c, d, a, b, 6, 17, T7);
    SET(b, c, d, a, 7, 22, T8);
    SET(a, b, c, d, 8, 7, T9);
    SET(d, a, b, c, 9, 12, T10);
    SET(c, d, a, b, 10, 17, T11);
    SET(b, c, d, a, 11, 22, T12);
    SET(a, b, c, d, 12, 7, T13);
    SET(d, a, b, c, 13, 12, T14);
    SET(c, d, a, b, 14, 17, T15);
    SET(b, c, d, a, 15, 22, T16);
#undef SET

    /* Round 2. */
    /* Let [abcd k s i] denote the operation
    a = b + ((a + G(b,c,d) + X[k] + T[i]) <<< s). */
#define G(x, y, z) (((x) & (z)) | ((y) & ~(z)))
#define SET(a, b, c, d, k, s, Ti) \
    t = a + G(b,c,d) + X[k] + Ti; \
    a = ROTATE_LEFT(t, s) + b

    /* Do the following 16 operations. */
    SET(a, b, c, d, 1, 5, T17);
    SET(d, a, b, c, 6, 9, T18);
    SET(c, d, a, b, 11, 14, T19);
    SET(b, c, d, a, 0, 20, T20);
    SET(a, b, c, d, 5, 5, T21);
    SET(d, a, b, c, 10, 9, T22);
    SET(c, d, a, b, 15, 14, T23);
    SET(b, c, d, a, 4, 20, T24);
    SET(a, b, c, d, 9, 5, T25);
    SET(d, a, b, c, 14, 9, T26);
    SET(c, d, a, b, 3, 14, T27);
    SET(b, c, d, a, 8, 20, T28);
    SET(a, b, c, d, 13, 5, T29);
    SET(d, a, b, c, 2, 9, T30);
    SET(c, d, a, b, 7, 14, T31);
    SET(b, c, d, a, 12, 20, T32);
#undef SET

    /* Round 3. */
    /* Let [abcd k s t] denote the operation
    a = b + ((a + H(b,c,d) + X[k] + T[i]) <<< s). */
#define H(x, y, z) ((x) ^ (y) ^ (z))
#define SET(a, b, c, d, k, s, Ti) \
    t = a + H(b,c,d) + X[k] + Ti; \
    a = ROTATE_LEFT(t, s) + b

    /* Do the following 16 operations. */
    SET(a, b, c, d, 5, 4, T33);
    SET(d, a, b, c, 8, 11, T34);
    SET(c, d, a, b, 11, 16, T35);
    SET(b, c, d, a, 14, 23, T36);
    SET(a, b, c, d, 1, 4, T37);
    SET(d, a, b, c, 4, 11, T38);
    SET(c, d, a, b, 7, 16, T39);
    SET(b, c, d, a, 10, 23, T40);
    SET(a, b, c, d, 13, 4, T41);
    SET(d, a, b, c, 0, 11, T42);
    SET(c, d, a, b, 3, 16, T43);
    SET(b, c, d, a, 6, 23, T44);
    SET(a, b, c, d, 9, 4, T45);
    SET(d, a, b, c, 12, 11, T46);
    SET(c, d, a, b, 15, 16, T47);
    SET(b, c, d, a, 2, 23, T48);
#undef SET

    /* Round 4. */
    /* Let [abcd k s t] denote the operation
    a = b + ((a + I(b,c,d) + X[k] + T[i]) <<< s). */
#define I(x, y, z) ((y) ^ ((x) | ~(z)))
#define SET(a, b, c, d, k, s, Ti) \
    t = a + I(b,c,d) + X[k] + Ti; \
    a = ROTATE_LEFT(t, s) + b

    /* Do the following 16 operations. */
    SET(a, b, c, d, 0, 6, T49);
    SET(d, a, b, c, 7, 10, T50);
    SET(c, d, a, b, 14, 15, T51);
    SET(b, c, d, a, 5, 21, T52);
    SET(a, b, c, d, 12, 6, T53);
    SET(d, a, b, c, 3, 10, T54);
    SET(c, d, a, b, 10, 15, T55);
    SET(b, c, d, a, 1, 21, T56);
    SET(a, b, c, d, 8, 6, T57);
    SET(d, a, b, c, 15, 10, T58);
    SET(c, d, a, b, 6, 15, T59);
    SET(b, c, d, a, 13, 21, T60);
    SET(a, b, c, d, 4, 6, T61);
    SET(d, a, b, c, 11, 10, T62);
    SET(c, d, a, b, 2, 15, T63);
    SET(b, c, d, a, 9, 21, T64);
#undef SET

    /* Then perform the following additions. (That is increment each
    of the four registers by the value it had before this block
    was started.) */
    pms->abcd[0] += a;
    pms->abcd[1] += b;
    pms->abcd[2] += c;
    pms->abcd[3] += d;
}

void mr_md5_init(md5_state_t *pms)
{
    pms->count[0] = pms->count[1] = 0;
    pms->abcd[0] = 0x67452301;
    pms->abcd[1] = 0xefcdab89;
    pms->abcd[2] = 0x98badcfe;
    pms->abcd[3] = 0x10325476;
}

void mr_md5_append(md5_state_t *pms, const md5_byte_t *data, int nbytes)
{
    const md5_byte_t *p = data;
    int left = nbytes;
    int offset = (pms->count[0] >> 3) & 63;
    md5_word_t nbits = (md5_word_t)(nbytes << 3);

    if (nbytes <= 0) return;

    /* Update the message length. */
    pms->count[1] += nbytes >> 29;
    pms->count[0] += nbits;
    if (pms->count[0] < nbits) pms->count[1]++;

    /* Process an initial partial block. */
    if (offset) {
        int copy = (offset + nbytes > 64 ? 64 - offset : nbytes);

        memcpy(pms->buf + offset, p, copy);
        if (offset + copy < 64) return;
        p += copy;
        left -= copy;
        md5_process(pms, pms->buf);
    }

    /* Process full blocks. */
    for (; left >= 64; p += 64, left -= 64)
        md5_process(pms, p);

    /* Process a final partial block. */
    if (left)
        memcpy(pms->buf, p, left);
}

void mr_md5_finish(md5_state_t *pms, md5_byte_t digest[16])
{
    static const md5_byte_t pad[64] = {0x80, 0};
    md5_byte_t data[8];
    int i;

    /* Save the length before padding. */
    for (i = 0; i < 8; ++i)
        data[i] = (md5_byte_t)(pms->count[i >> 2] >> ((i & 3) << 3));
    /* Pad to 56 bytes mod 64. */
    mr_md5_append(pms, pad, ((55 - (pms->count[0] >> 3)) & 63) + 1);
    /* Append the length. */
    mr_md5_append(pms, data, 8);
    for (i = 0; i < 16; ++i)
        digest[i] = (md5_byte_t)(pms->abcd[i >> 2] >> ((i & 3) << 3));
}

/*-----------------------------------*/
// unzip

/*
解压gzip数据
备注：
改编自zlib中uncompress函数 2012-9-9 eleqian
返回值：
Z_OK - 成功
Z_MEM_ERROR - 内存不足
Z_BUF_ERROR - 输出缓冲区不足
Z_DATA_ERROR - 数据错误
*/
static int ungzipdata(uint8 *dest, uint32 *destLen, const uint8 *source, uint32 sourceLen)
{
    z_stream stream;
    int err;

    stream.next_in = (Bytef*)source;
    stream.avail_in = (uInt)sourceLen;
    stream.next_out = (Bytef*)dest;
    stream.avail_out = (uInt)*destLen;
    stream.zalloc = (alloc_func)0;
    stream.zfree = (free_func)0;

    err = inflateInit2(&stream, MAX_WBITS + 16);
    if (err != Z_OK)
        return err;

    err = inflate(&stream, Z_FINISH);
    if (err != Z_STREAM_END) {
        inflateEnd(&stream);
        if (err == Z_NEED_DICT || (err == Z_BUF_ERROR && stream.avail_in == 0))
            return Z_DATA_ERROR;
        return err;
    }

    *destLen = stream.total_out;
    err = inflateEnd(&stream);

    return err;
}

int32 mr_unzip(uint8* inputbuf, int32 inputlen, uint8** outputbuf, int32* outputlen)
{
    int ret;


#ifdef LOG_FILE
    LOGI("mr_unzip(%#p,%d,%#p,%#p)", inputbuf, inputlen, outputbuf, outputlen);
#endif

    if(!inputbuf || inputlen<=0
    	|| !outputbuf || !outputlen
        || inputbuf[0] != 0x1f
        || inputbuf[1] != 0x8b)
        return MR_FAILED;

    *outputlen = *(uint32*)(inputbuf + inputlen - 4);
    if(*outputlen <= 0) return MR_FAILED;
    *outputbuf = mr_malloc(*outputlen);
    if(!*outputbuf) {
		LOGE("mr_unzip err on memory!");
		*outputlen = 0;

    	return MR_FAILED;
    }

    ret = ungzipdata(*outputbuf, (uint32 *)outputlen, inputbuf, inputlen);
    if (Z_OK != ret) { //CRC 错误
//    	LOGE("ungzipdata err, code=%d", ret);
    	return MR_FAILED;
    }

    return MR_SUCCESS;
}

/*-----------------------------------*/
// c2u

extern const uint8 *GBCodeToUnicode(const uint8 *gbCode);

uint16 *mr_c2u(char *cp, int32 *err, int32 *size)
{
    uint8 *ucp = (uint8 *)cp;
    const uint8 *pReturn;
    uint16 *uniBuf;
    uint32 uniSize;
    uint32 chCount;
    uint32 i;

   // LOGI("mr_c2u(%#p,%#p,%#p)", cp, err, size);

    if (NULL != err)
        *err = -1;

    chCount = 0;
    for (i = 0; 0 != ucp[i]; i++)
    {
        if (ucp[i] >= 0xa1 && ucp[i] <= 0xfe && 0 != ucp[i + 1])
            i++;
        chCount++;
    }

    uniSize = 2 * chCount + 2;
    if (NULL != size)
        *size = uniSize;

    uniBuf = mr_malloc(uniSize);
    if (NULL != uniBuf)
    {
        chCount = 0;
        i = 0;
        while (0 != ucp[i])
        {
            if (ucp[i] < 0x80)
            {
                uniBuf[chCount++] = ucp[i] << 8;
                i++;
            }
            else
            {
                pReturn = GBCodeToUnicode(ucp + i);
                if (NULL != pReturn)
                {
                    uniBuf[chCount++] = (pReturn[0] << 8) + pReturn[1];
                }
                else
                {
                    if (NULL != err)
                    {
                        *err = i;
                        mr_free(uniBuf, uniSize);
                        uniBuf = NULL;
                        goto end;
                    }
                    else
                    {
                        uniBuf[chCount++] = 0x3000;
                    }
                }
                i += 2;
            }
        }

        uniBuf[chCount] = 0;
    }

end:
    //LOGI("mr_c2u:%#p", uniBuf);

    return uniBuf;
}

/*-----------------------------------*/
// isMr

int32 _mr_isMr(void *buf)
{
	LOGI("_mr_isMr(%#p)", buf);
    return 0;
}
