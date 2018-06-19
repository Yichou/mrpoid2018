
#ifndef mr_h
#define mr_h


#ifndef BREW_MOD
#include <stdarg.h>
#include <stddef.h>
#include <string.h>
#endif

#ifdef PC_MOD
#include <stdio.h>
#endif


#ifdef BREW_MOD
#include <stdarg.h>
//#include <stddef.h>//ouli brew
#include "AEEStdLib.h"//ouli brew
#endif


#ifdef MR_START_FILE
#undef MR_START_FILE
#define MR_START_FILE "in.ct"
#define MR_ERROR_WAP "http://wap.flymobi.net/vmMain.jsp"
#else
#define MR_START_FILE "start.mr"
#define MR_ERROR_WAP "http://wap.skmeg.com/dsmWap/error.jsp"
#endif


#define MR_OFFSET_FORM_FILEHEAD 16

#define COMPATIBILITY01

#ifdef SDK_MOD
#define MR_MAX_FILENAME_SIZE		128
#else
#define MR_MAX_FILENAME_SIZE		128
#endif
#define MR_MAX_FILE_SIZE		    0x7FFFFFFF


#define MRP_L_BUFFERSIZE		256
//ouli brew

/*
#define STRTOL(n,e,b)		STRTOD(e,b)
#define STRCSPN(a,b)		    (unsigned int)(STRSTR(a,b) - a)
#define STRNCAT(a,b,c)		    STRLCAT(a,b,c+STRLEN(a))
#define STRCOLL		    STRCMP
#define STRPBRK(a,b)		   ((STRCHRSEND(a,b) == a+STRLEN(a))? NULL:STRCHRSEND(a,b))
*/


//#ifdef MR_MSTAR_MOD
//#define strncpy mr_strncpy
//extern char *mr_strncpy(char * s1, const char * s2, int n);
//#endif


#ifdef MR_SPREADTRUM_MOD
#define SETJMP mr_setjmp
#define LONGJMP mr_longjmp
#else
#define SETJMP setjmp
#define LONGJMP longjmp
#endif

#if (defined(MR_ANYKA_MOD)||defined(MR_SPREADTRUM_MOD)||defined(MR_BREW_MOD))

/*
#define ISDIGIT mr_isdigit
#define ISXDIGIT mr_isxdigit
#define ISALPHA mr_isalpha
#define ISLOWER mr_islower
#define ISSPACE mr_isspace
*/
int mr_isdigit(int ch);
int mr_isxdigit(int ch);
int mr_isalpha(int ch);
int mr_islower(int ch);
int mr_isspace(int ch);


#undef isdigit 
#undef isxdigit 
#undef isalpha 
#undef islower 
#undef isspace 

#define isdigit mr_isdigit
#define isxdigit mr_isxdigit
#define isalpha mr_isalpha
#define islower mr_islower
#define isspace mr_isspace

extern int atoi( const char *string );
extern unsigned long strtoul(const char *nptr, char **endptr, int base);
extern int rand(void); 
extern long strtol(const char *nptr, char **endptr, int base);
extern int abs(int x); 

extern int mr_toupper(int ch); 
extern int mr_tolower(int ch); 
extern int mr_iscntrl(int ch); 
extern int mr_ispunct(int ch); 
extern int mr_isupper(int ch); 
extern int mr_isalnum(int ch); 

#define toupper mr_toupper
#define tolower mr_tolower
#define iscntrl mr_iscntrl
#define ispunct mr_ispunct
#define isupper mr_isupper
#define isalnum mr_isalnum

#else

/*
#define ISDIGIT isdigit
#define ISXDIGIT isxdigit
#define ISALPHA isalpha
#define ISLOWER islower
#define ISSPACE isspace
*/

#endif



#ifdef MR_BIG_ENDIAN
#define ntohl(i) (((uint32)i>>24) | (((uint32)i&0xff0000)>>8) | (((uint32)i&0xff00)<<8) | ((uint32)i<<24))
#define htonl(i) (i)
#define ntohs(i) ((((uint16)i)>>8) | ((uint16)i<<8))
#else
#define ntohl(i) (i)
#define htonl(i) (((uint32)i>>24) | (((uint32)i&0xff0000)>>8) | (((uint32)i&0xff00)<<8) | ((uint32)i<<24))
#define ntohs(i) (i)
#endif

#define mr_htonl(i) (((uint32)i>>24) | (((uint32)i&0xff0000)>>8) | (((uint32)i&0xff00)<<8) | ((uint32)i<<24))



#ifdef MR_DEBUG
#define LUADBGPRINTF(a)    MRDBGPRINTF(a)
#define LUADBGPRINTF1(a, b)    MRDBGPRINTF(a, b)
#define LUADBGPRINTF2(a, b, c)    MRDBGPRINTF(a,b,c)
//#define LUADBGPRINTF1(a)    //MRDBGPRINTF
#else
#define LUADBGPRINTF(a)    //MRDBGPRINTF(a)
#define LUADBGPRINTF1(a, b)    //MRDBGPRINTF
#define LUADBGPRINTF2(a, b, c)    //MRDBGPRINTF
//#define LUADBGPRINTF1(a)    //MRDBGPRINTF
#endif





typedef  unsigned short     uint16;      /* Unsigned 16 bit value */
typedef  unsigned long int  uint32;      /* Unsigned 32 bit value */
typedef  long int                int32;      /* signed 32 bit value */
typedef  unsigned char      uint8;        /*Unsigned  Signed 8  bit value */
typedef  signed char          int8;        /* Signed 8  bit value */
typedef  signed short         int16;       /* Signed 16 bit value */
#define MR_C_NUMBER_TYPE


