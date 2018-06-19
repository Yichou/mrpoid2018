#include <stdio.h>
#include <android/log.h>
#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <time.h>
#include <linux/time.h>
#include <signal.h>
#include <sys/time.h>
#include <fcntl.h>
#include <asm-generic/fcntl.h>
#include <dirent.h>
#include <stdint.h>
#include <zlib.h>
#include <asm-generic/mman.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <linux/in.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/vfs.h>
#include <sys/mman.h>
#include <errno.h>


#include "emulator.h"

#include "mrporting.h"

//#include "font/tsffont.h"
#include "font/font_sky16_2.h"
#include "encode.h"
#include "utils.h"
#include "mr_plat.h"



static int32 dsmSwitchPath(uint8* input, int32 input_len, uint8** output, int32* output_len);
static void DsmPathInit();

extern void DsmSocketInit();
extern void DsmSocketClose();
void dsmRestoreRootDir();


//Linux处理错误
#define handle_error(msg) \
	do { perror(msg); exit(EXIT_FAILURE); } while (0)

uint16				*screenBuf;


#define 		NO_EXRAM 			0
#define 		DSM_MEM_SIZE		(gEmuEnv.memSize*1024*1024)	//DSM内存大小


//-- log 缓冲区 -------------------------------
#define			PRINTF_BUF_LEN  1024
static char		printfBuf[PRINTF_BUF_LEN + 2] = {0};
static char		utf8Buf[PRINTF_BUF_LEN*2 + 2] = {0};


static T_DSM_DISK_INFO 	dsmDiskInfo;
static T_DSM_CELL_INFO 	dsmCellInfo;
static char				dsmIMEI[MAX_IMEI_LEN+1];
static char				dsmIMSI[MAX_IMSI_LEN+1];
static int				dsmNetWorkID;
static T_MEDIA_TIME		dsmCommonRsp; //播放器回调变量
static int 				mr_getUserInfo_times = 0;

char					dsmSmsCenter[MAX_SMS_CENTER_LEN+1];
int						dsmNetType;
T_DSM_MEDIA_PLAY 		dsmMediaPlay; //音乐播放接口回调
char 					dsmType[8] = DSM_FACTORY;
char 					dsmFactory[8] = DSM_TYPE;

static MRAPP_IMAGE_SIZE_T dsmImageSize;


void dsm_init()
{
	DsmPathInit();
	DsmSocketInit();

	const char * str = emu_getStringSysinfo(SYSINFO_IMEI);
	if(str){
		strcpy(dsmIMEI, str);
		free((void*)str);
	}

	str = emu_getStringSysinfo(SYSINFO_IMSI);
	if(str){
		strcpy(dsmIMSI, str);
		free((void*)str);
	}

	dsmNetWorkID = emu_getIntSysinfo("netID");
	dsmNetType = emu_getIntSysinfo("netType");
}

void dsm_reset()
{
	DsmSocketClose();
	FREE_SET_NULL(gEmuEnv.exMem);
	mr_getUserInfo_times = 0;
}

//----------------------------------------------------------------

int32 mr_exit(void)
{
	LOGD("mr_exit() called by mythroad!");

	dsm_reset();
	emu_finish();

	gEmuEnv.b_vm_running = 0;

	return MR_SUCCESS;
}

#define MAKE_PLAT_VERSION(plat,ver,card,impl,brun) \
	(100000000 + (plat) * 1000000 + (ver) * 10000 + (card) * 1000 + (impl) * 10 + (brun))

//#if 1

/*取得手机相关信息。*/
int32 mr_getUserInfo1(mr_userinfo* info)
{
	if (!info)
		return MR_FAILED;

	if(showApiLog) LOGI("mr_getUserInfo1");

	memset(info, 0, sizeof(mr_userinfo));
	strncpy(info->IMEI, (uint8 *)dsmIMEI, 15);
	strncpy(info->IMSI, (uint8 *)dsmIMSI, 15);
	strncpy(info->manufactory, "mrpoid", 8);
	strncpy(info->spare, (char *)"yichouangle", 12);
	strncpy(info->type, "android ", 8);

	info->ver = MAKE_PLAT_VERSION(gEmuEnv.platform, 8, 0, 18, 0);
//	info->ver = MAKE_PLAT_VERSION(1, 8, 0, 18, 0);

	return MR_SUCCESS;
}

//#else


int32 mr_getUserInfo(mr_userinfo* info)
{
	if (info == NULL)
		return MR_FAILED;

//	if(showApiLog) LOGI("mr_getUserInfo");

	memset(info, 0x00, sizeof(mr_userinfo));
	memcpy(info->IMEI, dsmIMEI, 15);
	memcpy(info->IMSI, dsmIMSI, 15);

//	strncpy(info->manufactory, "aux", 3);
//	strncpy(info->type, "m625", 7);

	strncpy(info->manufactory, dsmFactory, 7);
	strncpy(info->type, dsmType, 7);

//	if(++mr_getUserInfo_times > 8)
//	{
//		info->ver = MAKE_PLAT_VERSION(1, 3, 0, 18, 0);
////		info->ver = 101000000 + 3 * 10000 + DSM_FAE_VERSION;
//		LOGI("\tnow is MT6225");
//	}else
		info->ver = 101000000 + DSM_PLAT_VERSION * 10000 + DSM_FAE_VERSION;
//	info->ver = 116000000 + DSM_PLAT_VERSION * 10000 + DSM_FAE_VERSION; //SPLE
//	info->ver = MAKE_PLAT_VERSION(1, 3, 0, 18, 0);

	memset(info->spare, 0, sizeof(info->spare));

#if 0
	LOGI("imei = %s",info->IMEI);
	LOGI("imsi = %s",info->IMSI);
	LOGI("factory = %s",info->manufactory);
	LOGI("type = %s",info->type);
	LOGI("ver = %d",info->ver);
#endif

//	LOGI("mr_getUserInfo suc!");

	return MR_SUCCESS;
}

//#endif


void mr_cacheFlush(int id)
{
#if defined(__arm__)
	cacheflush((long)gEmuEnv.vm_mem_base, (long)(gEmuEnv.vm_mem_base + gEmuEnv.vm_mem_len), 0);
#endif
}

#if 1  //mr_dsm
int32 mr_cacheSync(void* addr, int32 len)
{
//	if(showApiLog)
//		LOGI("mr_cacheSync(%#p, %d)", addr, len);
#if defined(__arm__)
	cacheflush((long)addr, (long)(addr + len), 0);
#endif
	return MR_SUCCESS;
}
#endif

#if 0
static void segv_handler (int signal_number)
{
	printf ("memory accessed!\n");
}

static int mem_fd = 0;
int32 mr_mem_get(char** mem_base, uint32* mem_len)
{
	int alloc_size;
	char* memory;
	int pagesize, pagecount;

	pagesize = getpagesize();
	pagecount = DSM_MEM_SIZE/pagesize;
	alloc_size = pagesize * pagecount;

	/* 使用映射 /dev/zero 分配内存页 */
	mem_fd = open ("/dev/zero", O_RDONLY );
	if(mem_fd < 0){
		LOGE("open /dev/zero fail!");
		exit(1);
	}

	memory = mmap (NULL, alloc_size, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE, mem_fd, 0);
	if(memory == MAP_FAILED){
		LOGE("mmap fail!");
		close (mem_fd);
		exit(1);
	}

	*mem_base = memory;
	*mem_len = alloc_size;
	gEmuEnv.vm_mem_base = memory;
	gEmuEnv.vm_mem_len = alloc_size;

	LOGI("mr_mem_get addr: 0x%08x, len: %d", memory, alloc_size);

	return MR_SUCCESS;
}

