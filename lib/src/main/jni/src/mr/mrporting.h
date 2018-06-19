
#ifndef mrporting_h
#define mrporting_h

/*如果手机平台没有定义下面的类型，将#if 语句内的定义生效*/
#if 0
#define MR_C_NUMBER_TYPE
typedef  unsigned short     uint16;      /* Unsigned 16 bit value */
typedef  unsigned long int  uint32;      /* Unsigned 32 bit value */
typedef  long int  int32;      /* signed 32 bit value */
typedef  unsigned char        uint8;        /* Signed 8  bit value */
typedef  signed char        int8;        /* Signed 8  bit value */
typedef  signed short       int16;       /* Signed 16 bit value */
#endif


enum {
   MR_KEY_0,               //按键 0
   MR_KEY_1,               //按键 1
   MR_KEY_2,               //按键 2
   MR_KEY_3,               //按键 3
   MR_KEY_4,               //按键 4
   MR_KEY_5,               //按键 5
   MR_KEY_6,               //按键 6
   MR_KEY_7,               //按键 7
   MR_KEY_8,               //按键 8
   MR_KEY_9,               //按键 9
   MR_KEY_STAR,            //按键 *
   MR_KEY_POUND,           //按键 #
   MR_KEY_UP,              //按键 上
   MR_KEY_DOWN,            //按键 下
   MR_KEY_LEFT,            //按键 左
   MR_KEY_RIGHT,           //按键 右
   MR_KEY_POWER,           //按键 挂机键
   MR_KEY_SOFTLEFT,        //按键 左软键
   MR_KEY_SOFTRIGHT,       //按键 右软键
   MR_KEY_SEND,            //按键 接听键
   MR_KEY_SELECT,          //按键 确认/选择（若方向键中间有确认键，建议设为该键）
   MR_KEY_VOLUME_UP,          //按键 侧键上
   MR_KEY_VOLUME_DOWN,          //按键 侧键下
   MR_KEY_NONE             //按键 保留
};

enum {
   MR_NET_ID_MOBILE,                  //移动
   MR_NET_ID_CN,          // 联通gsm
   MR_NET_ID_CDMA,       //联通CDMA
   MR_NET_ID_NONE       //未插卡
};

enum {
   MR_EDIT_ANY,                  //任何字符
   MR_EDIT_NUMERIC,          // 数字
   MR_EDIT_PASSWORD       //密码，用"*"显示
};

enum {
   MR_SIM_NEW,     //新手机第一次插入SIM卡
   MR_SIM_CHANGE, //用户更换SIM卡
   MR_SIM_SAME    //未进行换卡操作
};

enum {
   MR_DIALOG_OK,                   //对话框有"确定"键。
   MR_DIALOG_OK_CANCEL,
   MR_DIALOG_CANCEL
};

enum {
   MR_DIALOG_KEY_OK,
   MR_DIALOG_KEY_CANCEL
};

enum {
   MR_LOCALUI_KEY_OK,
   MR_LOCALUI_KEY_CANCEL
};

enum {
   MR_KEY_PRESS,
   MR_KEY_RELEASE,
   MR_MOUSE_DOWN,
   MR_MOUSE_UP,
   MR_MENU_SELECT,
   MR_MENU_RETURN,
   MR_DIALOG_EVENT,
   MR_SMS_INDICATION,
   MR_EXIT_EVENT,
   MR_SMS_RESULT,              //短信发送结果事件。
   MR_LOCALUI_EVENT         //本地化接口事件
};

typedef enum 
{
   MR_SOUND_MIDI,
   MR_SOUND_WAV,
   MR_SOUND_MP3,
   MR_SOUND_AMR,
   MR_SOUND_PCM    //8K
}MR_SOUND_TYPE;

typedef enum 
{
   MR_FONT_SMALL,
   MR_FONT_MEDIUM,
   MR_FONT_BIG
}MR_FONT_TYPE;