#include "mr_forvm.h"
//#define FREE_MEMORY mfwAlloc
//#define ALLOC_MEMORY mfwFree


//////////////////////////////////////////////
enum {
   MR_STATE_IDLE,
   MR_STATE_RUN,
   MR_STATE_PAUSE,
   MR_STATE_RESTART,
   MR_STATE_STOP,
   MR_STATE_ERROR
};

enum {
   MR_TIMER_STATE_IDLE,
   MR_TIMER_STATE_RUNNING,
   MR_TIMER_STATE_SUSPENDED,
   MR_TIMER_STATE_ERROR
};

extern int32 mr_state;
//////////////////////////////////////////////



/*内存申请*/
extern void* mr_malloc(uint32 len);
/*内存释放*/
extern void mr_free(void* p, uint32 len);
/*内存重新分配*/
extern void* mr_realloc(void* p, uint32 oldlen, uint32 len);

#define MRDBGPRINTF mr_printf
#ifndef SDK_MOD
#define MR_MALLOC(a) mr_malloc(a)//malloc(a)//mr_malloc(a)
#define MR_FREE(a, b) mr_free(a, b)//free(a)//mr_free(a, b)
#define MR_REALLOC(b,os,s)  mr_realloc(b,os,s)  //realloc(b, s)//mr_realloc(b,os,s)  
#else
#define MR_MALLOC(a) mr_malloc(a)//malloc(a)//mr_malloc(a)
#define MR_FREE(a, b) mr_free(a, b)//free(a)//mr_free(a, b)
#define MR_REALLOC(b,os,s)  mr_realloc(b,os,s)//realloc(b, s)//mr_realloc(b,os,s)  
#endif






#if defined(MR_BREW_MOD)

typedef  unsigned long int  AECHAR;      /* Unsigned 32 bit value */
typedef  unsigned long int  boolean;      /* Unsigned 32 bit value */
typedef  unsigned long int  byte;      /* Unsigned 32 bit value */
typedef  unsigned long int  TChType;      /* Unsigned 32 bit value */
typedef  unsigned long int  IApplet;      /* Unsigned 32 bit value */
typedef  unsigned long int  IQueryInterface;      /* Unsigned 32 bit value */
typedef  unsigned long int  AEECLSID;      /* Unsigned 32 bit value */
typedef  unsigned long int  AEEImageInfo;      /* Unsigned 32 bit value */
typedef  unsigned long int  JulianType;      /* Unsigned 32 bit value */
typedef  unsigned long int  AEEOldVaList;      /* Unsigned 32 bit value */
typedef  unsigned long int  AEEEvent;      /* Unsigned 32 bit value */
typedef  unsigned long int  IModule;      /* Unsigned 32 bit value */
typedef  unsigned long int  IShell;      /* Unsigned 32 bit value */



typedef int (*PFNQSORTCOMPARE)(const void *s1, const void *s2);

typedef struct AEEHelperFuncs AEEHelperFuncs;

struct AEEHelperFuncs
{
   void          *(*memmove)(void *pd, const void *ps, size_t s);
   void          *(*memset)(void *pd, int c, size_t s);

   // Standard String Functions...
   char          *(*strcpy)(char *dst, const char *src);
   char          *(*strcat)(char *dst, const char *src);
   int            (*strcmp)(const char *s1, const char *s2);
   size_t         (*strlen)(const char *s);
   char          *(*strchr)(const char *s1, int ch);
   char          *(*strrchr)(const char *s1, int ch);
   int            (*sprintf)(char *pszDest,const char *pszFormat, ...);

   // Wide String Functions...

   AECHAR        *(*wstrcpy)(AECHAR *pDest, const AECHAR *pSrc);
   AECHAR        *(*wstrcat)(AECHAR *pDest, const AECHAR *pSrc);
   int            (*wstrcmp)(const AECHAR *s1, const AECHAR *s2);
   int            (*wstrlen)(const AECHAR *p);
   AECHAR        *(*wstrchr)(const AECHAR *s1, AECHAR ch);
   AECHAR        *(*wstrrchr)(const AECHAR *s1, AECHAR ch);
   void           (*wsprintf)(AECHAR *pDest, int nSize, const AECHAR *pFormat,
                              ...);

   // String Conversions...

   AECHAR         *(*strtowstr)(const char *pszIn, AECHAR *pDest, int nSize);
   char           *(*wstrtostr)(const AECHAR *pIn, char *pszDest, int nSize);
   double          (*wstrtofloat)(const AECHAR *pszNum);
   boolean         (*floattowstr)(double val, AECHAR *psz, int nSize);
   boolean         (*utf8towstr)(const byte *pszIn, int nLen, AECHAR *pszDest,
                                 int nSizeDestBytes);
   boolean         (*wstrtoutf8)(const AECHAR *pszIn, int nLen, byte *pszDest, 
                                 int nSizeDestBytes);
   void            (*wstrlower)(AECHAR *pszDest);
   void            (*wstrupper)(AECHAR *pszDest);
   TChType         (*chartype)(AECHAR ch);

   // Bitmap Conversions...

   void           *(*SetupNativeImage)(AEECLSID cls, void *pBuffer, 
                                       AEEImageInfo *pii, boolean *pbRealloc);

   // Memory allocation routines
   