int32 mr_mem_free(char* memory, uint32 alloc_size)
{
	LOGI("mr_mem_free addr: 0x%08x, len: %d", memory, alloc_size);

	munmap (memory, alloc_size);
	close(mem_fd);
	mem_fd = -1;
	gEmuEnv.vm_mem_base = NULL;
	gEmuEnv.vm_mem_len = 0;

	return MR_SUCCESS;
}

#else

//extern void sys_cacheflush();
int32 mr_mem_get(char** mem_base, uint32* mem_len){
	char *buffer;
	int pagesize, pagecount, len = DSM_MEM_SIZE;

	pagesize = sysconf(_SC_PAGE_SIZE);
	if (pagesize == -1)
		handle_error("sysconf");

	pagecount = len/pagesize;	
	len = pagesize*pagecount;
	buffer = memalign(pagesize, len);
	if (buffer == NULL)
		handle_error("memalign");

	//设置内存可执行权限
	if (mprotect(buffer, len, PROT_EXEC|PROT_WRITE|PROT_READ) == -1){
		free(buffer);
		handle_error("mprotect");
	}

	*mem_base = buffer;
	*mem_len = len;

    gEmuEnv.vm_mem_base = buffer;
	gEmuEnv.vm_mem_len = len;
    gEmuEnv.vm_mem_end = buffer + len;

	if(showApiLog) 
		LOGE("mr_mem_get base=%p len=%x end=%p =================", gEmuEnv.vm_mem_base, len, gEmuEnv.vm_mem_end);

	return MR_SUCCESS;
}

int32 mr_mem_free(char* mem, uint32 mem_len)
{
	free(mem);

	gEmuEnv.vm_mem_base = NULL;
	gEmuEnv.vm_mem_len = 0;

	if(showApiLog)
		LOGI("mr_mem_free");

	return MR_SUCCESS;
}

#endif

int32 pageMalloc(void **out, int32 *outLen, uint32 needLen)
{
	char *buf;
	int pagesize, pagecount;

	pagesize = sysconf(_SC_PAGE_SIZE);
	if (pagesize == -1)
		handle_error("sysconf");

	pagecount = needLen/pagesize + 1;	
	needLen = pagesize*pagecount;
	buf = memalign(pagesize, needLen);
	if (buf == NULL)
		handle_error("memalign");

	//设置内存可执行权限
	if (mprotect(buf, needLen, PROT_EXEC|PROT_WRITE|PROT_READ) == -1){
		free(buf);
		handle_error("mprotect");
	}

	*out = buf;
	*outLen = needLen;

	return MR_SUCCESS;
}


void dsmGB2UCS2(char * src, char *dest)
{
	gbToUCS2((uint8 *)src, (uint8 *)dest);
}

int mr_sprintf(char *buf, const char *fmt, ...)
{
//	LOGI("mr_sprintf(%#p, %s)", buf, fmt);

	__va_list vars;
	int ret;

	va_start(vars, fmt);
	ret = vsprintf(buf, fmt, vars);
	va_end(vars);

	return ret;
}

void mr_printf(const char *format, ...)
{
//	LOGI("mr_printf(%s)", format);

	if(!gEmuEnv.showMrPrintf)
		return;

	if(!format){
		__android_log_print(ANDROID_LOG_ERROR, "mythroad", "mr_printf null");
		return;
	}

	__va_list params;
	int len;

	va_start(params, format);
	len = vsnprintf(printfBuf, PRINTF_BUF_LEN, format, params);
	va_end(params);

	GBToUTF8String((uint8 *)printfBuf, (uint8 *)utf8Buf, sizeof(utf8Buf));
	__android_log_print(ANDROID_LOG_INFO, "mythroad", "%s", utf8Buf);
}

int32 mr_timerStart(uint16 t)
{
    emu_timerStart(t); //调java定时器

	if(gEmuEnv.showTimerLog)
		LOGI("mr_timerStart(t:%d) suc!", t);

	return MR_SUCCESS;
}

int32 mr_timerStop(void)
{
	if(gEmuEnv.showTimerLog)
		LOGI("mr_timerStop");

    emu_timerStop();

	return MR_SUCCESS;
}

/*取得时间，单位ms*/
uint32 mr_getTime(void)
{
	struct timeval t;
	int ret = gettimeofday(&t, NULL);

	/**
	 * 考虑到 微秒可能会 <0
	 *
	 * 2013-3-22 20:47:23
	 */
	if(t.tv_usec < 0){
		t.tv_sec--;
		t.tv_usec += 1000000;
	}
	uint32 s = ret==0? ((t.tv_sec - gEmuEnv.dsmStartTime.tv_sec)*1000 + (t.tv_usec - gEmuEnv.dsmStartTime.tv_usec)/1000) : 0;

//	LOGI("mr_getTime 0x%08x", s);

	return s;
}

/*获取系统日期时间。*/
int32 mr_getDatetime(mr_datetime* datetime)
{
	if (!datetime)
		return MR_FAILED;

	time_t now;
	struct tm *t;

	time(&now);
	t = localtime(&now);

	/*struct   tm
	　　{
　　		  int   tm_sec;//seconds   0-61
　　		  int   tm_min;//minutes   1-59
　　		  int   tm_hour;//hours   0-23
　　		  int   tm_mday;//day   of   the   month   1-31
　　		  int   tm_mon;//months   since   jan   0-11
　　		  int   tm_year;//years   from   1900
　　		  int   tm_wday;//days   since   Sunday,   0-6
　　		  int   tm_yday;//days   since   Jan   1,   0-365
　　		  int   tm_isdst;//Daylight   Saving   time   indicator
	　　};*/

	//2013-3-22 13:08:50 修正需要加上时间 1900
	datetime->year = t->tm_year + 1900;
	datetime->month = t->tm_mon + 1;
	datetime->day = t->tm_mday;
	datetime->hour = t->tm_hour;
	datetime->minute = t->tm_min;
	datetime->second = t->tm_sec;

//	LOGI("mr_getDatetime [%d/%d/%d %d:%d:%d]",
//			t->tm_year+1900, t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);

	return MR_SUCCESS;
}

/*任务睡眠，单位ms*/
int32 mr_sleep(uint32 ms)
{
	if(showApiLog) 
		LOGI("mr_sleep(%d)", ms);

	usleep(ms * 1000); //注意 usleep 传的是 微秒 ，所以要 *1000

	return MR_SUCCESS;
}


///////////////////////// 文件操作接口 //////////////////////////////////////
static char SDPath[DSM_MAX_FILE_LEN/2 + 1] = SDCARD_PATH; /*SD卡根路径 gb 编码*/
static char dsmPath[DSM_MAX_FILE_LEN/2 + 1] = DSM_ROOT_PATH;   /*mythroad工作目录（相对于SD卡路径） gb 编码*/
static char dsmWorkPath[DSM_MAX_FILE_LEN/2 + 1] = DSM_ROOT_PATH; /*当前工作路径 gb 编码*/

static char filenamebuf[DSM_MAX_FILE_LEN + 1] = {0};

#define CHAR_DIR_SEP '/'


/**
 * 创建运行时需要的目录
 */
