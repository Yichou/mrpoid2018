

#include "mr.h"
#include "mythroad.h"
#include "mr_gzip.h"


extern MR_FILE_HANDLE mr_openForPlat(const char* filename,  uint32 mode);
extern int32 mr_closeForPlat(MR_FILE_HANDLE f);
extern int32 mr_readForPlat(MR_FILE_HANDLE f,void *p,uint32 l);
extern int32 mr_seekForPlat(MR_FILE_HANDLE f, int32 pos, int method);
extern void* mr_mallocForPlat(uint32 len);
extern void mr_freeForPlat(void* p, uint32 len);


extern int mr_unzipForPlat(void);

#ifndef SYMBIAN_MOD
extern uint8 *mr_gzInBuf;
extern uint8 *mr_gzOutBuf;
extern unsigned LG_gzinptr;            /* index of next byte to be processed in inbuf */
extern unsigned LG_gzoutcnt;           /* bytes in output buffer */
#else
uint8 *mr_gzInBuf;
uint8 *mr_gzOutBuf;
unsigned LG_gzinptr;            /* index of next byte to be processed in inbuf */
unsigned LG_gzoutcnt;           /* bytes in output buffer */

int32 mr_zipType;
#endif


//int32 mr_zipType;


static void _mr_readFileShowInfo(const char* filename, int32 code)
{
   MRDBGPRINTF( "read file  \"%s\" err, code=%d", filename, code);
}