   void          *(*malloc)(uint32 dwSize);
   void           (*free)(void *pObj);
   AECHAR        *(*wstrdup)(const AECHAR *pIn);
   void          *(*realloc)(void *pSrc, uint32 dwSize);
   
   // Additional Wide String Helpers

   AECHAR        *(*wwritelongex)(AECHAR *pszBuf, long n, int nPad, 
                                  int *pnRemaining);
   int            (*wstrsize)(const AECHAR *p);
   int            (*wstrncopyn)(AECHAR *pszDest, int cbDest, 
                                const AECHAR *pszSource, int lenSource);
   
   // OEM String Routines

   int            (*OEMStrLen)(const byte *p);
   int            (*OEMStrSize)(const byte *p);

   // AEE/BREW version, See above for flags

   uint32         (*GetAEEVersion)(byte *pszFormatted, int nSize, 
                                   uint16 wFlags);  

   // ATOI
   
   int            (*atoi)(const char *psz);

   // Floating point - covers issues related to floating point usage across 
   //  ARM/THUMB

   double         (*f_op)(double v1, double v2,int nType);
   boolean        (*f_cmp)(double v1, double v2,int nType);

   // Debug 

   void           (*dbgprintf)(const char *psz,...);
   void           (*wstrcompress)(const AECHAR *pIn, int nChars, byte *pDest,int nSize);

   // time

   int32          (*LocalTimeOffset)(boolean *pbDaylightSavings);
   void           (*GetRand)(byte *pDest, int nSize);
   uint32         (*GetTimeMS)(void); 
   uint32         (*GetUpTimeMS)(void);
   uint32         (*GetSeconds)(void);
   void           (*GetJulianDate)(uint32 dwSecs, JulianType *pDate);

   void           (*sysfree)(void *pb);

   // Applet helper function - current applet

   IApplet       *(*GetAppInstance)(void);

   uint32         (*strtoul)(const char *nptr, char **endptr, int base );
   char          *(*strncpy)(char *strDest, const char *strSource,int32 count );
   int            (*strncmp)(const char *a, const char *b, size_t length );
   int            (*stricmp)(const char *a, const char *b);
   int            (*strnicmp)(const char *a, const char *b, size_t length);
   char          *(*strstr)(const char *haystack, const char *needle);
   int            (*memcmp)(const void *a, const void *b, size_t length );
   void          *(*memchr)(const void *src, int c, size_t length );
   void           (*strexpand)(const byte *pSrc, int nCount, AECHAR *pDest,int nSize);
   char          *(*stristr)(const char *cpszHaystack, const char *cpszNeedle);
   char          *(*memstr)(const char *cpHaystack, const char *cpszNeedle, size_t nHaystackLen);
   int            (*wstrncmp)(const AECHAR *s1, const AECHAR *s2, size_t nLen);
   char          *(*strdup)(const char *psz);
   boolean        (*strbegins)(const char *cpszPrefix, const char *psz);
   boolean        (*strends)(const char *cpszSuffic, const char *psz);
   char          *(*strchrend)(const char *pszSrc, char c);
   char          *(*strchrsend)(const char *pszSrc, const char *pszChars);
   char          *(*memrchr)(const char *pcSrch, int c, size_t nLen);
   char          *(*memchrend)(const char *pcSrch, int c, size_t nLen);
   char          *(*memrchrbegin)(const char *pcSrch, int c, size_t nLen);
   char          *(*strlower)(char *psz);
   char          *(*strupper)(char *psz);
   int            (*wstricmp)(const AECHAR * p1, const AECHAR * p2);
   int            (*wstrnicmp)(const AECHAR * p1, const AECHAR * p2, size_t length);
   boolean        (*inet_aton)(const char *pc, uint32 *pulAddr);
   void           (*inet_ntoa)(uint32 ulAddr, char *pc, size_t nLen);
   uint32         (*swapl)(uint32 ul);
   uint16         (*swaps)(uint16 us);

   // Heap/File query functions.

   uint32         (*GetFSFree)(uint32 * pdwTotal);
   uint32         (*GetRAMFree)(uint32 * pdwTotal,uint32 * pdwLargest);

   int            (*vsprintf)(char * pszDest,const char * pszFormat, va_list arg);
   int32          (*vsnprintf)(char *buf, uint32 f, const char *format, AEEOldVaList list);
   int32          (*snprintf)(char *buf, uint32 f, const char *format, ...);
	
	// Added for 2.0
   uint32         (*JulianToSeconds)(JulianType* pDate);
   size_t         (*strlcpy)(char * dst, const char * src, size_t nSize);
   size_t         (*strlcat)(char * dst, const char * src, size_t nSize);
   size_t         (*wstrlcpy)(AECHAR * dst, const AECHAR * src, size_t nSize);
   size_t         (*wstrlcat)(AECHAR * dst, const AECHAR * src, size_t nSize);
   void          *(*setstaticptr)(int nSPId, void *pNew);
   