void DsmPathInit(void)
{
	char buf[DSM_MAX_FILE_LEN*2 + 1] = {0};
	char buf2[DSM_MAX_FILE_LEN*2 + 1] = {0};

	snprintf(buf, sizeof(buf), "%s", SDPath); //sd卡根目录
	GBToUTF8String((uint8 *)buf, (uint8 *)buf2, sizeof(buf2));
	if(showApiLog) LOGI("checkdir %s", buf2);
	if(MR_IS_DIR != getFileType(buf2))
		mkdir(buf2, 0777);

	snprintf(buf, sizeof(buf), "%s%s", SDPath, dsmPath); //mythroad 目录
	GBToUTF8String((uint8 *)buf, (uint8 *)buf2, sizeof(buf2));
	if(showApiLog) LOGI("checkdir %s", buf2);
	if(MR_IS_DIR != getFileType(buf2))
		mkdir(buf2, 0777);

	snprintf(buf, sizeof(buf), "%s%s%s", SDPath, dsmPath, DSM_HIDE_DRIVE);
	GBToUTF8String((uint8 *)buf, (uint8 *)buf2, sizeof(buf2));
	if(showApiLog) LOGI("checkdir %s", buf2);
	if(MR_IS_DIR != getFileType(buf2))
		mkdir(buf2, 0777);

	snprintf(buf, sizeof(buf), "%s%s%s%s", SDPath, dsmPath, DSM_HIDE_DRIVE, DSM_DRIVE_A);
	GBToUTF8String((uint8 *)buf, (uint8 *)buf2, sizeof(buf2));
	if(showApiLog) LOGI("checkdir %s", buf2);
	if(MR_IS_DIR != getFileType(buf2))
		mkdir(buf2, 0777);

	snprintf(buf, sizeof(buf), "%s%s%s%s", SDPath, dsmPath, DSM_HIDE_DRIVE, DSM_DRIVE_B);
	GBToUTF8String((uint8 *)buf, (uint8 *)buf2, sizeof(buf2));
	if(showApiLog) LOGI("checkdir %s", buf2);
	if(MR_IS_DIR != getFileType(buf2))
		mkdir(buf2, 0777);

	snprintf(buf, sizeof(buf), "%s%s%s%s%s", SDPath, dsmPath, DSM_HIDE_DRIVE, DSM_DRIVE_A, DSM_ROOT_PATH_SYS);
	GBToUTF8String((uint8 *)buf, (uint8 *)buf2, sizeof(buf2));
	if(showApiLog) LOGI("checkdir %s", buf2);
	if(MR_IS_DIR != getFileType(buf2))
		mkdir(buf2, 0777);
}

/*
 * 整理路径，将分隔符统一为sep，并清除连续的多个
 *
 * 参数：路径(必须可读写)
 */
char *FormatPathString(char *path, char sep)
{
	char *p, *q;
	int flag = 0;

	if (NULL == path)
		return NULL;

	for (p = q = path; '\0' != *p; p++)
	{
		if ('\\' == *p || '/' == *p)
		{
			if (0 == flag)
				*q++ = sep;
			flag = 1;
		}
		else
		{
			*q++ = *p;
			flag = 0;
		}
	}

	*q = '\0';

	return path;
}

/**
 * 设置 SD卡路径
 */
void SetDsmSDPath(const char * path)
{
	LOGI("old sdpath %s", SDPath);

	strncpy(SDPath, path, sizeof(SDPath)-1);
	FormatPathString(SDPath, CHAR_DIR_SEP);

	int l = strlen(SDPath);
	if(SDPath[l-1] != '/'){
		SDPath[l] = '/';
		SDPath[l+1] = '\0';
	}
	LOGI("new sdpath %s", SDPath);

	char gbBuf[DSM_MAX_FILE_LEN + 1];
	UTF8ToGBString(SDPath, gbBuf, DSM_MAX_FILE_LEN);

	strncpy(SDPath, gbBuf, sizeof(SDPath)-1);
}

/**
 * 设置 mythroad 工作目录 外部调用
 */
void SetDsmPath(const char *path)
{
	LOGI("old dsmpath %s", dsmPath);
	strncpy(dsmPath, path, sizeof(dsmPath)-1);
	FormatPathString(dsmPath, '/');

	int l = strlen(dsmPath);
	if(dsmPath[l-1] != '/'){
		dsmPath[l] = '/';
		dsmPath[l+1] = '\0';
	}
	LOGI("new dsmpath %s", dsmPath);

	char gbBuf[DSM_MAX_FILE_LEN + 1];
	UTF8ToGBString(dsmPath, gbBuf, DSM_MAX_FILE_LEN);

	strncpy(dsmPath, gbBuf, sizeof(dsmPath)-1);
	strcpy(dsmWorkPath, dsmPath);
}

static void SetDsmWorkPath_inner(const char *path)
{
	strncpy(dsmWorkPath, path, sizeof(dsmWorkPath)-1);
	FormatPathString(dsmWorkPath, '/');

	int l = strlen(dsmWorkPath);
	if(dsmWorkPath[l-1] != '/'){
		dsmWorkPath[l] = '/';
		dsmWorkPath[l+1] = '\0';
	}
	//检查并创建目录
}

const char *GetDsmWorkPath(void)
{
	return dsmWorkPath;
}

const char * GetDsmSDPath()
{
	return SDPath;
}

/****************************************************************************
 函数名:static void dsmToLaunchDir(void)
 描  述:将操作路径返回到刚启动时候的路径
 参  数:无
 返  回:无
 ****************************************************************************/
static void dsmToLaunchDir(void)
{
	strcpy(dsmWorkPath, dsmPath);
}

/****************************************************************************
函数名:static int32 dsmSwitchPath(uint8* input, int32 input_len, uint8** output, int32* output_len)
描  述:VM 对路径操作的接口
参  数:
返  回:
****************************************************************************/
static int32 dsmSwitchPath(uint8* input, int32 input_len, uint8** output, int32* output_len)
{
	if(gEmuEnv.showFile)
		LOGI("dsmSwitchPath %s,%d, %p,%p", input, input_len, output, output_len);

	if(input ==  NULL)
		return MR_FAILED;

	input_len = mr_strlen((char *)input);
	if(input_len > (DSM_MAX_FILE_LEN-3))
		return MR_FAILED;

//	LOGI("dsmSwitchPath(%s, %d)", input, input_len);

	switch(input[0])
	{
	case 'Z': //返回刚启动时路径
	case 'z':
		dsmToLaunchDir();
		break;

	case 'Y': //获取当前工作绝对路径
	case 'y':
		{
			static char buf[DSM_MAX_FILE_LEN];

			memset(buf, 0, sizeof(buf));

			char *p;

			/**
			 * 此处用 c:/ a:/ b:/ 代替真正SD卡路径，避免mrp层空间不足死机
			 */
			if((p = strstr(dsmWorkPath, DSM_HIDE_DRIVE)) != NULL){ //在A盘下
				p += strlen(DSM_HIDE_DRIVE); //a/...
				if(p){
					if(*(p + 2))
						snprintf(buf, sizeof(buf), "%c:/%s", *p, (p+2));
					else
						snprintf(buf, sizeof(buf), "%c:/", *p);
				}else{
					//说明不是 .disk/a/ 形式，未知错误
					LOGE("dsmWorkPath ERROR!");

					strcpy(buf, "a:/"); //给他个默认值
				}
			}else{
				snprintf(buf, sizeof(buf), "c:/%s", dsmWorkPath);
			}

//			LOGI("dsmWorkPath:%s", buf);

			*output = (uint8 *)buf;
			*output_len = strlen(buf);

			break;	
		}

	case 'X': //进入DSM隐藏根目录
	case 'x':
		{
			snprintf(filenamebuf, sizeof(filenamebuf), "%s%s%s%s",
					dsmPath, DSM_HIDE_DRIVE, DSM_DRIVE_A, DSM_ROOT_PATH_SYS);
			SetDsmWorkPath_inner(filenamebuf);
			break;
		}

	default:
		{
			char buf[DSM_MAX_FILE_LEN + 1] = {0};

			if(input_len > DSM_MAX_FILE_LEN )
				return MR_FAILED;

			if(*input == 'A' || *input == 'a'){ //A 盘
				if(input_len > 3){
					sprintf(buf, "%s%s%s%s", dsmPath, DSM_HIDE_DRIVE, DSM_DRIVE_A, input+3);
				}else{
					sprintf(buf, "%s%s%s", dsmPath, DSM_HIDE_DRIVE, DSM_DRIVE_A);
				}
				SetDsmWorkPath_inner(buf);
			}else if(*input == 'B' || *input == 'b'){ //B 盘
				if(input_len > 3){
					sprintf(buf, "%s%s%s%s", dsmPath, DSM_HIDE_DRIVE, DSM_DRIVE_B, input+3);
				}else{
					sprintf(buf, "%s%s%s", dsmPath, DSM_HIDE_DRIVE, DSM_DRIVE_B);
				}
				SetDsmWorkPath_inner(buf);
			}else{ //C 盘
				if(input_len > 3){
					sprintf(buf, "%s", input+3);
					SetDsmWorkPath_inner(buf);
				}else{
					SetDsmWorkPath_inner("");
				}
			}
			break;
		}
	}

	return MR_SUCCESS;
}