void * _mr_readFileForPlat(const char* mrpname, const char* filename, int *filelen, int lookfor)
{
   int ret;
   int method;
   uint32 reallen,found=0;
   int32 oldlen,nTmp;
   uint32 len;
   void* filebuf;
   MR_FILE_HANDLE f;
   char TempName[MR_MAX_FILENAME_SIZE];
   char* mr_m0_file;
   int is_rom_file = FALSE;

      f = mr_openForPlat(mrpname, MR_FILE_RDONLY );
      if (f == 0)
      {
         //MRDBGPRINTF( "file  \"%s\" can not be opened!", filename);
         _mr_readFileShowInfo(filename, 2002);
         return 0;
      }
      
      // 从这里开始是新版的mrp处理
            {
               uint32 headbuf[4];
               MEMSET(headbuf, 0, sizeof(headbuf));
               nTmp = mr_readForPlat(f, &headbuf, sizeof(headbuf));
#ifdef MR_BIG_ENDIAN
               headbuf[0] = ntohl(headbuf[0]);
               headbuf[1] = ntohl(headbuf[1]);
               headbuf[2] = ntohl(headbuf[2]);
               headbuf[3] = ntohl(headbuf[3]);
#endif
               if( (nTmp != 16)||(headbuf[0] != 1196446285))
               {
                   mr_closeForPlat(f);
                   _mr_readFileShowInfo(filename, 3001);
                   return 0;
               }
               if(headbuf[1] > 232){                             //新版mrp 
                  uint32 indexlen = headbuf[1] + 8 - headbuf[3];
                  uint8* indexbuf = mr_mallocForPlat(indexlen);
                  uint32 pos = 0;
                  uint32 file_pos,file_len;
                  if(!indexbuf){
                     mr_closeForPlat(f);
                     _mr_readFileShowInfo(filename, 3003);
                     return 0;
                  }
                  nTmp = mr_seekForPlat(f, headbuf[3] - 16, MR_SEEK_CUR);
                  if (nTmp < 0)
                  {
                     mr_closeForPlat(f);
                     mr_freeForPlat(indexbuf, indexlen);
                     _mr_readFileShowInfo(filename, 3002);
                     return 0;
                  }
                  
                  nTmp = mr_readForPlat(f, indexbuf, indexlen);
                  
                  if ((nTmp != (int32)indexlen))
                  {
                     mr_closeForPlat(f);
                     mr_freeForPlat(indexbuf, indexlen);
                     _mr_readFileShowInfo(filename, 3003);
                     return 0;
                  }
                  

                  //MRDBGPRINTF("str1=%s",filename);
                  while(!found)
                  {
                     MEMCPY(&len, &indexbuf[pos], 4);
#ifdef MR_BIG_ENDIAN
                     len = ntohl(len);
#endif
                     pos = pos + 4;
                     if (((len + pos) > indexlen)||(len<1)||(len>=MR_MAX_FILENAME_SIZE))
                     {
                        mr_closeForPlat(f);
                        mr_freeForPlat(indexbuf, indexlen);
                        _mr_readFileShowInfo(filename, 3004);
                        return 0;
                     }
                     MEMSET(TempName, 0, sizeof(TempName));
                     MEMCPY(TempName, &indexbuf[pos], len);
                     pos = pos + len;
                     if (STRCMP(filename, TempName)==0)
                     {
                        if(lookfor == 1)
                        {
                           mr_closeForPlat(f);
                           mr_freeForPlat(indexbuf, indexlen);
                           return (void *)1;
                        }
                        found = 1;
                        MEMCPY(&file_pos, &indexbuf[pos], 4);
                        pos = pos + 4;
                        MEMCPY(&file_len, &indexbuf[pos], 4);
                        pos = pos + 4;
#ifdef MR_BIG_ENDIAN
                        file_pos = ntohl(file_pos);
                        file_len = ntohl(file_len);
#endif
                        if ((file_pos + file_len) > headbuf[2])
                        {
                           mr_closeForPlat(f);
                           mr_freeForPlat(indexbuf, indexlen);
                           _mr_readFileShowInfo(filename, 3005);
                           return 0;
                        }
                     }else{
                        pos = pos + 12;
                        if (pos >= indexlen)
                        {
                           mr_closeForPlat(f);
                           mr_freeForPlat(indexbuf, indexlen);
                           _mr_readFileShowInfo(filename, 3006);
                           return 0;
                        }
                     }/*if (STRCMP(filename, TempName)==0)*/
                  }

                  mr_freeForPlat(indexbuf, indexlen);
                  
                  *filelen = file_len;
                  
                  //MRDBGPRINTF("Debug:_mr_readFile:old filelen = %d",file_len);
                  filebuf = mr_mallocForPlat((uint32)*filelen);
                  if(filebuf == NULL)
                  {
                     mr_closeForPlat(f);
                     _mr_readFileShowInfo(filename, 3007);
                     return 0;
                  }

                  nTmp = mr_seekForPlat(f, file_pos, MR_SEEK_SET);
                  if (nTmp < 0)
                  {
                     mr_freeForPlat(filebuf, *filelen);
                     mr_closeForPlat(f);
                     _mr_readFileShowInfo(filename, 3008);
                     return 0;
                  }



                  oldlen = 0;
#ifdef MR_SPREADTRUM_MOD
                  if ((*filelen < 0)){
                     MRDBGPRINTF("filelen=%d",*filelen);
                     mr_freeForPlat(filebuf, file_len);
                     mr_closeForPlat(f);
                     _mr_readFileShowInfo(filename, 3010);
                     return 0;
                  }
#endif
                  //MRDBGPRINTF("oldlen=%d",oldlen);
                  while(oldlen < *filelen){
                     //MRDBGPRINTF("oldlen=%d",oldlen);
                     nTmp = mr_readForPlat(f, (char*)filebuf+oldlen, *filelen-oldlen);
                     //MRDBGPRINTF("Debug:_mr_readFile:readlen = %d,oldlen=%d",nTmp,oldlen);
                     //MRDBGPRINTF("oldlen=%d",oldlen);
#ifdef MR_SPREADTRUM_MOD
                     if ((nTmp <= 0) || (oldlen > 1024*1024))
#else
                     if (nTmp <= 0)
#endif
                     {
                         //MRDBGPRINTF("oldlen=%d",oldlen);
                         mr_freeForPlat(filebuf, *filelen);
                         mr_closeForPlat(f);
                         _mr_readFileShowInfo(filename, 3009);
                         return 0;
                     }
                     oldlen = oldlen + nTmp;
                  }

                  mr_closeForPlat(f);

                  
               }
            }
      // 新版的mrp处理
   
   mr_gzInBuf = filebuf;
   LG_gzoutcnt = 0;
   LG_gzinptr = 0;

   method = mr_get_method(*filelen);
   if (method < 0) 
   {
       return filebuf;             
   }

   reallen  = (uint32)(((uch*)filebuf)[*filelen-4]);
   reallen |= (uint32)(((uch*)filebuf)[*filelen-3]) << 8;
   reallen |= (uint32)(((uch*)filebuf)[*filelen-2]) << 16;
   reallen |= (uint32)(((uch*)filebuf)[*filelen-1]) << 24;

   mr_gzOutBuf = mr_mallocForPlat(reallen);
   oldlen = *filelen;
   *filelen = reallen;
   if(mr_gzOutBuf == NULL)
   {
      mr_freeForPlat(mr_gzInBuf, oldlen);
      return 0;
   }
   
   if (mr_unzipForPlat() != 0) {
      mr_freeForPlat(mr_gzInBuf, oldlen);
      mr_freeForPlat(mr_gzOutBuf, reallen);
      MRDBGPRINTF("_mr_readFile: \"%s\" Unzip err!", filename);
      return 0;
   }

    mr_freeForPlat(mr_gzInBuf, oldlen);

   return mr_gzOutBuf;
}


void * _mrc_readFile(const char* mrpname, const char* filename, int *filelen, int lookfor)
{
   void * ret;
   ret = _mr_readFileForPlat(mrpname, filename, filelen, lookfor);
   return ret;
}