   //More floating point operations
   double         (*f_assignstr)(const char *pszFloat);
   double         (*f_assignint)(int32 val);
   AECHAR *       (*wwritelong)(AECHAR *pszBuf, long n);
   void          *(*dbgheapmark)(void *ptr, const char * pszFile, int nLine);
   int            (*lockmem)(void ** ppHandle);
   int            (*unlockmem)(void ** ppHandle);
   void           (*dumpheap)(void);
   double         (*strtod)(const char *pszFloat, char ** ppszEnd);
   double         (*f_calc)(double x, int calcType);
   void           (*sleep)(uint32 msecs);
   int            (*getlasterror)(int errRegion);
   double         (*wgs84_to_degrees)(int32 latlon);
   void           (*dbgevent)(AEEEvent evt, AEECLSID cls, uint32 pl);
   boolean        (*AEEOS_IsBadPtr)(int chkType, void *pBuf, uint32 len);
   char          *(*aee_basename)(const char *cpszPath);
   int            (*aee_makepath)(const char *cpszDir, const char *cpszFile,
                                  char *pszOut, int *pnOutLen);
   char          *(*aee_splitpath)(const char *cpszPath, const char *cpszDir);   
   boolean        (*aee_stribegins)(const char *cpszPrefix, const char *psz);
   uint32         (*GetUTCSeconds)(void);
   int            (*f_toint)(double val);
   double         (*f_get)(uint32 fgetType);
   void           (*qsort)(void *base, size_t nmemb, size_t size, PFNQSORTCOMPARE pfn);
   int32          (*trunc)(double x);
   uint32         (*utrunc)(double x);
   int            (*err_realloc)(uint32 uSize, void **pp);
   int            (*err_strdup)(const char *pszSrc, char **pp);
   
   // Utility functions for IPV6.
   int            (*inet_pton)(int af, const char *src, void *dst);   
   const char    *(*inet_ntop)(int af, const void *src, char *dst, size_t size);
   IQueryInterface *(*GetALSContext)(void);
};



extern int AEEMod_Load(IShell *ps, void * pszRes, IModule ** pm);

/*lint -emacro(611,GET_HELPER) doesn't like cast from function ptr*/
/*lint -emacro(740,GET_HELPER) doesn't like cast from function ptr*/
#define GET_HELPER()      (*(((AEEHelperFuncs **)AEEMod_Load) - 1))
#define GET_HELPER_VER()  (*((uint32 *)(((byte *)AEEMod_Load) - sizeof(AEEHelperFuncs *) - sizeof(uint32))))

#if 0
#define MEMMOVE                     GET_HELPER()->memmove
#define MEMCPY                    MEMMOVE
#define MEMSET                      GET_HELPER()->memset
#define MEMCMP                      GET_HELPER()->memcmp
#define MEMCHR                      GET_HELPER()->memchr
#define MEMSTR                      GET_HELPER()->memstr
#define MEMRCHR                     GET_HELPER()->memrchr
#define MEMCHREND                   GET_HELPER()->memchrend
#define MEMRCHRBEGIN                GET_HELPER()->memrchrbegin
#define MSLEEP                      GET_HELPER()->sleep
#define STRCPY                       GET_HELPER()->strcpy
#define STRNCPY(dest,src,count)        GET_HELPER()->strncpy((dest),(char*)(src),(count))
#define STRNCMP                     GET_HELPER()->strncmp
#define STRICMP                     GET_HELPER()->stricmp
#define STRNICMP                    GET_HELPER()->strnicmp
#define STRCAT                      GET_HELPER()->strcat
#define STRCMP                      GET_HELPER()->strcmp
#define STRCOLL		    STRCMP
#define STRLEN                      GET_HELPER()->strlen
#define STRNLEN(s1, n)                  ((GET_HELPER()->strlen((char*)(s1))>n)? n:strlen((char*)(s1)))
#define STRCHR                      GET_HELPER()->strchr
#define STRCHREND                   GET_HELPER()->strchrend
#define STRCHRSEND                  GET_HELPER()->strchrsend
#define STRRCHR                     GET_HELPER()->strrchr
#define STRSTR                      GET_HELPER()->strstr
#define STRISTR                     GET_HELPER()->stristr
#define STRBEGINS                   GET_HELPER()->strbegins
#define STRIBEGINS                  GET_HELPER()->aee_stribegins
#define STRENDS                     GET_HELPER()->strends
#define STRLOWER                    GET_HELPER()->strlower
#define STRUPPER                    GET_HELPER()->strupper
#define SPRINTF                     GET_HELPER()->sprintf
#define SNPRINTF                    GET_HELPER()->snprintf
#define STRTOUL                     GET_HELPER()->strtoul
#define STRTOD                      GET_HELPER()->strtod
#define STRLCPY                     GET_HELPER()->strlcpy
#define STRLCAT                     GET_HELPER()->strlcat
#define WSTRCPY                     GET_HELPER()->wstrcpy
#define WSTRCAT                     GET_HELPER()->wstrcat
#define WSTRCMP                     GET_HELPER()->wstrcmp
#define WSTRNCMP                    GET_HELPER()->wstrncmp
#define WSTRICMP                    GET_HELPER()->wstricmp
#define WSTRNICMP                   GET_HELPER()->wstrnicmp
#define WSTRLEN                     GET_HELPER()->wstrlen
#define WSTRCHR                     GET_HELPER()->wstrchr
#define WSTRRCHR                    GET_HELPER()->wstrrchr
#define WSPRINTF                    GET_HELPER()->wsprintf
#define STRTOWSTR                   GET_HELPER()->strtowstr
#define WSTRTOSTR                   GET_HELPER()->wstrtostr
#define WSTRTOFLOAT                 GET_HELPER()->wstrtofloat
#define FLOATTOWSTR                 GET_HELPER()->floattowstr
#define UTF8TOWSTR                  GET_HELPER()->utf8towstr
#define WSTRTOUTF8                  GET_HELPER()->wstrtoutf8
#define WSTRLOWER                   GET_HELPER()->wstrlower
#define WSTRUPPER                   GET_HELPER()->wstrupper
#define WSTRLCPY                    GET_HELPER()->wstrlcpy
#define WSTRLCAT                    GET_HELPER()->wstrlcat
#define GETCHTYPE                   GET_HELPER()->chartype
#define ATOI                        GET_HELPER()->atoi
#define  MAX( x, y ) ( ((x) > (y)) ? (x) : (y) )
#define  MIN( x, y ) ( ((x) < (y)) ? (x) : (y) )