/****************************************************************************
 函数名:char* get_filename(char* outputbuf,const char *filename)
 描  述:由相对路径的文件名接成绝对路径名
 参  数:filename:相对路径的文件名
 outputbuf:转换好的绝对路径文件名(outputbuf的大小要大于等于DSM_MAX_FILE_LEN * ENCODING_LENGTH)
 返  回:绝对路径的文件名
 ****************************************************************************/
char* get_filename(char * outputbuf, const char *filename)
{
	char dsmFullPath[DSM_MAX_FILE_LEN + 1];
	char *p = outputbuf;

	snprintf(dsmFullPath, sizeof(dsmFullPath),
			"%s%s%s", SDPath, dsmWorkPath, filename);
	FormatPathString(dsmFullPath, '/');
	GBToUTF8String(dsmFullPath, outputbuf, DSM_MAX_FILE_LEN);
	
#if 0
	//监控某些文件
	if(strstr(outputbuf, "system/userinfo2.sys")) {
		strcat(outputbuf, g_procInfo.name);
		LOGD("change to %s", outputbuf);
	}
#endif

	return outputbuf;
}

int startWith(const char *str, const char *s)
{
	int l = strlen(s);
	//若参数s1 和s2 字符串相同则返回0。s1 若大于s2 则返回大于0 的值，s1 若小于s2 则返回小于0 的值。
	return (l>0 &&  0 == strncasecmp(str, s, l));
}

/****************************************************************************
 函数名:MR_FILE_HANDLE mr_open(const char* filename,  uint32 mode)
 描  述:打开一个文件
 参  数:filename:文件名
 mode:打开方式
 返  回:文件句柄
 ****************************************************************************/ ///
MR_FILE_HANDLE mr_open(const char* filename, uint32 mode)
{
	int f;
	int new_mode = 0;
	char fullpathname[DSM_MAX_FILE_LEN] = { 0 };

	if (mode & MR_FILE_RDONLY)
		new_mode = O_RDONLY;
	if (mode & MR_FILE_WRONLY)
		new_mode = O_WRONLY;
	if (mode & MR_FILE_RDWR)
		new_mode = O_RDWR;

	//如果文件存在 带此标志会导致错误
	if ((mode & MR_FILE_CREATE) && (0 != access(fullpathname, F_OK)))
		new_mode |= O_CREAT;

	//返回值：成功则返回文件描述符，否则返回 -1
	f = open(get_filename(fullpathname, filename), new_mode, 0777);
	//返回 0 也是成功的啊我擦
//	if(f == 0) LOGW("open fd = 0");
	if (f < 0){
//		EEXIST
		//大部分 mrp 通过此法检测文件是否存在，打印日志太多注释
//		LOGE("mr_open(%s,%d(%d)) err, e=%d", fullpathname, new_mode, mode, errno);
		return (MR_FILE_HANDLE)NULL;
	}

	f += 5; //因为 linux 返回0也成功，mrp返回0 为失败！所以统一加5
	if(gEmuEnv.showFile)
		LOGI("mr_open(%s,%d) fd is: %d", fullpathname, new_mode, f);

	return (MR_FILE_HANDLE) (f);
}

/****************************************************************************
 函数名:int32 mr_close(MR_FILE_HANDLE f)
 描  述:关闭一个文件
 参  数:f:要关闭得文件得句柄
 返  回:NR_SUCCESS,MR_FAILED
 ****************************************************************************/
int32 mr_close(MR_FILE_HANDLE f)
{
	if(f == 0)
		return MR_FAILED;

	int ret;

	ret = close(f-5);
	if (ret != 0){
		LOGE("mr_close(%d) err, %d", f, errno);
		return MR_FAILED;
	}

	if(gEmuEnv.showFile)
		LOGI("mr_close(%d) suc", f);

	return MR_SUCCESS;
}

/****************************************************************************
 函数名:int32 mr_read(MR_FILE_HANDLE f,void *p,uint32 l)
 描  述:读取文件中得数据
 参  数:f:要读得文件得句柄
 p:缓存得指针
 l:缓存得大小
 返  回:
 ****************************************************************************/
int32 mr_read(MR_FILE_HANDLE f, void *p, uint32 l)
{
    if(gEmuEnv.showFile)
        LOGI("mr_read %d,%p,%d", f, p, l);
	if(f < 5)
		return MR_FAILED;

	size_t readnum;

	readnum = read(f-5, p, (size_t) l);
	if (readnum < 0){
		LOGE("mr_read(%d) err, %d", f, errno);
		return MR_FAILED;
	}

	return (int32) readnum;
}

/****************************************************************************
 函数名:int32 mr_write(MR_FILE_HANDLE f,void *p,uint32 l)
 描  述:往一个文件中写入数据
 参  数:f:要写入得文件得句柄
 p:缓存得指针
 l:要写入数据得大小
 返  回:
 ****************************************************************************/
int32 mr_write(MR_FILE_HANDLE f, void *p, uint32 l)
{
    if(gEmuEnv.showFile)
        LOGI("mr_read %d,%p,%d", f, p, l);

	if(f < 0)
		return MR_FAILED;

	size_t writenum = 0;

	writenum = write(f-5, p, (size_t) l);

	if (writenum < 0){
		LOGE("mr_write(%d) err, %d", f, errno);
		return MR_FAILED;
	}

	return writenum;
}

/****************************************************************************
 函数名:int32 mr_seek(MR_FILE_HANDLE f, int32 pos, int method)
 描  述:偏移文件读写指针
 参  数:f     :文件句柄
 pos   :要偏移得数量
 method:偏移起算的位置
 返  回:MR_SUCCESS,MR_FAILED
 ****************************************************************************/
int32 mr_seek(MR_FILE_HANDLE f, int32 pos, int method)
{
	if(f == 0)
		return MR_FAILED;

	off_t ret;

	ret = lseek(f-5, (off_t) pos, method);
	if (ret < 0) {
		LOGE("mr_seek(%d,%d) err, %d", f, pos, errno);
		return MR_FAILED;
	}

	return MR_SUCCESS;
}