enum
{
   MR_SEEK_SET, 
   MR_SEEK_CUR, 
   MR_SEEK_END
};

enum
{
   MR_SOCK_STREAM,
   MR_SOCK_DGRAM
};

enum
{
   MR_IPPROTO_TCP,
   MR_IPPROTO_UDP
};

enum
{
   MR_ENCODE_ASCII,
   MR_ENCODE_UNICODE
};

#define   MR_SMS_ENCODE_FLAG   7
#define   MR_SMS_REPORT_FLAG   8
#define   MR_SMS_RESULT_FLAG   16

enum {
   MR_SOCKET_BLOCK,          //阻塞方式（同步方式）
   MR_SOCKET_NONBLOCK       //非阻塞方式（异步方式）
};

typedef struct
{
   uint16 year;                 //年
   uint8  month;                //月
   uint8  day;                  //日
   uint8  hour;                 //时，24小时制
   uint8  minute;               //分
   uint8  second;               //秒
}mr_datetime;

typedef struct
{
   uint32 width;                  //屏幕宽
   uint32 height;                 //屏幕高
   uint32 bit;                    //屏幕象素深度，单位bit
}mr_screeninfo;

typedef struct
{
	uint8       IMEI[16];	      //IMEI len eq 15
	uint8       IMSI[16];	      //IMSI len not more then 15
	char        manufactory[8];  //厂商名，最大7个字符，空字节填\0
	char        type[8];         //mobile type，最大7个字符，空字节填\0
	uint32      ver;            //SW ver
	uint8       spare[12];       //备用
}mr_userinfo;

typedef struct
{
   int32 socket;
   int32 port;
}mr_bind_st;



#ifdef MR_FS_ASYN

typedef int32 (*MR_ASYN_FS_CB)(int32 result, uint32  cb_param);

typedef struct
{
   void* buf;                 //文件缓存地址
   uint32  buf_len;                //缓冲长度，即要读取/写入的长度
   uint32  offset;               //文件读取/写入偏移
   MR_ASYN_FS_CB  cb;      //回调函数
   uint32  cb_param;               //回调参数
}mr_asyn_fs_param;
#endif


#if 0 //取消
typedef struct
{
   uint32 mr_head;                 //Mythroad文件标签
   uint32 mri_len;                   //应用信息头长度
   uint32 app_len;                  //应用长度
   uint32 appid;                      //应用ID
   char    filename[12];          //应用文件名
   uint16 appname[20];          //应用名，Unicode
   uint16 appversion[10];          //应用版本，Unicode
   uint16 vendor[20];             //开发商信息，Unicode
   uint16 description[40];      //应用描述，Unicode
   char    mrdata[32];            //应用数据
}mr_appSt;
#endif

#define MR_FILE_RDONLY         1//以只读的方式打开文件。
#define MR_FILE_WRONLY        2//以只写的方式打开文件。
#define MR_FILE_RDWR             4//以读写的方式打开文件。
#define MR_FILE_CREATE          8//如果文件不存在，创建该文件。
#define MR_FILE_RECREATE      16//无论文件存不存在，都重新创建该文件。

#define MR_IS_FILE     1      //文件
#define MR_IS_DIR      2      //目录
#define MR_IS_INVALID  8  //无效(非文件、非目录)


#define MR_FILE_HANDLE int32
#define MR_SEARCH_HANDLE int32

#define MR_SUCCESS  0    //成功
#define MR_FAILED   -1    //失败
#define MR_IGNORE   1     //不关心
#define MR_WAITING   2     //异步(非阻塞)模式
//#define MR_NO_SUPPORT   -2     //不支持

#define MR_PLAT_VALUE_BASE 1000    //用于某些返回值的基础值



/*定时器到期时调用定时器事件，Mythroad平台将对之进行处理。
p是启动定时器时传入的Mythroad定时器数据*/
extern int32 mr_timer(void);
/*在Mythroad平台中对按键事件进行处理，press	= MR_KEY_PRESS按键按下，
= MR_KEY_RELEASE按键释放，key	对应的按键编码*/
extern int32 mr_event(int16 type, int32 param1, int32 param2);
/*退出Mythroad并释放相关资源*/
extern int32 mr_stop(void);