#define STRCSPN(a,b)		    (unsigned int)(STRSTR(a,b) - a)
#define STRNCAT(a,b,c)		    STRLCAT(a,b,c+STRLEN(a))
#define STRCOLL		    STRCMP
#define STRPBRK(a,b)		   ((STRCHRSEND(a,b) == a+STRLEN(a))? NULL:STRCHRSEND(a,b))


#define STRTOL(n,e,b)		STRTOD((const char*)n,(char**)e)

#ifndef ABS
   #define ABS(VAL) (((VAL)>0)?(VAL):(-(VAL)))
#endif
#endif


#endif         //#if (defined(BREW_MOD) || defined(MR_BREW_MOD))



#define STRCSPN		    strcspn
#define STRNCAT		    strncat
#define STRPBRK		   strpbrk


#define ABS abs
//#define REALLOC(a, s) MR_FREE(a),MALLOC(s)  //ouli TI

#ifdef UNDER_CE 
#define STRCOLL		    strcmp
#else
#ifdef MTK_MOD
#define STRCOLL		    STRCMP
#else
#define STRCOLL		    strcoll
#endif
#endif



#define MEMCPY(dest,src,size)       memcpy((dest),(src),(size))
#define MEMMOVE(dest,src,size)      memmove((dest),(src),(size))
#define MEMSET(dest,ch,size)        memset((dest),(ch),(size))
#define MEMCMP(a,b,size)            memcmp((a),(b),(size))
#define MEMCHR(s,c,size)            memchr((s),(c),(size))
#define MEMSTR(h,n,hl)              memstr((h),(n),(hl))
#define MEMRCHR(s,c,sl)             memrchr((s),(c),(sl))
#define MEMCHREND(s,c,sl)           memchrend((s),(c),(sl))
#define MEMRCHRBEGIN(s,c,sl)        memrchrbegin((s),(c),(sl))
//#define MSLEEP(n)                   sleep((n))
#define STRCPY(dest,src)            strcpy((dest),(src))
#define STRNCPY(dest,src,count)     strncpy((dest),(char*)(src),(count))
#define STRNCMP(a,b,count)          strncmp((a),(b),(count))
#define STRICMP(a, b)               stricmp((a),(b))
#define STRNICMP(a,b,count)         strnicmp((a),(b),(count))
#define STRCAT(dest,src)            strcat((dest),(src))
#define STRCMP(s1,s2)               strcmp((s1),(s2))
#define STRLEN(s1)                  strlen((char*)(s1))
#define STRNLEN(s1, n)                  ((strlen((char*)(s1))>n)? n:strlen((char*)(s1)))
#define STRCHR(s1,ch)               strchr((s1),(ch))
#define STRCHREND(s1,ch)            strchrend((s1),(ch))
#define STRCHRSEND(s,cs)            strchrsend((s),(cs))
#define STRRCHR(s1,ch)              strrchr((s1),(ch))
#define STRSTR(h,n)                 strstr((h),(n))
#define STRISTR(h,n)                stristr((h),(n))
#define STRBEGINS(p,s)              strbegins((p),(s))
#define STRENDS(p,s)                strends((p),(s))
#define STRLOWER(s)                 strlower((s))
#define STRUPPER(s)                 strupper((s))
#define SNPRINTF                    snprintf

#if !defined(MR_BREW_MOD)
#define SPRINTF                     sprintf
#define VSPRINTF                    vsprintf

#else //if !defined(MR_BREW_MOD)

#if !defined(MR_BREW_OTA_MOD)
#define VSPRINTF                    vsprintf
extern int           aee_sprintf(char * pszDest,const char * pszFormat, ...);
#define SPRINTF                     aee_sprintf
#else  //if !defined(MR_BREW_OTA_MOD)

#define VSPRINTF                    GET_HELPER()->vsprintf

//static __inline int VSPRINTF(char *buf, const char *fmt, va_list arg)
//{
//   return GET_HELPER()->vsprintf(buf, fmt, AEEOldVaList_From_va_list((va_list *)&(arg)));
//}

extern int           mrp_sprintf(char * pszDest,const char * pszFormat, ...);
#define SPRINTF                     mrp_sprintf

#endif

#endif// #if !defined(MR_BREW_MOD)