/****************************************************************************
 函数名:int32 mr_info(const char* filename)
 描  述:得到一个文件信息
 参  数:filename
 返  回:是文件:MR_IS_FILE
 是目录:MR_IS_DIR
 无效:  MR_IS_INVALID
 ****************************************************************************/
int32 mr_info(const char* filename)
{
	char fullpathname[DSM_MAX_FILE_LEN] = { 0 };
	struct stat s1;
	int ret;

	//返回 0 成功
	ret = stat(get_filename(fullpathname, filename), &s1);
	if(gEmuEnv.showFile)
		LOGI("mr_info(%s)", fullpathname);

	if (ret != 0){
		if(gEmuEnv.showFile) LOGI("  is err");
		return MR_IS_INVALID;
	}

	if (s1.st_mode & S_IFDIR){
		if(gEmuEnv.showFile) LOGI("  is dir");
		return MR_IS_DIR;
	} else if (s1.st_mode & S_IFREG){
		if(gEmuEnv.showFile) LOGI("  is file");
		return MR_IS_FILE;
	} else {
		if(gEmuEnv.showFile) LOGI("  is other");
		return MR_IS_INVALID;
	}
}

/****************************************************************************
 函数名:int32 mr_remove(const char* filename)
 描  述:删除一个文件
 参  数:filename:要被删除的文件的文件名
 返  回:MR_SUCCESS,MR_FAILED
 ****************************************************************************/
int32 mr_remove(const char* filename)
{
	char fullpathname[DSM_MAX_FILE_LEN] = { 0 };
	int ret;

	ret = remove(get_filename(fullpathname, filename));
	if(ret != 0 && errno != 2){
		LOGE("mr_remove(%s) err, ret=%d, errno=%d", fullpathname, ret, errno);
		return MR_FAILED;
	}

	if(gEmuEnv.showFile)
		LOGI("mr_remove(%s) suc", fullpathname);

	return MR_SUCCESS;
}

/****************************************************************************
 函数名:int32 mr_rename(const char* oldname, const char* newname)
 描  述:对一个文件进行重命名
 参  数:oldname:原文件名
 newname:新文件名
 返  回:MR_SUCCESS,MR_FAILED
 ****************************************************************************/
int32 mr_rename(const char* oldname, const char* newname)
{
	char fullpathname_1[DSM_MAX_FILE_LEN] = { 0 };
	char fullpathname_2[DSM_MAX_FILE_LEN] = { 0 };
	int ret;

	if(gEmuEnv.showFile)
		LOGI("mr_rename(%s to %s)", oldname, newname);

	get_filename(fullpathname_1, oldname);
	get_filename(fullpathname_2, newname);
	ret = rename(fullpathname_1, fullpathname_2);
	if(ret != 0){
		LOGE("mr_rename(%s to %s) err! errno=%d", fullpathname_1, fullpathname_2, errno);
		return MR_FAILED;
	}
	if(gEmuEnv.showFile)
		LOGI("  suc");

	return MR_SUCCESS;
}

/****************************************************************************
 函数名:int32 mr_mkDir(const char* name)
 描  述:创建一个目录
 参  数:name:目录名
 返  回:MR_SUCCESS,MR_FAILED
 ****************************************************************************/
int32 mr_mkDir(const char* name)
{
	char fullpathname[DSM_MAX_FILE_LEN] = { 0 };
	int ret;

	get_filename(fullpathname, name);
	if(access(fullpathname, F_OK) == 0){ //检测是否已存在
		goto ok;
	}

	ret = mkdir(fullpathname, 0777);
	if (ret != 0){
		LOGE("mr_mkDir(%s) err!", fullpathname);
		return MR_FAILED;
	}
ok:
	if(gEmuEnv.showFile)
		LOGI("mr_mkDir(%s) suc!", fullpathname);

	return MR_SUCCESS;
}

/****************************************************************************
 函数名:int32 mr_rmDir(const char* name)
 描  述:删除一个目录
 参  数:name:被删除的目录名
 返  回:MR_SUCCESS,MR_FAILED
 ****************************************************************************/
int32 mr_rmDir(const char* name)
{
	char fullpathname[DSM_MAX_FILE_LEN] = { 0 };
	char fullpathname2[DSM_MAX_FILE_LEN] = { 0 };
	int ret;

	get_filename(fullpathname, name);

	//删除权限
	if(strcasecmp(fullpathname, SDCARD_PATH) == 0){
		LOGE("Has no right to delete this directory:%s ", fullpathname);
		return MR_FAILED;
	}

	sprintf(fullpathname2, "%s%s", SDCARD_PATH, DSM_ROOT_PATH);
	if(strcasecmp(fullpathname, fullpathname2) == 0){
		LOGE("Has no right to delete this directory:%s ", fullpathname);
		return MR_FAILED;
	}

	ret = rmdir(fullpathname);
	if(ret != 0){
		LOGE("mr_rmDir(%s) err!", fullpathname);
		return MR_FAILED;
	}

	if(gEmuEnv.showFile)
		LOGI("mr_rmDir(%s) suc!", fullpathname);

	return MR_SUCCESS;
}

/****************************************************************************
 函数名:MR_FILE_HANDLE mr_findStart(const char* name, char* buffer, uint32 len)
 描  述:初始化一个文件目录的搜索，并返回第一搜索。
 参  数:name	 :要搜索的目录名
 buffer:保存第一个搜索结果的buf
 len   :buf的大小
 返  回:成功:第一个搜索结果的句柄
 失败:MR_FAILED
 ****************************************************************************/
MR_FILE_HANDLE mr_findStart(const char* name, char* buffer, uint32 len)
{
	if(!name || !buffer || len==0)
		return MR_FAILED;

	if(gEmuEnv.showFile)
		LOGI("mr_findStart %s", name);

	DIR *pDir;
	struct dirent *pDt;
	char fullpathname[DSM_MAX_FILE_LEN] = { 0 };

	memset(buffer, 0, len);
	if((pDir = opendir(get_filename(fullpathname, name))) != NULL) {
		if((pDt = readdir(pDir)) != NULL) {
			UTF8ToGBString(pDt->d_name, buffer, len);
		} else {
			LOGW("mr_findStart: readdir FAIL!");
		}

		return (MR_FILE_HANDLE) pDir;
	} else {
		LOGE("mr_findStart %s: opendir FAIL!", fullpathname);
	}

	return MR_FAILED;
}

/****************************************************************************
 函数名:int32 mr_findGetNext(MR_FILE_HANDLE search_handle, char* buffer, uint32 len)
 描  述:搜索目录的下一个结果
 参  数:search_handle :目录的句柄
 buffer        :存放搜索结果的buf
 len           :buf的大小
 返  回:MR_SUCCESS,MR_FAILED
 ****************************************************************************/
int32 mr_findGetNext(MR_FILE_HANDLE search_handle, char* buffer, uint32 len)
{
	if(!search_handle || search_handle == MR_FAILED || !buffer || len==0)
		return MR_FAILED;

	if(gEmuEnv.showFile)
		LOGI("mr_findGetNext %d", search_handle);

	DIR *pDir = (DIR *)search_handle;
	struct dirent *pDt;

	memset(buffer, 0, len);
	if((pDt = readdir(pDir)) != NULL){
		if(gEmuEnv.showFile)
			LOGI("mr_findGetNext %s", pDt->d_name);

		UTF8ToGBString(pDt->d_name, buffer, len);
		return MR_SUCCESS;
	} else {
		//查找完毕
		if(gEmuEnv.showFile)
			LOGI("mr_findGetNext end");
	}

	return MR_FAILED;
}

