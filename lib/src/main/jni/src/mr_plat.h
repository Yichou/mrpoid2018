/*
 * mr_plat.h
 *
 *  Created on: 2013年9月8日
 *      Author: Yichou
 */

#ifndef MR_PLAT_H_
#define MR_PLAT_H_
//
//#define MR_PLAT_VALUE_BASE 1000    //用于某些返回值的基础值
//
//typedef struct
//{
//	int32 index;
//	int8 type;
//}T_DSM_GET_SMS_INFO_REQ;
//
//typedef struct
//{
//	int32 index;
//	int8 type;
//}T_DSM_DELETE_SMS_REQ;
//typedef enum
//{
//    MR_SMS_NOBOX ,
//    MR_SMS_UNREAD,
//    MR_SMS_INBOX,
//    MR_SMS_OUTBOX,
//    MR_SMS_DRAFTS,
//    MR_SMS_AWAITS,
//    MR_SMS_DATA,
//    MR_SMS_UNSENT,
//    MR_SMS_READED,
//    MR_SMS_SENT
//}MR_MSG_STATUS;
//
//typedef enum
//{
//	MR_SMS_STORAGE_SIM,
//	MR_SMS_STORAGE_ME
//}MR_SMS_STORAGE;
//
//typedef enum
//{
//	MR_SMS_NOT_READY = MR_PLAT_VALUE_BASE,
//	MR_SMS_READY
//}MR_SMS_STATUS;
//typedef enum
//{
//	MR_NORMAL_SCREEN = MR_PLAT_VALUE_BASE,
//	MR_TOUCH_SCREEN,
//	MR_ONLY_TOUCH_SCREEN
//}MR_SCREEN_TYPE;
//
//typedef enum
//{
//	MR_CHINESE = MR_PLAT_VALUE_BASE,
//	MR_ENGLISH,
//}MR_LANGUAGE;
//
//typedef enum
//{
//	MR_IDLE_BG_PAPER,        /*背景*/
//	MR_IDLE_SAVER_PAPER		 /*屏保*/
//}MR_IDLE_PAPER_TYPE;
//
//typedef enum
//{
//	MR_SMS_ACTION_SAVE = MR_PLAT_VALUE_BASE+1,
//	MR_SMS_ACTION_DELETE
//}MR_SMS_ACTION_TYPE;
//
//enum
//{
//	MR_MSDC_NOT_EXIST = MR_PLAT_VALUE_BASE,
//	MR_MSDC_OK,
//	MR_MSDC_NOT_USEFULL /*可能在usb模式导致无法操作t卡*/
//};
//
//
//#define MR_NES_SET_WRITE_ADDR      		102
//#define MR_NES_GET_READ_ADDR       		103
//#define MR_GOTO_BASE_WIN                        104
//#define MR_LIST_CREATE					1
//#define MR_LIST_SET_ITEM				2
//#define MR_SET_ACTIVE_SIM				1004
//#define MR_SET_VOL                             		1302
//#define MR_CONNECT                         		1001
//#define MR_SET_SOCTIME                    		1002
//#define MR_BIND_PORT                                 1003
//#define MR_ACTIVE_APP                                1003
//#define MR_CHARACTER_HEIGHT       		1201
//#define MR_MALLOC_EX                      		1001
//#define MR_MFREE_EX                        		1002
//#define MR_BACKSTAGE                                 1004
//#define MR_SHOW_PIC                                   1005
//#define MR_STOP_SHOW_PIC                        1006
//#define MR_APPEND_SMS                              1007
//#define MR_GET_CHARACTOR_INFO    		1201
//#define MR_SET_EVENT_FLAG            		1202
//#define MR_SEND_MMS                       		1203
//#define MR_SWITCHPATH 			    		1204
//#define MR_CHECK_TOUCH                  		1205
//#define MR_GET_HANDSET_LG            		1206
//#define MR_UCS2GB                             		1207
//#define MR_SET_RING                         		1208
//#define MR_GET_AUDIO_INFO            		1209
//#define MR_GET_KEYPAD_MAP            		1210
//#define MR_GET_RAND					1211
//#define MR_GET_SCENE                                 1213
//#define MR_SET_KEY_END 				1214
//#define MR_GET_CELL_ID_START                  1215
//#define MR_GET_CELL_ID_STOP                    1216
//#define MR_WEATHER_EXIT                           1217
//#define MR_GET_NES_DEFAULT_DIR 		1220
//#define MR_GET_APPLIST_TITLE      		1221
//#define MR_TURONBACKLIGHT				1222
//#define MR_TUROFFBACKLIGHT 			1223
//#define MR_GET_CELL_INFO                          1224
//#define MR_GET_FILE_POS                            1231
//#define MR_GET_FREE_SPACE                       1305
//#define MR_GET_SIM_INFO				1307
//
////meidia 接口编号区
//#define MR_MEDIA_INIT                  		201
//#define MR_MEDIA_BUF_LOAD            		203
//#define MR_MEDIA_FILE_LOAD           		202
//#define MR_MEDIA_PLAY_CUR_REQ    		204
//#define MR_MEDIA_PAUSE_REQ          		205
//#define MR_MEDIA_RESUME_REQ        		206
//#define MR_MEDIA_STOP_REQ            		207
//#define MR_MEDIA_CLOSE                   		208
//#define MR_MEDIA_GET_STATUS        		209
//#define MR_MEDIA_SETPOS                 		210
//#define MR_MEDIA_GETTIME               		211
//#define MR_MEDIA_GET_TOTAL_TIME  		212
//#define MR_MEDIA_GET_CURTIME        		213
//#define MR_MEDIA_GET_CURTIME_MSEC      215
//#define MR_MEDIA_FREE         				216
//#define MR_MEDIA_ALLOC_INRAM         		220
//#define MR_MEDIA_FREE_INRAM			221
//
//#define MR_MEDIA_OPEN_MUTICHANNEL     222
//#define MR_MEDIA_PLAY_MUTICHANNEL      223
//#define MR_MEDIA_STOP_MUTICHANNEL      224
//#define MR_MEDIA_CLOSE_MUTICHANNEL    225
//
//
//
//#define MR_GET_IMG_INFO                 		3001
//#define MR_DECODE_IMG                     		3002
//#define MR_DECODE_STATUS               		3003
//#define MR_GIF_DECODE                      		3004
//#define MR_GIF_RELEASE                    		3005
//#define MR_DRAW_BUFFER		   		3007
//#define MR_GET_ACT_LAYER 				3008
//#define MR_DISPLAY_LCD					3009
//#define ACI_MIDI_DEVICE                   		1
//#define ACI_WAVE_DEVICE                  		2
//#define ACI_MP3_DEVICE                     		3
//#define ACI_AMR_DEVICE                     		4
//#define ACI_PCM_DEVICE			     		5
//#define ACI_M4A_DEVICE                     		6
//#define ACI_AMR_WB_DEVICE                     	7
//#define MR_MEDIA_IDLE                       		1
//#define MR_MEDIA_INITED                   		2
//#define MR_MEDIA_LOADED                  		3
//#define MR_MEDIA_PLAYING                 		4
//#define MR_MEDIA_PAUSED                  		5
//#define MR_MEDIA_SUSPENDED            		6
//#define MR_MEDIA_SUSPENDING          		7
//
//#define MR_GET_FRAME_BUFFER       		1001
//#define MR_SEND_MMS                       		1203
//
////电话本 接口编号区
//#define MR_PHB_OPERATION_BASE 		4000
//
//#define MR_PHB_NONE 					0
//#define MR_PHB_SIM 						1  /*对sim 卡操作*/
//#define MR_PHB_NVM 						2  /* 对nvm操作 */
//#define MR_PHB_BOTH 					3
//
//#define MR_PHB_BY_NAME 				1
//#define MR_PHB_BY_NUMBER 				2
//
//
//
//#define MR_PHB_SEARCH_ENTRY 			(MR_PHB_OPERATION_BASE + 11) //获得 记录通过 电话本排列的顺序index
//#define MR_PHB_SET_ENTRY 				(MR_PHB_OPERATION_BASE + 12) //SET 一条记录
//#define MR_PHB_GET_ENTRY 				(MR_PHB_OPERATION_BASE + 16)//get 一条记录
//#define MR_PHB_COPY_ENTRY 				(MR_PHB_OPERATION_BASE + 17) //拷贝一条记录
//#define MR_PHB_SET_OWNER_ENTRY 		(MR_PHB_OPERATION_BASE + 21) // 添加本机号码记录.指的是 电话本的 MSISDN 区
//#define MR_PHB_GET_OWNER_ENTYR 		(MR_PHB_OPERATION_BASE + 22) // 获得本机号码.
//
//#define MR_PHB_DELETE_ENTRY_ALL  		(MR_PHB_OPERATION_BASE + 31) // 删除所有记录
//
//#define MR_PHB_GET_COUNT 				(MR_PHB_OPERATION_BASE + 32) //得到记录数
//
//#define MR_PHB_GET_STATUS 				(MR_PHB_OPERATION_BASE + 33)
//
//#define MR_PHB_USE_LOCAL 				(MR_PHB_OPERATION_BASE + 41)
//#define MR_PHB_USE_LOCAL_GET_ENTRY 	(MR_PHB_OPERATION_BASE + 42)
//#define MR_PHB_USE_LOCAL_DESTORY 		(MR_PHB_OPERATION_BASE + 43)
////电话本操作返回值
//
//#define MR_PHB_ERROR 					-1
//#define MR_PHB_IDLE 					0
//#define MR_PHB_SUCCESS 				0
//#define MR_PHB_NOT_SUPPORT 			1
//#define MR_PHB_NOT_READY 				2
//#define MR_PHB_NOT_FOUND 				3
//#define MR_PHB_STORAGE_FULL 			4
//#define MR_PHB_NUMBER_TOO_LONG 		5
//#define MR_PHB_OUT_OF_INDEX 			6
//
//#define MR_PHB_MAX_NAME 				(36*2) //暂时定位24个字节,12个汉字.
//#define MR_PHB_MAX_NUMBER 			(48*2)//暂时定位可输入40个字节.40个数字
//#define MR_PHB_MAX_SEARCH_PATTERN 	(60*2)
//
//#define MR_SET_OPERATION_BASE 		(5000)
//#define MR_SET_GETDATETIME 			(MR_SET_OPERATION_BASE + 1)
//#define MR_SET_SETDATETIME 			(MR_SET_OPERATION_BASE + 2)
//
//#define MR_SET_SETALARMMRP	              (MR_SET_OPERATION_BASE+14)
//#define MR_SET_GETWEATHER 	              (MR_SET_OPERATION_BASE+15)
//#define MR_SET_CREATWEATHER  	              (MR_SET_OPERATION_BASE+16)
//#define MR_SET_DESTORYWEATHER  	       (MR_SET_OPERATION_BASE+17)
//#define MR_GETWEATHERNOTIFY                  (MR_SET_OPERATION_BASE+18)
//#define MR_SET_GETWEATHERDESTOP 	       (MR_SET_OPERATION_BASE+19)
//
////LCD 旋转
//#define MR_LCD_ROTATE_NORMAL			0
//#define MR_LCD_ROTATE_90				1
//#define MR_LCD_ROTATE_180				2
//#define MR_LCD_ROTATE_270				3
//#define MR_LCD_MIRROR					4
//#define MR_LCD_MIRROR_ROTATE_90			5
//#define MR_LCD_MIRROR_ROTATE_180		6
//#define MR_LCD_MIRROR_ROTATE_270		7
//
////本地文件浏览
//#define MR_FMGR_OPERATION_BASE 		1400
//#define MR_BROWSE_FMGR_FILTER_INIT 	(MR_FMGR_OPERATION_BASE + 1)
//#define MR_BROWSE_FMGR_FILTER_SET 	(MR_FMGR_OPERATION_BASE + 2)
//#define MR_BROWSE_FMGR_FILTER_CLEAR 	(MR_FMGR_OPERATION_BASE + 3)
//#define MR_BROWSE_FMGR 				(MR_FMGR_OPERATION_BASE + 4)
//#define MR_BROWSE_FMGR_GET_PATH 		(MR_FMGR_OPERATION_BASE + 5)
//#define MR_BROWSE_FMGR_EXIT 			(MR_FMGR_OPERATION_BASE + 6)
//
//#define   MR_FMGR_TYPE_ALL 				1
//#define	  MR_FMGR_TYPE_FOLDER 			2
//#define   MR_FMGR_TYPE_FOLDER_DOT 	3
//#define   MR_FMGR_TYPE_UNKNOW 		4
//
///* image */
//#define 	MR_FMGR_TYPE_BMP 				5
//#define 	MR_FMGR_TYPE_JPG 				6
//#define 	MR_FMGR_TYPE_JPEG 			7
//#define 	MR_FMGR_TYPE_GIF 				8
//#define 	MR_FMGR_TYPE_PNG 				9
//#define 	MR_FMGR_TYPE_EMS 				10
//#define 	MR_FMGR_TYPE_ANM 			11
//#define 	MR_FMGR_TYPE_WBMP 			12
//#define 	MR_FMGR_TYPE_WBM 			13
//
///* audio */
//#define   MR_FMGR_TYPE_IMY 				14
//#define   MR_FMGR_TYPE_MID 				15
//#define   MR_FMGR_TYPE_MIDI 			16
//#define   MR_FMGR_TYPE_WAV 			17
//#define   MR_FMGR_TYPE_AMR 			18
//#define   MR_FMGR_TYPE_AAC 				19
//#define   MR_FMGR_TYPE_DAF 				20
//#define   MR_FMGR_TYPE_VM 				21
//#define   MR_FMGR_TYPE_AWB 			22
//#define   MR_FMGR_TYPE_AIF 				23
//#define   MR_FMGR_TYPE_AIFF 			24
//#define   MR_FMGR_TYPE_AIFC 			25
//#define   MR_FMGR_TYPE_AU 				26
//#define   MR_FMGR_TYPE_SND 				27
//#define   MR_FMGR_TYPE_M4A 			28
//#define   MR_FMGR_TYPE_MMF  			29
//#define   MR_FMGR_TYPE_WMA 			30
///* video */
//#define   MR_FMGR_TYPE_3GP 				31
//#define   MR_FMGR_TYPE_MP4 				32
//#define   MR_FMGR_TYPE_AVI 				33
///* others */
//#define   MR_FMGR_TYPE_JAD 				34
//#define   MR_FMGR_TYPE_JAR 				35
//#define   MR_FMGR_TYPE_VCF 				36
//#define   MR_FMGR_TYPE_VCS 				37
//#define   MR_FMGR_TYPE_THEME 			38
//#define		MR_FMGR_TYPE_MRP 				39
//#define 	MR_FMGR_TYPE_NES 				40
//#define 	MR_FMGR_TYPE_ZIP 				41
//#define   MR_FMGR_TYPE_ZPK                       42
//
////媒体播放器编号
//#define ACI_MIDI_DEVICE                   		1
//#define ACI_WAVE_DEVICE                  		2
//#define ACI_MP3_DEVICE                     		3
//#define ACI_AMR_DEVICE                     		4
//#define ACI_PCM_DEVICE			     		5
//#define ACI_M4A_DEVICE                     		6
//#define ACI_AMR_WB_DEVICE                     	7
//
//#define MR_MEDIA_IDLE                       		1
//#define MR_MEDIA_INITED                   		2
//#define MR_MEDIA_LOADED                  		3
//#define MR_MEDIA_PLAYING                 		4
//#define MR_MEDIA_PAUSED                  		5
//#define MR_MEDIA_SUSPENDED            		6
//#define MR_MEDIA_SUSPENDING          		7
//
//
//#define MR_PHB_NONE 					0
//#define MR_PHB_SIM 						1  /*对sim 卡操作*/
//#define MR_PHB_NVM 						2  /* 对nvm操作 */
//#define MR_PHB_BOTH 					3
//
//#define MR_PHB_BY_NAME 				1
//#define MR_PHB_BY_NUMBER 				2
//
//#define MR_PHB_MAX_NAME 				(36*2) //暂时定位24个字节,12个汉字.
//#define MR_PHB_MAX_NUMBER 			(48*2)//暂时定位可输入40个字节.40个数字
//#define MR_PHB_MAX_SEARCH_PATTERN 	(60*2)
//
////图片绘制结构
//typedef struct
//{
//	uint8* src;
//	uint8* dest;
//	uint16 src_width;
//	uint16 src_height;
//	uint16 src_pitch;
//	uint16 dest_width;
//	uint16 dest_height;
//	uint16 output_clip_x1;
//	uint16 output_clip_y1;
//	uint16 output_clip_x2;
//	uint16 output_clip_y2;
//}mr_img_resize_struct;
//
//
//typedef struct
//{
//	uint16 lac;
//	uint16 cell_id;
//	uint8 mnc[2];
//	uint8 mcc[3];
//	uint8 mnc3[4];
//}T_DSM_CELL_INFO;
//
//
//typedef struct
//{
//	uint32 total;
//	uint32 tUnit;
//	uint32 account;
//	uint32 unit;
//}T_DSM_DISK_INFO;
//
//typedef struct {
//	uint8	mcc[3];	//MCC
//	uint8	mnc[3];	//MNC
//	uint8	lac[2];	//LAC
//	uint16	cell_id;	 //cell ID
//	uint8	is_present;
//}dsm_rr_em_lai_info_t;
//
//typedef struct
//{
//	int32   appid;   //app id
//	uint8   describe[20];     //应用标志符 - "ipqq"
//	uint8*  param;     //预留扩展用
//}mr_backstage_st;
//
//typedef enum
//{
//	DSM_PIC_DESTID_IDLE_ICON,
//	DSM_PIC_DESTID_IDLE_BG
//}mr_pic_destId;
//
// typedef struct
// {
//       int32 appid;
//       int32 time;
//       int32 img_type;
//       int32 img_size;
//       int32 width;
//       int32 height;
//       uint16 color;
//       uint8 *buff;
//	uint8  destId;
//}mr_pic_req;
//
//typedef int32 (*startSkyLapp)(uint8* param);
//
//void mr_registerLappEntry(void* entry);
//
////---------------------------------
//typedef enum {
//	NETTYPE_WIFI=0,
//	NETTYPE_CMWAP=1,
//	NETTYPE_CMNET=2,
//	NETTYPE_UNKNOW=3
//}AND_NETTYPE;
//
//typedef enum
//{
//	DSM_SOC_CLOSE,
//	DSM_SOC_OPEN,
//	DSM_SOC_CONNECTING,
//	DSM_SOC_CONNECTED,
//	DSM_SOC_ERR
//}T_DSM_SOC_STAT_ENUM;
//
//typedef enum
//{
//	DSM_SOC_NOREAD,
//	DSM_SOC_READABLE
//}T_DSM_SOC_READ_STAT;
//
//typedef enum
//{
//	DSM_SOC_NOWRITE,
//	DSM_SOC_WRITEABLE
//}T_DSM_SOC_WRITE_STAT;
//
//
//typedef struct
//{
//	uint8 mod_id;
//	uint8 identifier;
//	int event_id;
//	int result;
//} mr_socket_event_struct;
//
//typedef struct
//{
//	int socketId; //socket 句柄
//	int realSocketId; //真实 socket 句柄（代理有效）
//	int isProxy; //代理标志
//	int realConnected; //真实连接上标志
//
//	int socStat;
//	int readStat;
//	int writeStat;
//}T_DSM_SOC_STAT;
//
//typedef struct
//{
//	void *callBack;
//} mr_socket_struct;
//
//typedef struct
//{
//   int32 pos;  //单位,秒s.
//}T_SET_PLAY_POS;
//
//typedef struct
//{
//	int32 pos;
//} T_MEDIA_TIME;
//
///*回调有两种可能的返回值：
//ACI_PLAY_COMPLETE   0  //播放结束
//ACI_PLAY_ERROR       1  //播放时遇到错误
//Loop ：1，循环播放；0，不循环播放；2，PCM循环播放模式
//Block：1，阻塞播放；0，不阻塞播放*/
//typedef void (*ACI_PLAY_CB)(int32 result);
//
//typedef struct
//{
//	ACI_PLAY_CB cb; //回调函数
//	int32 loop;
//	int32 block;
//}T_DSM_MEDIA_PLAY;
//
//typedef struct
//{
//	char *src;
//	int32 len;
//	int32 src_type;// MRAPP_SRC_TYPE
//} MRAPP_IMAGE_ORIGIN_T;
//
//typedef struct
//{
//   int32 width;    //图片的宽度
//   int32 height;   //图片的高度
//}MRAPP_IMAGE_SIZE_T;
//
//typedef struct
//{
//	char *src;
//	int32 src_len;
//	int32 src_type;
//	int32 ox;
//	int32 oy;
//	int32 w;
//	int32 h;
//} T_DRAW_DIRECT_REQ;
//
//typedef struct
//{
//	uint8 level;
//	uint8 current_band;
//	uint8 rat;
//	uint8 flag;
//} T_RX;

#endif /* MR_PLAT_H_ */