#define STRTOUL(s1,s2,n)            strtoul((s1),(s2),(n))
#define STRTOD(s,ps)                strtod((s),(ps))
#define STRLCPY(d,s,n)              strlcpy((d),(s),(n))
#define STRLCAT(d,s,n)              strlcat((d),(s),(n))
#define WSTRCPY(d,s)                wstrcpy((d),(s))
#define WSTRCAT(d,s)                wstrcat((d),(s))
#define WSTRCMP(s1,s2)              wstrcmp((s1),(s2))
#define WSTRNCMP(s1,s2,n)           wstrncmp((s1),(s2),(n))
#define WSTRICMP(s1,s2)             wstricmp((s1),(s2))
#define WSTRNICMP(s1,s2,n)          wstrnicmp((s1),(s2),(n))
#define WSTRLEN(s1)                 wstrlen((s1))
#define WSTRCHR(s1,ch)              wstrchr((s1),(ch))
#define WSTRRCHR(s1,ch)             wstrrchr((s1),(ch))
#define WSPRINTF                    wsprintf
#define VSNPRINTF(b,l,f,r)          vsnprintf((b),(l),(f),(r))
#define STRTOWSTR(src,dest,size)    strtowstr((src),(dest),(size))
#define WSTRTOSTR(src,dest,size)    wstrtostr((src),(dest),(size))
#define WSTRTOFLOAT(src)            wstrtofloat((src))
#define FLOATTOWSTR(v,d,dl)         floattowstr((v),(d),(dl))
#define UTF8TOWSTR(in,len,dest,s)   utf8towstr((in),(len),(dest),(s))
#define WSTRTOUTF8(in,len,dest,s)   wstrtoutf8((in),(len),(dest),(s))
#define WSTRLOWER(dest)             wstrlower((dest))
#define WSTRUPPER(dest)             wstrupper((dest))
#define WSTRLCPY(d,s,n)             wstrlcpy((d),(s),(n))
#define WSTRLCAT(d,s,n)             wstrlcat((d),(s),(n))
#define GETCHTYPE(ch)               chartype((ch))
#define ATOI(psz)                   atoi((psz))
#define WSTRCOMPRESS(ps,ns,pd,nd)   wstrcompress((ps),(ns),(pd),(nd))
#define STREXPAND(ps,ns,pd,nd)      strexpand((ps),(ns),(pd),(nd))

#define  MAX( x, y ) ( ((x) > (y)) ? (x) : (y) )
#define  MIN( x, y ) ( ((x) < (y)) ? (x) : (y) )
#ifndef ABS
   #define ABS(VAL) (((VAL)>0)?(VAL):(-(VAL)))
#endif
//#define LOCALTIMEOFFSET(pb)         LocalTimeOffset((pb))
//#define GETAPPINSTANCE()            GetAppInstance()


#define STRTOL		strtol




#ifdef BREW_MOD
#define MR_MALLOC(a) MALLOC(a)
#define MR_FREE(a, b) FREE(a)
#define MR_REALLOC(b,os,s)  REALLOC(b,s)  //ouli TI

#define MRDBGPRINTF DBGPRINTF
#endif


//#ifdef MR_V2000
//#define MR_VERSION	2009     //升级版本前进行版本备份
//#else
//#define MR_VERSION	 1966     //升级版本前进行版本备份
//#endif

#define MR_COPYRIGHT	"Copyright (C) "
#define MR_AUTHORS 	" "



/* option for multiple returns in `mrp_pcall' and `mrp_call' */
#define MRP_MULTRET	(-1)


/*
** pseudo-indices
*/
#define MRP_REGISTRYINDEX	(-10000)
#define MRP_GLOBALSINDEX	(-10001)
#define mrp_upvalueindex(i)	(MRP_GLOBALSINDEX-(i))


/* error codes for `mrp_load' and `mrp_pcall' */
#define MRP_ERRRUN	1
#define MRP_ERRFILE	2
#define MRP_ERRSYNTAX	3
#define MRP_ERRMEM	4
#define MRP_ERRERR	5


typedef struct mrp_State mrp_State;

typedef int (*mrp_CFunction) (mrp_State *L);


/*
** functions that read/write blocks when loading/dumping Lua chunks
*/
typedef const char * (*mrp_Chunkreader) (mrp_State *L, void *ud, size_t *sz);

typedef int (*mrp_Chunkwriter) (mrp_State *L, const void* p,
                                size_t sz, void* ud);


/*
** basic types
*/
#define MRP_TNONE	(-1)

#define MRP_TNIL	0
#define MRP_TBOOLEAN	1
#define MRP_TLIGHTUSERDATA	2
#define MRP_TNUMBER	3
#define MRP_TSTRING	4
#define MRP_TTABLE	5
#define MRP_TFUNCTION	6
#define MRP_TUSERDATA	7
#define MRP_TTHREAD	8


/* minimum Lua stack available to a C function */
#define MRP_MINSTACK	20


/*
** generic extra include file
*/
#define USE_INT

#ifndef MR_V2000
#define MRP_USER_H "../src/mr_user_number.h"
#ifdef MRP_USER_H
#include MRP_USER_H
#endif
#endif //#ifndef MR_V2000


/* type of numbers in Lua */
#ifndef MRP_NUMBER
typedef double mrp_Number;
#else
typedef MRP_NUMBER mrp_Number;
#endif


/* mark for all API functions */
#ifndef MRP_API
#define MRP_API		extern
#endif


/*
** state manipulation
*/
MRP_API mrp_State *mrp_open (void);
MRP_API void       mrp_close (mrp_State *L);
MRP_API mrp_State *mrp_newthread (mrp_State *L);

MRP_API mrp_CFunction mrp_atpanic (mrp_State *L, mrp_CFunction panicf);


/*
** basic stack manipulation
*/
MRP_API int   mrp_gettop (mrp_State *L);
MRP_API void  mrp_settop (mrp_State *L, int idx);
MRP_API void  mrp_pushvalue (mrp_State *L, int idx);
MRP_API void  mrp_remove (mrp_State *L, int idx);
MRP_API void  mrp_insert (mrp_State *L, int idx);
MRP_API void  mrp_replace (mrp_State *L, int idx);
MRP_API int   mrp_checkstack (mrp_State *L, int sz);