/****************************************************************************
 函数名:int32 mr_findStop(MR_SEARCH_HANDLE search_handle)
 描  述:停止当前的搜索
 参  数:search_handle:搜索句柄
 返  回:MR_SUCCESS,MR_FAILED
 ****************************************************************************/
int32 mr_findStop(MR_SEARCH_HANDLE search_handle)
{
	if(!search_handle || search_handle == MR_FAILED)
		return MR_FAILED;

	DIR *pDir = (DIR *)search_handle;
	closedir(pDir);

	return MR_SUCCESS;
}

/****************************************************************************
 函数名:int32 mr_ferrno(void)
 描  述:该函数用于调试使用，返回的是最后一次操作文件失败的错误信息，返回的错误
 信息具体含义与平台上使用的文件系统有关。
 参  数:无
 返  回:MR_SUCCESS,MR_FAILED
 ****************************************************************************/
int32 mr_ferrno(void)
{
	return (int32) MR_FAILED;
}

/****************************************************************************
 函数名:int32 mr_getLen(const char* filename)
 描  述:得到指定文件得大小
 参  数:filename:所指定得文件名
 返  回:成功返回文件大小
 失败返回:MR_FAILED
 ****************************************************************************/
int32 mr_getLen(const char* filename)
{
	char fullpathname[DSM_MAX_FILE_LEN] = { 0 };
	struct stat s1;
	int ret;

	ret = stat(get_filename(fullpathname, filename), &s1);

	if (ret != 0)
		return -1;

	return s1.st_size;
}

static 
int32 dsmGetFreeSpace(uint8 *input, int32 input_len, T_DSM_DISK_INFO *spaceInfo)
{
	U64 disk_free_space, disk_total_space;
	int32 fs_ret;
	struct statfs disk_statfs;

	disk_free_space	= 0;
	disk_total_space = 0;

	fs_ret = statfs(GetDsmSDPath(), &disk_statfs);

//	if(showApiLog)
//		LOGI("dsmGetFreeSpace() ret=%d", fs_ret);

	if(fs_ret == 0)
	{
		//disk_free_space	= (U64)1024*1024*1024*2;
		//disk_total_space  = (U64)1024*1024*1024;
		disk_total_space = disk_statfs.f_blocks * disk_statfs.f_bsize;
		disk_free_space = disk_statfs.f_bfree * disk_statfs.f_bsize;

		if((disk_total_space/10) >(U64)1024*1024*1024)
		{
			spaceInfo->tUnit = 1024*1024*1024;
		}
		else if((disk_total_space/10) > 1024*1024)
		{
			spaceInfo->tUnit = 1024*1024;
		}
		else if((disk_total_space/10) > 1024)
		{
			spaceInfo->tUnit = 1024;
		}
		else 
		{
			spaceInfo->tUnit = 1;
		}

		spaceInfo->total = disk_total_space/spaceInfo->tUnit;

		if((disk_free_space/10) >(U64)1024*1024*1024)
		{
			spaceInfo->unit = 1024*1024*1024;
		}
		else if((disk_free_space/10) > 1024*1024)
		{
			spaceInfo->unit = 1024*1024;
		}
		else if((disk_free_space/10) > 1024)
		{
			spaceInfo->unit = 1024;
		}
		else 
		{
			spaceInfo->unit = 1;
		}

		spaceInfo->account = disk_free_space/spaceInfo->unit;

		return MR_SUCCESS;
	}

	return MR_FAILED;
}

int32 mr_getScreenInfo(mr_screeninfo *s)
{
	//if(showApiLog) LOGI("mr_getScreenInfo");
	if(s){
		s->width = SCNW;
		s->height = SCNH;
		s->bit = SCNBIT;
	}
	return MR_SUCCESS;
}

void mr_drawBitmap(uint16* bmp, int16 x, int16 y, uint16 w, uint16 h)
{
	emu_bitmapToscreen(bmp, x, y, w, h);
}

const char *mr_getCharBitmap(uint16 ch, uint16 fontSize, int *width, int *height)
{
	if(gEmuEnv.androidDrawChar) {
		emu_measureChar(ch, width, height);
//		return tsf_getCharBitmapA(ch);
		return xl_font_sky16_getChar(ch);
	} else {
		xl_font_sky16_charWidthHeight(ch, width, height);
//		tsf_charWidthHeight(ch, width, height);
//		return tsf_getCharBitmapA(ch);
		return xl_font_sky16_getChar(ch);
	}
}

void mr_platDrawCharReal(uint16 ch, int32 x, int32 y, uint16 color)
{
	if(gEmuEnv.androidDrawChar) {
		emu_drawChar(ch, x, y, color);
	} else {
//		tsf_drawChar(ch, x, y, color);
		xl_font_sky16_drawChar(ch,x,y,color);
	}
}

int32 mr_startShake(int32 ms){
	emu_startShake(ms);
	return MR_SUCCESS;
}

int32 mr_stopShake(){
	emu_stopShake();
	return MR_SUCCESS;
}

const static char exts[][5] = {
	".mid", ".wav", ".mp3", ".amr"
};

int32 mr_playSound(int type, const void* data, uint32 dataLen, int32 loop)
{
	if (type >= 4)
	{
		LOGW("sound id %d not support!", type);
		return MR_FAILED;
	}

	int32 fd;

	if (MR_IS_DIR != mr_info(".tmp"))
		mr_mkDir(".tmp");

	char buf[128];
	sprintf(buf, ".tmp/tmp%s", exts[type]);

	MR_CHECK_AND_REMOVE(buf);

	fd = mr_open(buf, MR_FILE_RDWR | MR_FILE_CREATE);
	if (fd > 0)
	{
		mr_write(fd, (void *)data, dataLen);
		mr_close(fd);
	}

	char fullpathname[DSM_MAX_FILE_LEN] = { 0 };
	emu_palySound(get_filename(fullpathname, buf), loop);

	return MR_SUCCESS;
}

int32 mr_stopSound (int type){
	emu_stopSound(type);
	return MR_SUCCESS;
}

int32 mr_sendSms(char* pNumber, char*pContent, int32 encode){
	if(showApiLog) {
		LOGI("mr_sendSms(%s)", pNumber);
	}

	emu_sendSms(pNumber, pContent, encode);

	return MR_SUCCESS;
}
void mr_call(char *number){
	if(showApiLog) LOGI("mr_call(%s)", number);

	const char *buf[2];
	buf[0] = "call";
	buf[1] = number;
	N2J_callVoidMethodString(2, buf);
}

int32 mr_getNetworkID(void){
	if(showApiLog) LOGI("mr_getNetworkID %d", dsmNetWorkID);
	return dsmNetWorkID;
}

void mr_connectWAP(char *wap){
	if(showApiLog) LOGI("mr_connectWAP(%s)", wap);

	const char *buf[2];
	buf[0] = "viewUrl";
	buf[1] = wap;
	N2J_callVoidMethodString(2, buf);
}

char * unibe_unile(const char *s)
{
	if(s == NULL)
		return NULL;

	int l = UCS2_strlen(s);
	char *buf = malloc(l+2);

	memset(buf, 0, l + 2);
	memcpy(buf, s, l);
	UCS2ByteRev(buf); //unicode大端转小端

//	jtitle = (*jniEnv)->NewString(jniEnv, (const jchar*) buf, l / 2);

	return buf;
}