/****************外部接口定义*********************/


#if 0 //已取消
/*当启动Mythroad应用的时候，应该调用Mythroad的初始化函数，
用以对Mythroad平台进行初始化，并用在Mythroad平台上运行指
定的应用，app指向将要运行的游戏或应用的启动数据*/
extern int32 mr_start(mr_appSt* app);
#endif

/*当启动DSM应用的时候，应该调用DSM的初始化函数，
用以对DSM平台进行初始化*/
extern int32 mr_start_dsm(const char* entry);

/*
与函数mr_start_dsm功能相似，区别是可以分别设置启动文件
和启动参数。
*/
int32 mr_start_dsm_ex(const char* filename, const char* entry);


/*
与函数mr_start_dsm功能相似，区别是mr_start_dsmB会处理以','分割的
启动文件及参数
如:
若entry为"%applist.mrp,reload"，则mr_start_dsmB认为"applist.mrp"是启动文件
而"%applist.mrp,reload"为启动字串
*/
int32 mr_start_dsmB(const char* entry);

/*
与函数mr_start_dsm功能相似，区别是mr_start_dsmC可以设置mrp初始
加载的文件名，在精简VM中，默认加载文件为"logo.ext"*/
int32 mr_start_dsmC(char* start_file, const char* entry);


/*注册固化应用*/
extern int32 mr_registerAPP(uint8 *p, int32 len, int32 index);

#if 0 //已取消
/*取得应用列表
   appList:指向已经安装未安装应用列表的第一个元素
   appLen:应用的个数
   注意:appList所指向的内存空间必须由调用此函数的程序员
   释放
*/
extern int32 mr_getAppList(mr_appSt** appList, uint32* appLen);

/*取得应用说明*/
void mr_getAppInfo(char* info, mr_appSt *app);

/*删除指定的应用
app:该应用的数据指针
*/
extern int32 mr_uninstall(mr_appSt *app);
#endif

/*暂停应用*/
extern int32 mr_pauseApp(void);

/*恢复应用*/
extern int32 mr_resumeApp(void);

/*当手机收到短消息时调用该函数*/
extern int32 mr_smsIndiaction(uint8 *pContent, int32 nLen, uint8 *pNum, int32 type);

/*对下载内容（保存在内存区中的一个下载的文件）进行判断，
若下载文件是DSM菜单，由DSM引擎对下载文件进行保存。使用
本函数时，下载文件应该已经下载完全，并且全部内容保存在
所给的内存中。*/
extern int32 mr_save_mrp(void *p,uint32 l);

/*功能同mr_save_mrp，但传入的是一个打开的文件句柄，文件由
调用者关闭。该函数目前尚未实现，若需要使用，请联系ouli*/
extern int32 mr_save_mrp_with_handle(MR_FILE_HANDLE f);

/*用户SIM卡变更*/
extern int32 mr_newSIMInd(int16  type, uint8* old_IMSI);

/*函数mr_initNetwork使用的回调函数定义*/
typedef int32 (*MR_INIT_NETWORK_CB)(int32 result);

/*函数mr_initNetwork使用的回调函数定义*/
typedef int32 (*MR_CONNECT_CB)(int32 result);

/*函数mr_getHostByName使用的回调函数定义*/
typedef int32 (*MR_GET_HOST_CB)(int32 ip);


/****************外部接口定义*********************/







/*********************以下是抽象接口定义******************************/





/*调试打印*/
extern void mr_printf(const char *format,...);


/*内存申请*/
extern int32 mr_mem_get (char** mem_base, uint32* mem_len);
/*内存申请*/
extern int32 mr_mem_get_ex (int32 ram ,char** mem_base, uint32* mem_len);
/*内存释放*/
extern int32 mr_mem_free (char* mem, uint32 mem_len);