MRP_API void  mrp_xmove (mrp_State *from, mrp_State *to, int n);


/*
** access functions (stack -> C)
*/

MRP_API int             mrp_isnumber (mrp_State *L, int idx);
MRP_API int             mrp_isstring (mrp_State *L, int idx);
MRP_API int             mrp_iscfunction (mrp_State *L, int idx);
MRP_API int             mrp_isuserdata (mrp_State *L, int idx);
MRP_API int             mrp_type (mrp_State *L, int idx);
MRP_API const char     *mrp_typename (mrp_State *L, int tp);
MRP_API const char     *mrp_shorttypename (mrp_State *L, int tp);


MRP_API int            mrp_equal (mrp_State *L, int idx1, int idx2);
MRP_API int            mrp_rawequal (mrp_State *L, int idx1, int idx2);
MRP_API int            mrp_lessthan (mrp_State *L, int idx1, int idx2);

MRP_API mrp_Number      mrp_tonumber (mrp_State *L, int idx);
MRP_API int             mrp_toboolean (mrp_State *L, int idx);
MRP_API const char     *mrp_tostring (mrp_State *L, int idx);
MRP_API size_t          mrp_strlen (mrp_State *L, int idx);
MRP_API const char     *mrp_tostring_t (mrp_State *L, int idx);
MRP_API size_t          mrp_strlen_t (mrp_State *L, int idx);
MRP_API mrp_CFunction   mrp_tocfunction (mrp_State *L, int idx);
MRP_API void	       *mrp_touserdata (mrp_State *L, int idx);
MRP_API mrp_State      *mrp_tothread (mrp_State *L, int idx);
MRP_API const void     *mrp_topointer (mrp_State *L, int idx);


/*
** push functions (C -> stack)
*/
MRP_API void  mrp_pushnil (mrp_State *L);
MRP_API void  mrp_pushnumber (mrp_State *L, mrp_Number n);
MRP_API void  mrp_pushlstring (mrp_State *L, const char *s, size_t l);
MRP_API void  mrp_pushstring (mrp_State *L, const char *s);
MRP_API const char *mrp_pushvfstring (mrp_State *L, const char *fmt,
                                                    va_list argp);
MRP_API const char *mrp_pushfstring (mrp_State *L, const char *fmt, ...);
MRP_API void  mrp_pushcclosure (mrp_State *L, mrp_CFunction fn, int n);
MRP_API void  mrp_pushboolean (mrp_State *L, int b);
MRP_API void  mrp_pushlightuserdata (mrp_State *L, void *p);


/*
** get functions (Lua -> stack)
*/
MRP_API void  mrp_gettable (mrp_State *L, int idx);
MRP_API void  mrp_rawget (mrp_State *L, int idx);
MRP_API void  mrp_rawgeti (mrp_State *L, int idx, int n);
MRP_API void  mrp_newtable (mrp_State *L);
MRP_API void *mrp_newuserdata (mrp_State *L, size_t sz);
MRP_API int   mrp_getmetatable (mrp_State *L, int objindex);
MRP_API void  mrp_getfenv (mrp_State *L, int idx);


/*
** set functions (stack -> Lua)
*/
MRP_API void  mrp_settable (mrp_State *L, int idx);
MRP_API void  mrp_rawset (mrp_State *L, int idx);
MRP_API void  mrp_rawseti (mrp_State *L, int idx, int n);
MRP_API int   mrp_setmetatable (mrp_State *L, int objindex);
MRP_API int   mrp_setfenv (mrp_State *L, int idx);


/*
** `load' and `call' functions (load and run Lua code)
*/
MRP_API void  mrp_call (mrp_State *L, int nargs, int nresults);
MRP_API int   mrp_pcall (mrp_State *L, int nargs, int nresults, int errfunc);
MRP_API int mrp_cpcall (mrp_State *L, mrp_CFunction func, void *ud);
MRP_API int   mrp_load (mrp_State *L, mrp_Chunkreader reader, void *dt,
                        const char *chunkname);

MRP_API int mrp_dump (mrp_State *L, mrp_Chunkwriter writer, void *data);


/*
** coroutine functions
*/
MRP_API int  mrp_yield (mrp_State *L, int nresults);
MRP_API int  mrp_resume (mrp_State *L, int narg);

/*
** garbage-collection functions
*/
MRP_API int   mrp_getgcthreshold (mrp_State *L);
MRP_API int   mrp_getgccount (mrp_State *L);
MRP_API void  mrp_setgcthreshold (mrp_State *L, int newthreshold);

/*
** miscellaneous functions
*/

MRP_API uint32 mrp_version (void);

MRP_API int   mrp_error (mrp_State *L);

MRP_API int   mrp_next (mrp_State *L, int idx);

MRP_API void  mrp_concat (mrp_State *L, int n);



/* 
** ===============================================================
** some useful macros
** ===============================================================
*/

#define mrp_boxpointer(L,u) \
	(*(void **)(mrp_newuserdata(L, sizeof(void *))) = (u))

#define mrp_unboxpointer(L,i)	(*(void **)(mrp_touserdata(L, i)))

#define mrp_pop(L,n)		mrp_settop(L, -(n)-1)

#define mrp_register(L,n,f) \
	(mrp_pushstring(L, n), \
	 mrp_pushcfunction(L, f), \
	 mrp_settable(L, MRP_GLOBALSINDEX))