int32 mr_menuCreate(const char* title, int16 num) {
#if 0
	if(num <= 0)
		return MR_FAILED;

	T_MR_MENU *menu = (T_MR_MENU *)malloc(sizeof(T_MR_MENU));
	menu->itemCount = num;
	menu->items = malloc(sizeof(char *) * num);
	menu->title = unibe_unile(title);

	return (int32)menu;
#else
	return MR_FAILED;
#endif
}

int32 mr_menuSetItem(int32 hd, const char *text, int32 index) {
#if 0
	T_MR_MENU *menu = (T_MR_MENU *)hd;

	if(index < 0 || index > menu->itemCount-1)
		return MR_FAILED;

	menu->items[index] = unibe_unile(text);

	return MR_SUCCESS;
#else
	return MR_FAILED;
#endif
}

int32 mr_menuShow(int32 menu) {
	return MR_IGNORE;
}

int32 mr_menuSetFocus(int32 menu, int32 index){
	return MR_IGNORE;
}

int32 mr_menuRelease(int32 menu){
	return MR_IGNORE;
}

int32 mr_menuRefresh(int32 menu){
	return MR_IGNORE;
}

int32 mr_dialogCreate(const char * title, const char * text, int32 type){
	return MR_IGNORE;
}

int32 mr_dialogRelease(int32 dialog){
	return MR_IGNORE;
}

int32 mr_dialogRefresh(int32 dialog, const char * title, const char * text, int32 type){
	return MR_FAILED;
}

int32 mr_textCreate(const char * title, const char * text, int32 type){
	return MR_IGNORE;
}

int32 mr_textRelease(int32 text){
	return MR_IGNORE;
}

int32 mr_textRefresh(int32 handle, const char * title, const char * text){
	return MR_IGNORE;
}

int32 mr_editCreate(const char * title, const char * text, int32 type, int32 max_size){
	return emu_showEdit(title, text, type, max_size);
}

int32 mr_editRelease(int32 edit){
//	LOGI("mr_editRelease %d", edit);
	emu_releaseEdit(edit);
	return MR_SUCCESS;
}

const char* mr_editGetText(int32 edit){
	return emu_getEditInputContent(edit);
}

int32 mr_winCreate(void){
	return MR_IGNORE;

}
int32 mr_winRelease(int32 win){
	return MR_IGNORE;
}

void dsmDrawImage(T_DRAW_DIRECT_REQ *t) {
	if(t->src_type == SRC_NAME) {
		char fullpathname[DSM_MAX_FILE_LEN] = { 0 };
		emu_drawImage(get_filename(fullpathname, t->src), t->ox, t->oy, t->w, t->h);
	}
}

MRAPP_IMAGE_SIZE_T *dsmGetImageSize(MRAPP_IMAGE_ORIGIN_T *t) {
	if(t->src_type == SRC_NAME) {
		char fullpathname[DSM_MAX_FILE_LEN] = { 0 };
		emu_getImageSize(get_filename(fullpathname, t->src), &dsmImageSize.width, &dsmImageSize.height);
	}

	return &dsmImageSize;
}

//----------------------------------------------------
/*平台扩展接口*/
int32 mr_plat(int32 code, int32 param)
{
	if(gEmuEnv.showMrPlat)
        LOGI("mr_plat code=%d param=%d", code, param);

	switch(code)
	{
	case MR_GET_FILE_POS: //获取文件读写指针
		{
			int32 ret = lseek(param-5, 0, SEEK_CUR);
			if (ret >= 0)
				return (ret + MR_PLAT_VALUE_BASE);
			else
				return MR_FAILED;
		}

	case  MR_CONNECT:
		{
			return mr_getSocketState(param);
		}		

	case  MR_SET_SOCTIME:
		{
			return MR_IGNORE;
		}

	case MR_GET_RAND://1211
		{
			srand(mr_getTime());
			return (MR_PLAT_VALUE_BASE+rand()%param);
		}

	case MR_CHECK_TOUCH: //是否支持触屏
		{
			return MR_NORMAL_SCREEN;
		}

	case MR_GET_HANDSET_LG: //获取语言
		{
			return MR_CHINESE;
		}

	case 1391: //查询后台支持
		{
			return MR_IGNORE;
		}

	case 1106: //获取短信中心
		{
			const char* args[1];
			args[0] = "getSmsCenter";
//			N2J_callVoidMethodString(1, args);

			return MR_WAITING;
		}

	case 1016: //初始化信号强度
		return MR_SUCCESS;

	case 1302: // 设置音量
		emu_musicCMD(1302, param, param);
		return MR_SUCCESS;

	case 1214:
		return MR_SUCCESS;

	case 1327://查询 WIFI 是否可用
		return MR_IGNORE;
		//return (emu_getIntSysinfo(SYSINFO_NETTYPE)==NETTYPE_WIFI? MR_SUCCESS : MR_FAILED);
	case 1328: //设置是否使用 WIFI
		return MR_SUCCESS;

	case 1011: //设置收到新短信的时候是否显示提示界面。1提示
		return MR_SUCCESS;

	case 1218:
		return MR_MSDC_OK;

	default:
		LOGW("mr_plat(code:%d, param:%d) not impl!", code, param);
		break;
	}

	return MR_IGNORE;
}

void writeDataToTempFile(const char *path, void *data, int len)
{
	CHECK_AND_REMOVE(path);
	int fd = open(path, O_RDWR|O_CREAT, 0777);
	if(fd >=0 ){
		write(fd, data, len);
		close(fd);
	}
}

