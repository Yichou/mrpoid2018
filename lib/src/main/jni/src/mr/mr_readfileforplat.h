
/*
以mode方式打开文件，如果文件不存在，根据mode值
判断是否创建之。
输入参数
参数	说明
返回说明
返回状态	说明 
非 NULL	文件句柄
NULL	失败（注意，这里与其他接口不一样）

输入:
filename	文件名
mode	文件打开方式
         mode取值
                MR_FILE_RDONLY   //以只读的方式打开文件。
                MR_FILE_WRONLY   //以只写的方式打开文件。
                                    （这个方式可能被实现为与
                                    MR_FILE_RDWR相同的操作）
                MR_FILE_RDWR      //以读写的方式打开文件。
                MR_FILE_CREATE     //如果文件不存在，创建
                                    该文件，该参数不会单独出现
                                    ，只能与其他值一同出现（使
                                    用"或"运算）
               mode可能的取值：
               a、	前三个参数的其中一个
               b、	前三个参数的其中一个和MR_FILE_CREATE的"或"值
返回:
非 NULL	       文件句柄
NULL	            失败（注意，这里与其他接口不一样）
*/
extern MR_FILE_HANDLE mr_openForPlat(const char* filename,  uint32 mode);

/*
关闭文件。
输入:
f	文件句柄
返回:
MR_SUCCESS	成功
MR_FAILED	失败
*/
extern int32 mr_closeForPlat(MR_FILE_HANDLE f);

/*
读取文件的内容到指定的缓冲。
输入:
f	文件句柄
p	文件缓存地址
l	缓冲长度
返回:
      >=0                确切读取的字节数
      MR_FAILED      失败
*/
extern int32 mr_readForPlat(MR_FILE_HANDLE f,void *p,uint32 l);

extern int32 mr_seekForPlat(MR_FILE_HANDLE f, int32 pos, int method);



extern void* mr_mallocForPlat(uint32 len);
extern void mr_freeForPlat(void* p, uint32 len);


/************************************************************/
/*
从mrp中读取文件的所有内容到申请的内存中。
使用该函数得到的内存，需要使用mr_freeForPlat函数释放。

输入:
mrpname     mrp文件名
filename    读取文件名
lookfor          指出函数的操作形式:
                    0:    读取mrp中文件的内容
                    1:    仅仅查找mrp中是否存在该文件，并不读取
                               文件内容
                    5:    返回该文件在MRP中的偏移量和长度（文件不压缩）

输出:
*filelen         当lookfor==0时返回文件内容长度
                   当lookfor==1时未知
                   当lookfor==5时返回文件内容长度

返回:
   当lookfor==0时
      非NULL         指向读取到的内容的指针，该内存需要
                              调用者释放
      NULL         失败
   当lookfor==1时
      1         mrp中存在该文件
      NULL         mrp中不存在该文件
   当lookfor==5时
      非NULL         该文件在MRP中的偏移量
      NULL         mrp中不存在该文件
*/
extern void * _mrc_readFile(const char* mrpname, const char* filename, int *filelen, int lookfor);

/*
解压缩。

输入:
inputbuf  待解压数据指针
inputlen  待解压数据长度
outputbuf    若不等于NULL，解压后数据存放的空间
outputlen    若outputbuf不等于NULL，解压后数据存放空间的长度
                  若outputlen小于解压后数据的长度，解压失败

输出:
outputbuf    解压后的数据，需要应用自己释放(mrc_free)；
outputlen    解压后的数据长度
      
返回:
      MR_SUCCESS     成功
      MR_FAILED         失败。
      MR_IGNORE        数据不是可识别的压缩格式。
*/
extern int32 _mrc_unzip(uint8* inputbuf, int32 inputlen, uint8** outputbuf, int32* outputlen);