int32 _mrc_unzip(uint8* inputbuf, int32 inputlen, uint8** outputbuf, int32* outputlen)
{
   int method;
   uint32 reallen;
   int32 needMalloc=FALSE;

   mr_gzInBuf = inputbuf;
   LG_gzinptr = 0;

   method = mr_get_method(inputlen);
   if (method < 0) 
   {
      return MR_IGNORE;             
   }

   reallen  = (uint32)(((uint8*)inputbuf)[inputlen-4]);
   reallen |= (uint32)(((uint8*)inputbuf)[inputlen-3]) << 8;
   reallen |= (uint32)(((uint8*)inputbuf)[inputlen-2]) << 16;
   reallen |= (uint32)(((uint8*)inputbuf)[inputlen-1]) << 24;

   if(*outputbuf){
      if(*outputlen<reallen){
         return MR_FAILED;
      }
   }else{
      *outputbuf = mr_mallocForPlat(reallen);
      needMalloc = TRUE;
   }
   *outputlen = reallen;
   if(*outputbuf == NULL)
   {
      return MR_FAILED;
   }

   mr_gzOutBuf = *outputbuf;
   LG_gzoutcnt = 0;
   if (mr_unzipForPlat() != MR_SUCCESS) {
      if (needMalloc){
         mr_freeForPlat(*outputbuf, *outputlen);
      }
      return MR_FAILED;
   }

   return MR_SUCCESS;
}


#if 0
#define CHECK_MRP_BUF_SIZE 10240

int32 _mrc_checkMrp(char* mrp_name)
{
   int32 f;
   uint32 headbuf[4];
   int32 nTmp, crc32;
   uint8* tempbuf;

   tempbuf = mr_mallocForPlat(CHECK_MRP_BUF_SIZE);
   if (tempbuf == NULL)
   {
      MRDBGPRINTF("mrc_checkMrp err %d",0);
      return MR_FAILED-1;
   }
   f = mr_openForPlat(mrp_name, MR_FILE_RDONLY );
   if (f == 0)
   {
      mr_freeForPlat(tempbuf, CHECK_MRP_BUF_SIZE);
      MRDBGPRINTF("mrc_checkMrp err %d",1);
      return MR_FAILED-2;
   }
      
   MEMSET(headbuf, 0, sizeof(headbuf));
   nTmp = mr_readForPlat(f, &headbuf, sizeof(headbuf));
   mr_updcrc(NULL, 0);
   mr_updcrc((uint8*)&headbuf, sizeof(headbuf));
#ifdef MR_BIG_ENDIAN
   headbuf[0] = ntohl(headbuf[0]);
   headbuf[1] = ntohl(headbuf[1]);
   headbuf[2] = ntohl(headbuf[2]);
   headbuf[3] = ntohl(headbuf[3]);
#endif
   if( (nTmp != 16)||(headbuf[0] != 1196446285/*1196446285*/) ||(headbuf[1] <= 232))
   {
      mr_closeForPlat(f);
      mr_freeForPlat(tempbuf, CHECK_MRP_BUF_SIZE);
      //MRDBGPRINTF("%d", headbuf[0]);
      //MRDBGPRINTF("%d", headbuf[1]);
      //MRDBGPRINTF("%d", nTmp);
      MRDBGPRINTF("mrc_checkMrp err %d",2);
      return MR_FAILED-3;
   }

   
   nTmp = mr_readForPlat(f, tempbuf, 224);
   if(nTmp != 224)
   {
      mr_closeForPlat(f);
      mr_freeForPlat(tempbuf, CHECK_MRP_BUF_SIZE);
      MRDBGPRINTF("mrc_checkMrp err %d",3);
      return MR_FAILED-4;
   }

//2008-6-11
#ifdef MR_SPREADTRUM_MOD
   if (tempbuf[192] != 2)
#else
   if (tempbuf[192] != 1)
#endif
   {
      mr_closeForPlat(f);
      mr_freeForPlat(tempbuf, CHECK_MRP_BUF_SIZE);
      MRDBGPRINTF("mrc_checkMrp err %d",31);
      return MR_FAILED-5;
   }
//2008-6-11
   
   MEMCPY(&crc32, &tempbuf[68], 4);
#ifdef MR_BIG_ENDIAN
   crc32 = ntohl(crc32);
#endif
   MEMSET(&tempbuf[68], 0, 4);
   mr_updcrc(tempbuf, 224);

   while(nTmp > 0){
      nTmp = mr_readForPlat(f, tempbuf, 10240);
      if(nTmp > 0){
         mr_updcrc(tempbuf, nTmp);
      }
   }
   if (crc32 == mr_updcrc(tempbuf, 0)){
      nTmp = MR_SUCCESS;
   }else{
      //MRDBGPRINTF("%d", crc32);
      //MRDBGPRINTF("%d", t);
      MRDBGPRINTF("mrc_checkMrp err %d",4);
      nTmp = MR_FAILED-6;
   }
   mr_closeForPlat(f);
   mr_freeForPlat(tempbuf, CHECK_MRP_BUF_SIZE);
   return nTmp;
}
#endif