/*增强的平台扩展接口*/
int32 mr_platEx(int32 code, uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
	if(gEmuEnv.showMrPlat)
        LOGI("mr_platEx code=%d ip=%p il=%d op=%p olp=%p cb=%p", code, input, input_len, output, output_len, cb);

	switch(code)
	{
	case MR_MALLOC_EX: //1001申请屏幕缓冲区，这里给的值即 VM 的屏幕缓冲区
		{
			*output = (uint8 *)cacheScreenBuffer;
//			screenBuf = cacheScreenBuffer;
			*output_len = SCNW * SCNH * 2;
            LOGD("mocall ram2 addr=%p l=%d", output, *output_len);
			return MR_SUCCESS;
		}

	case MR_MFREE_EX: //1002
		{
			return MR_SUCCESS;
		}

	case 1012: //申请内部cache
		{
			*output = NULL;
			return MR_SUCCESS;
		}
	case 1013: //释放内部cache
		{
			return MR_SUCCESS;
		}

	case 1014: //申请拓展内存
		{
#if 0
			LOGI("malloc exRam");
			*output_len = 1024*1024*2;
			*output = malloc(*output_len);
			return MR_SUCCESS;
#else
			if(gEmuEnv.enableExram) {
				//pageMalloc((void**)output, output_len, SCNW * SCNH *4);
                *output_len = SCNW * SCNH * 4;
                *output = malloc(*output_len);
                LOGI("malloc exRam addr=%p len=%d", output, output_len);
                return MR_SUCCESS;
			} else {
				*output = NULL;
				*output_len = 0;
				return MR_IGNORE;
			}
#endif
		}

	case 1015: //释放拓展内存
		{
#if 0
			LOGI("free exRam");
			free(input);
			return MR_SUCCESS;
#else
			if(gEmuEnv.enableExram) {
				LOGI("free exRam");
				free(input);
				return MR_SUCCESS;
			}

			return MR_IGNORE;
#endif
		}

	case MR_TUROFFBACKLIGHT: //关闭背光常亮
		{
//			emu_setStringOptions("keepScreenOn", "false");
			return MR_SUCCESS;
		}
	case MR_TURONBACKLIGHT: //开启背光常亮
		{
//			emu_setStringOptions("keepScreenOn", "true");
			return MR_SUCCESS;
		}

	case MR_SWITCHPATH: //切换跟目录 1204
		return dsmSwitchPath(input,input_len,output,output_len);

	case MR_GET_FREE_SPACE:
		{
			if(dsmGetFreeSpace(input, input_len, &dsmDiskInfo) == MR_SUCCESS)
			{
				*output = (uint8 *)&dsmDiskInfo;
				*output_len = sizeof(T_DSM_DISK_INFO);
				return MR_SUCCESS;
			}
			return MR_FAILED;
		}

	case MR_UCS2GB: //1207
		{
			if(!input || input_len == 0){
				LOGE("mr_platEx(1207) input err");
				return MR_FAILED;
			}

			if(!*output){
				LOGE("mr_platEx(1207) ouput err");
				return MR_FAILED;
			}

			//input is bigend
			{
				int len = UCS2_strlen(input);
				char *buf = malloc(len + 2);

				int gbBufLen = len + 1;
				char *gbBuf = malloc(gbBufLen);

				memcpy(buf, input, len+2);
				UCS2ByteRev(buf);
				UCS2ToGBString((uint16*)buf, gbBuf, gbBufLen);

				strcpy(*output, gbBuf);
				/**
				 * output_len 返回的GB字符串缓冲的长度。
				 *
				 * output   	转换成功以后的bg2312编码字符串存放缓冲区指针，缓冲区的内存由应用调用者提供并管理、释放。
				 * output_len   output缓冲区的长度，单位字节数
				 *
				 * 2013-3-25 16:29:21 2013-3-25 16:51:44
				 */
//				if(output_len)
//					*output_len = strlen(gbBuf);
				
				free(buf);
			}

			return MR_SUCCESS;
		}

	case MR_CHARACTER_HEIGHT:
		{
			static int32 wordInfo = (DSM_ENWORD_H<<24 )|  (DSM_ENWORD_W<<16) |  (DSM_CHWORD_H<<8 )|  (DSM_CHWORD_W) ;
			*output = (unsigned char*)&wordInfo;
			*output_len = 4;
			
			return MR_SUCCESS;
		}

	case 1116: //获取编译时间
		{
			static char buf[32];

			int l = snprintf(buf, sizeof(buf), "%s %s", __TIME__, __DATE__);
			*output = (uint8 *)buf;//"2013/3/21 21:36";
			*output_len = l + 1;

			LOGI("build time %s", buf);

			return MR_SUCCESS;
		}

	case 1307: //获取SIM卡个数，非多卡多待直接返回 MR_INGORE
		return MR_IGNORE;

	//case 1224: //T_DSM_CELL_INFO 获取小区信息 需要调用mr_plat中的1215和1216进行初始化和退出 

	case 0x90004: //获取型号强度
		return 5;

	case 1224: //小区信息ID
	{
		memset(&dsmCellInfo, 0, sizeof(T_DSM_CELL_INFO));
		*output = (uint8 *) &dsmCellInfo;
		*output_len = sizeof(T_DSM_CELL_INFO);
		LOGW("get T_DSM_CELL_INFO");
		return MR_IGNORE;
	}

	case 1017: //获得信号强度。
	{
		static T_RX rx = {
			3,5,5,1
		};

		*output = (uint8 *)&rx;
		*output_len = sizeof(T_RX);
		return MR_SUCCESS;
	}

	case 3001: //获取图片信息
	{
		MRAPP_IMAGE_ORIGIN_T *t = (MRAPP_IMAGE_ORIGIN_T *)input;

		*output = (uint8 *)dsmGetImageSize(t);
		*output_len = sizeof(MRAPP_IMAGE_SIZE_T);

		return MR_SUCCESS;
	}

	case 3010: //画图
	{
		T_DRAW_DIRECT_REQ *t = (T_DRAW_DIRECT_REQ *)input;

		dsmDrawImage(t);

		return MR_SUCCESS;
	}

	default:
		{
//			LOGW("mr_platEx(code=%d, input=%#p, il=%d) not impl!", code, input, input_len);

//			if(input_len>0 && input){
//				int fd;
//				if(MR_IS_FILE != mr_info("system.log"))
//					fd = mr_open("system.log", MR_FILE_CREATE|MR_FILE_RDWR);
//				else
//					fd = mr_open("system.log", MR_FILE_RDWR);
//				if(fd){
//					mr_seek(fd, 0, MR_SEEK_END);
//					mr_write(fd, input, input_len);
//					mr_write(fd, "\r\n", 2);
//					mr_close(fd);
//				}
//			}
			break;
		}
	}

	int cmd = code / 10;
	switch (cmd)
	{
	case MR_MEDIA_INIT:
		emu_musicCMD(cmd, 0, 0);
		return MR_SUCCESS;

	case MR_MEDIA_BUF_LOAD:
	{
		char fullpathname[DSM_MAX_FILE_LEN] = { 0 };
		get_filename(fullpathname, "musicbuf");
		writeDataToTempFile(fullpathname, input, input_len);
		emu_musicLoadFile(fullpathname);

		return MR_SUCCESS;
	}

	case MR_MEDIA_FILE_LOAD:
	{
		char fullpathname[DSM_MAX_FILE_LEN] = { 0 };
		emu_musicLoadFile((const char*)get_filename(fullpathname, (const char*)input));

		return MR_SUCCESS;
	}

	case MR_MEDIA_PLAY_CUR_REQ:
	{
		if (input_len >= sizeof(T_DSM_MEDIA_PLAY) && input != NULL){
			LOGI("mediaplay need callback!");

			T_DSM_MEDIA_PLAY *pt = (T_DSM_MEDIA_PLAY *)input;

			dsmMediaPlay.cb = pt->cb;

			emu_musicCMD(MR_MEDIA_PLAY_CUR_REQ, pt->loop, 1);
		}else {
			emu_musicCMD(MR_MEDIA_PLAY_CUR_REQ, 0, 0);
		}

		return MR_SUCCESS;
	}

	case MR_MEDIA_PAUSE_REQ:
	case MR_MEDIA_RESUME_REQ:
	case MR_MEDIA_STOP_REQ:
	case MR_MEDIA_CLOSE:
		emu_musicCMD(cmd, 0, 0);
		return MR_SUCCESS;

	case MR_MEDIA_GET_STATUS:
		return emu_musicCMD(cmd, 0, 0);

	case MR_MEDIA_SETPOS:
	{
		T_SET_PLAY_POS *pos = (T_SET_PLAY_POS *) input;
		emu_musicCMD(cmd, pos->pos * 1000, 0); //上层需要 ms

		return MR_SUCCESS;
	}

	case MR_MEDIA_GET_TOTAL_TIME:
	case MR_MEDIA_GET_CURTIME:
	case MR_MEDIA_GET_CURTIME_MSEC:
	{
		int ret = emu_musicCMD(cmd, 0, 0);
		if (ret == -1)
		{
			LOGE("emu_musicCMD(%d) error", cmd);
			return MR_FAILED;
		}
		else
		{
			dsmCommonRsp.pos = ret;
			*output = (uint8 *) &dsmCommonRsp;
			*output_len = sizeof(int32);
			return MR_SUCCESS;
		}
	}

	case MR_MEDIA_FREE:
		emu_musicCMD(cmd, 0, 0);
		return MR_SUCCESS;

	default:
		LOGW("mr_platEx(code=%d, input=%p, il=%d) not impl!", code, (void *)input, input_len);
		break;
	}

	return MR_IGNORE;
}