/*******************************************************************/
/*当使用本地屏幕缓冲时使用的接口*/
/*在屏幕上绘BMP*/
extern void mr_drawBitmap(uint16* bmp, int16 x, int16 y, uint16 w, uint16 h);
/*取得获取unicode码ch指向的字体的点阵信息，并告之
该字体的宽和高，获取到的点阵信息每一个bit表示
字体的一个象素，字体每行的象素必须按字节对其，
也就是说如果一个字体宽为12，则需要用两个字节
来表示该信息，第二个字节的后四个bit为0，从第三
个字节开始才表示下一行的点阵数据*/
extern const char *mr_getCharBitmap(uint16 ch, uint16 fontSize, int *width, int *height);
/*******************************************************************/

#if 0
/*******************************************************************/
/*当使用厂商屏幕缓冲时使用的接口*/
/*显示字符串于屏幕，绘制左上角为x,y。color是RGB颜色*/
extern void mr_drawText(char * pcText, int16 x, int16 y, uint32 color);
/*从显示缓冲区到显示设备的图象更新操作。更新范围左上角为x,y，
宽高为w,h*/
extern void mr_bufToScreen(int16 x, int16 y, uint16 w, uint16 h);
/*取得屏幕缓冲区指针*/
extern void* mr_getScreenBuf(void);
/*******************************************************************/
#endif

/*显示字符于屏幕，绘制左上角为x,y。color是565的RGB颜色
ch为字符unicode码*/
extern void mr_platDrawCharReal(uint16 ch, int32 x, int32 y, uint16 color);

/*启动定时器*/
extern int32 mr_timerStart(uint16 t);
/*停止定时器。*/
extern int32 mr_timerStop(void);
/*取得时间，单位ms*/
extern uint32 mr_getTime(void);
/*获取系统日期时间。*/
extern int32 mr_getDatetime(mr_datetime* datetime);
/*取得手机相关信息。*/
extern int32 mr_getUserInfo(mr_userinfo* info);
/*任务睡眠，单位ms*/
extern int32 mr_sleep(uint32 ms);
/*平台扩展接口*/
extern int32 mr_plat(int32 code, int32 param);
/*增强的平台扩展接口*/
typedef void (*MR_PLAT_EX_CB)(uint8* output, int32 output_len);
extern int32 mr_platEx(int32 code, uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb);


/*文件和目录操作*/
extern int32 mr_ferrno(void);
extern MR_FILE_HANDLE mr_open(const char* filename,  uint32 mode);
extern int32 mr_close(MR_FILE_HANDLE f);
extern int32 mr_info(const char* filename);
extern int32 mr_write(MR_FILE_HANDLE f,void *p,uint32 l);
extern int32 mr_read(MR_FILE_HANDLE f,void *p,uint32 l);
extern int32 mr_seek(MR_FILE_HANDLE f, int32 pos, int method);
extern int32 mr_getLen(const char* filename);
extern int32 mr_remove(const char* filename);
extern int32 mr_rename(const char* oldname, const char* newname);
extern int32 mr_mkDir(const char* name);
extern int32 mr_rmDir(const char* name);


/*异步文件读写接口*/
#ifdef MR_FS_ASYN
extern int32 mr_asyn_read(MR_FILE_HANDLE f, mr_asyn_fs_param *param);
extern int32 mr_asyn_write(MR_FILE_HANDLE f, mr_asyn_fs_param *param);
#endif

/*目录搜索开始*/
extern MR_SEARCH_HANDLE mr_findStart(const char* name, char* buffer, uint32 len);
/*取得一个目录搜索结果*/
extern int32 mr_findGetNext(MR_SEARCH_HANDLE search_handle, char* buffer, uint32 len);
/*目录搜索结束*/
extern int32 mr_findStop(MR_SEARCH_HANDLE search_handle);

