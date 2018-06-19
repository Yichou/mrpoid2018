#ifndef __SMSCFG_H__
#define __SMSCFG_H__

#include "mr_types.h"

// 打开宏时不自动进行短信远程配置
//#define NO_SMS_CFG

#define DSM_CFG_FILE_NAME "dsm.cfg"

#define SMS_CFG_BLOCK_SIZE  (120)
#define SMS_CFG_BLOCK_NUM   (36)
#define SMS_CFG_SIZE        (SMS_CFG_BLOCK_SIZE * SMS_CFG_BLOCK_NUM)

extern char  dsm_cfg_data_file_name[];
extern uint8 mr_sms_cfg_buf[SMS_CFG_SIZE];
extern uint8 mr_sms_return_flag;
extern int32 mr_sms_return_val;
extern int32 mr_sms_cfg_need_save;

int32 _mr_smsSetBytes(uint32 offset, const void *Src, uint32 Size);
int32 _mr_smsGetBytes(uint32 offset, void *Dst, uint32 Size);

int32 _mr_smsGetNum(int32 index, char *Buf);
int32 _mr_smsAddNum(int32 index, const char *Str);
int32 _mr_smsDelNum(int32 index);

int32 _mr_newSIMInd(int16 type, const void *old_IMSI);
int32 _mr_smsIndiaction(uint8 *pContent, int32 nLen, uint8 *pNum, int32 type);

int32 _mr_load_sms_cfg(void);
int32 _mr_save_sms_cfg(int32 f);

#endif // __SMSCFG_H__