#define mrp_pushcfunction(L,f)	mrp_pushcclosure(L, f, 0)

#define mrp_isfunction(L,n)	(mrp_type(L,n) == MRP_TFUNCTION)
#define mrp_istable(L,n)	(mrp_type(L,n) == MRP_TTABLE)
#define mrp_islightuserdata(L,n)	(mrp_type(L,n) == MRP_TLIGHTUSERDATA)
#define mrp_isnil(L,n)		(mrp_type(L,n) == MRP_TNIL)
#define mrp_isboolean(L,n)	(mrp_type(L,n) == MRP_TBOOLEAN)
#define mrp_isnone(L,n)		(mrp_type(L,n) == MRP_TNONE)
#define mrp_isnoneornil(L, n)	(mrp_type(L,n) <= 0)

#define mrp_pushliteral(L, s)	\
	mrp_pushlstring(L, "" s, (sizeof(s)/sizeof(char))-1)



/*
** compatibility macros and functions
*/


MRP_API int mrp_pushupvalues (mrp_State *L);

#define mrp_getregistry(L)	mrp_pushvalue(L, MRP_REGISTRYINDEX)
#define mrp_setglobal(L,s)	\
   (mrp_pushstring(L, s), mrp_insert(L, -2), mrp_settable(L, MRP_GLOBALSINDEX))

#define mrp_getglobal(L,s)	\
		(mrp_pushstring(L, s), mrp_gettable(L, MRP_GLOBALSINDEX))


/* compatibility with ref system */

/* pre-defined references */
#define MRP_NOREF	(-2)
#define MRP_REFNIL	(-1)

#define mrp_ref(L,lock)	((lock) ? mr_L_ref(L, MRP_REGISTRYINDEX) : \
      (mrp_pushstring(L, "unlocked references are obsolete"), mrp_error(L), 0))

#define mrp_unref(L,ref)	mr_L_unref(L, MRP_REGISTRYINDEX, (ref))

#define mrp_getref(L,ref)	mrp_rawgeti(L, MRP_REGISTRYINDEX, ref)


#ifndef api_check
#define api_check(L, o)		/*{ assert(o); }*/
#endif
#define api_incr_top(L)   {api_check(L, L->top < L->ci->top); L->top++;}

/*
** {======================================================================
** useful definitions for Lua kernel and libraries
** =======================================================================
*/

/* formats for Lua numbers */
#ifndef MRP_NUMBER_SCAN
#define MRP_NUMBER_SCAN		"%lf"
#endif

#ifndef MRP_NUMBER_FMT
#define MRP_NUMBER_FMT		"%.14g"
#endif

/* }====================================================================== */


/*
** {======================================================================
** Debug API
** =======================================================================
*/


/*
** Event codes
*/
#define MRP_HOOKCALL	0
#define MRP_HOOKRET	1
#define MRP_HOOKLINE	2
#define MRP_HOOKCOUNT	3
#define MRP_HOOKTAILRET 4


/*
** Event masks
*/
#define MRP_MASKCALL	(1 << MRP_HOOKCALL)
#define MRP_MASKRET	(1 << MRP_HOOKRET)
#define MRP_MASKLINE	(1 << MRP_HOOKLINE)
#define MRP_MASKCOUNT	(1 << MRP_HOOKCOUNT)

typedef struct mrp_Debug mrp_Debug;  /* activation record */

typedef void (*mrp_Hook) (mrp_State *L, mrp_Debug *ar);


MRP_API int mrp_getstack (mrp_State *L, int level, mrp_Debug *ar);
MRP_API int mrp_getinfo (mrp_State *L, const char *what, mrp_Debug *ar);
MRP_API const char *mrp_getlocal (mrp_State *L, const mrp_Debug *ar, int n);
MRP_API const char *mrp_setlocal (mrp_State *L, const mrp_Debug *ar, int n);
MRP_API const char *mrp_getupvalue (mrp_State *L, int funcindex, int n);
MRP_API const char *mrp_setupvalue (mrp_State *L, int funcindex, int n);

MRP_API int mrp_sethook (mrp_State *L, mrp_Hook func, int mask, int count);
MRP_API mrp_Hook mrp_gethook (mrp_State *L);
MRP_API int mrp_gethookmask (mrp_State *L);
MRP_API int mrp_gethookcount (mrp_State *L);


#define MRP_IDSIZE	60

struct mrp_Debug {
  int event;
  const char *name;	/* (n) */
  const char *namewhat;	/* (n) `global', `local', `field', `method' */
  const char *what;	/* (S) `Lua', `C', `main', `tail' */
  const char *source;	/* (S) */
  int currentline;	/* (l) */
  int nups;		/* (u) number of upvalues */
  int linedefined;	/* (S) */
  char short_src[MRP_IDSIZE]; /* (S) */
  /* private part */
  int i_ci;  /* active function */
};

/* }====================================================================== */


/******************************************************************************
* Copyright (C) 1994-2004 Tecgraf, PUC-Rio.  All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining
* a copy of this software and associated documentation files (the
* "Software"), to deal in the Software without restriction, including
* without limitation the rights to use, copy, modify, merge, publish,
* distribute, sublicense, and/or sell copies of the Software, and to
* permit persons to whom the Software is furnished to do so, subject to
* the following conditions:
*
* The above copyright notice and this permission notice shall be
* included in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
******************************************************************************/


#endif