/*退出平台*/
extern int32 mr_exit(void);

/*开始手机震动*/
extern int32 mr_startShake(int32 ms);
/*结束手机震动*/
extern int32 mr_stopShake(void);

/*播放音频数据*/
extern int32 mr_playSound(int type, const void* data, uint32 dataLen, int32 loop);
/*停止播放音频*/
extern int32 mr_stopSound (int type);

/*发送一条短消息*/
extern int32 mr_sendSms(char* pNumber, char*pContent, int32 flags);
/*拨打电话*/
extern void mr_call(char *number);
/*取得网络ID，0 移动，1 联通*/
extern int32 mr_getNetworkID(void);
/*连接WAP*/
extern void mr_connectWAP(char* wap);


/*GUI 接口*/
extern int32 mr_menuCreate(const char* title, int16 num);
extern int32 mr_menuSetItem(int32 menu, const char *text, int32 index);
extern int32 mr_menuShow(int32 menu);
/*设置选中项目，保留*/
extern int32 mr_menuSetFocus(int32 menu, int32 index);
extern int32 mr_menuRelease(int32 menu);
extern int32 mr_menuRefresh(int32 menu);



extern int32 mr_dialogCreate(const char * title, const char * text, int32 type);
extern int32 mr_dialogRelease(int32 dialog);
extern int32 mr_dialogRefresh(int32 dialog, const char * title, const char * text, int32 type);

extern int32 mr_textCreate(const char * title, const char * text, int32 type);
extern int32 mr_textRelease(int32 text);
extern int32 mr_textRefresh(int32 handle, const char * title, const char * text);

extern int32 mr_editCreate(const char * title, const char * text, int32 type, int32 max_size);
extern int32 mr_editRelease(int32 edit);
extern const char* mr_editGetText(int32 edit);

extern int32 mr_winCreate(void);
extern int32 mr_winRelease(int32 win);

extern int32 mr_getScreenInfo(mr_screeninfo * screeninfo);


/*Socket接口*/
extern int32 mr_initNetwork(MR_INIT_NETWORK_CB cb, const char *mode);
extern int32 mr_closeNetwork(void);
extern int32 mr_getHostByName(const char *name, MR_GET_HOST_CB cb);
extern int32 mr_socket(int32 type, int32 protocol);
extern int32 mr_connect(int32 s, int32 ip, uint16 port, int32 type);
extern int32 mr_closeSocket(int32 s);
extern int32 mr_recv(int32 s, char *buf, int len);
extern int32 mr_recvfrom(int32 s, char *buf, int len, int32 *ip, uint16 *port);
extern int32 mr_send(int32 s, const char *buf, int len);
extern int32 mr_sendto(int32 s, const char *buf, int len, int32 ip, uint16 port);


/*
这个函数，转成小端的unicode。支持批量转，但是一次
最好不要多于128个字节。
*/
void dsmGB2UCS2(char * src,char *dest);

//------------- C接口封装 --------------------------------------
void* mr_memcpy(void* dst, const void* src, uint32 n);
void* mr_memmove(void* dst, const void* src, uint32 n);
void* mr_memset(void* dst, int ch, uint32 n);
int mr_memcmp(const void* lhs, const void* rhs, uint32 n);
void* mr_memchr(const void* s, int ch, uint32 n);
void* mr_strcpy(char* dst, const char* src);
char* mr_strncpy(char* dst, const char* src, uint32 n);
char* mr_strcat(char* dst, const char* src);
char* mr_strncat(char* dst, const char* src, uint32 n);
int mr_strcmp(const char* lhs, const char* rhs);
int mr_strncmp(const char* lhs, const char* rhs, uint32 n);
int mr_strcoll(const char* lhs, const char* rhs);
int mr_strlen(const char* s);
char* mr_strstr(const char* haystack, const char* needle);
int mr_atoi(const char* s);
unsigned long mr_strtoul(const char* s, char** end_ptr, int base);
int mr_rand(void);


#endif
