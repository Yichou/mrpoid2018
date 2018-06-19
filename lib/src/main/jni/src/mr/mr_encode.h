
/*如果手机平台没有定义下面的类型，将#ifdef 语句内的定义生效*/
#ifndef MR_C_NUMBER_TYPE
typedef  unsigned short        uint16;      /* Unsigned 16 bit value */
typedef  unsigned long int     uint32;      /* Unsigned 32 bit value */
typedef  long int                  int32;      /* signed 32 bit value */
typedef  unsigned char         uint8;        /*Unsigned  Signed 8  bit value */
typedef  signed char             int8;        /* Signed 8  bit value */
typedef  signed short           int16;       /* Signed 16 bit value */
#endif


#ifndef MR_FAILED
#define MR_FAILED   -1    //失败
#endif

/*
返回:
MR_FAILED  -1  表示失败
其他值       表示编码字符串长度，不包括字符串结束符\0
*/
extern int32 _mr_encode(uint8  *in, uint32 len,uint8 *out);

/*
返回:
MR_FAILED  -1  表示失败
其他值       表示解码后的数据长度
*/
extern int32 _mr_decode(uint8 *in, uint32 len, uint8 *out);

