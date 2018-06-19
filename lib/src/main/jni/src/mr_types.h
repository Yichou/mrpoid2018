#ifndef _MR_TYPES_H
#define _MR_TYPES_H



#ifndef SDK_MOD
typedef  unsigned long long  uint64;      /* Unsigned 64 bit value */
typedef  long long                int64;      /* signed 64 bit value */
#else
typedef	unsigned _int64	uint64;
typedef	 _int64	int64;
#endif

#ifndef MR_C_NUMBER_TYPE
typedef  unsigned 	short     	uint16;      /* Unsigned 16 bit value */
typedef  		 	short       int16;       /* Signed 16 bit value */

typedef  unsigned 	int  		uint32;      /* Unsigned 32 bit value */
typedef  			int         int32;      /* signed 32 bit value */

typedef  unsigned 	char      	uint8;        /*Unsigned  Signed 8  bit value */
typedef  signed 	char        int8;        /* Signed 8  bit value */
#endif

typedef char *			PSTR;
typedef const char *	PCSTR;
typedef uint8			U8;
typedef uint64			U64;

typedef unsigned int	UINT;
typedef unsigned long	DWORD;
typedef unsigned char	BYTE;
typedef DWORD *			DWORD_PTR;

typedef int BOOL;

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef NULL 
#define NULL (void*)0
#endif

//typedef long int size_t;

#define LOBYTE(w) ((BYTE)((DWORD_PTR)(w) & 0xff))


#define MR_SUCCESS  0    //成功
#define MR_FAILED   -1    //失败
#define MR_IGNORE   1     //不关心
#define MR_WAITING   2     //异步(非阻塞)模式



#endif
