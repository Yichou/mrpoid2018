/*
 * 远程短信配置
 * 创建：eleqian 2012-9-30
 */

#include <string.h>
#include "mrporting.h"
#include "mr_smscfg.h"
#include "mythroad.h"


char dsm_cfg_data_file_name[] = DSM_CFG_FILE_NAME;
uint8 mr_sms_cfg_buf[SMS_CFG_SIZE];
uint8 mr_sms_return_flag;
int32 mr_sms_return_val;
int32 mr_sms_cfg_need_save;

/*-----------------------------------*/
// 编码&解码

static uint8 _mr_encode_table(uint8 ch)
{
    if (0x7 == ch)
        return 0x44;
    else if (0xe == ch)
        return 0x68;
    else if (0x3b == ch)
        return 0x2f;
    else if (ch >= 0xb && ch <= 0x24)
        return ch + 0x36;
    else if (ch >= 0x2f && ch <= 0x3d)
        return ch + 0x3d;
    else if (ch <= 0xa)
        return ch + 0x61;
    else if (ch >= 0x25 && ch <= 0x2e)
        return ch + 0xb;
    else if (0x3e == ch)
        return 0x2b;
    else if (0x3f == ch)
        return 0x78;

    return 0xff;
}

static int32 _mr_encode(const uint8 *srcData, uint32 srcLen, uint8 *dstBuf)
{
    uint32 srcOffset;
    uint32 dstLen;
    uint32 i;

    srcOffset = 0;
    dstLen = 0;

    for ( i = 0; i < srcLen / 3; ++i )
    {
        dstBuf[dstLen] = _mr_encode_table(srcData[srcOffset] >> 2);
        dstBuf[dstLen + 1] = _mr_encode_table((srcData[srcOffset + 1] >> 4) | ((srcData[srcOffset] & 0x3) << 4));
        dstBuf[dstLen + 2] = _mr_encode_table((srcData[srcOffset + 2] >> 6) | ((srcData[srcOffset + 1] & 0xF) << 2));
        dstBuf[dstLen + 3] = _mr_encode_table(srcData[srcOffset + 2] & 0x3F);

        if ((dstBuf[dstLen] | dstBuf[dstLen + 1] | dstBuf[dstLen + 2] | dstBuf[dstLen + 3]) == 255)
            return -1;

        dstLen += 4;
        srcOffset += 3;
    }

    if (0 != srcLen % 3)
    {
        uint8 buf[3] = {0};

        for (i = 0; i < srcLen % 3; i++)
            buf[i] = srcData[srcOffset + i];

        dstBuf[dstLen] = _mr_encode_table(buf[0] >> 2);
        dstBuf[dstLen + 1] = _mr_encode_table((buf[1] >> 4) | 16 * (buf[0] & 3));
        dstBuf[dstLen + 2] = _mr_encode_table((buf[2] >> 6) | 4 * (buf[1] & 0xF));
        dstBuf[dstLen + 3] = _mr_encode_table(buf[2] & 0x3F);

        if ((dstBuf[dstLen] | dstBuf[dstLen + 1] | dstBuf[dstLen + 2] | dstBuf[dstLen + 3]) != 255)
        {
            dstLen += 4;
            for (i = 0; i < 3 - srcLen % 3; i++)
                dstBuf[dstLen - i - 1] = 0x3d;
        }
    }

    dstBuf[dstLen] = 0;

    return dstLen;
}

static uint8 _mr_decode_table(uint8 ch)
{
    if (0x44 == ch)
        return 0x7;
    else if (0x68 == ch)
        return 0xe;
    else if (0x78 == ch)
        return 0x3f;
    else if (ch >= 0x41 && ch <= 0x5a)
        return ch - 0x36;
    else if (ch >= 0x61 && ch <= 0x6b)
        return ch - 0x61;
    else if (ch >= 0x6c && ch <= 0x7a)
        return ch - 0x3d;
    else if (ch >= 0x30 && ch <= 0x39)
        return ch - 0xb;
    else if (0x2b == ch)
        return 0x3e;
    else if (0x2f == ch)
        return 0x3b;
    else if (0x3d == ch)
        return 0x40;

    return 0xff;
}

static int32 _mr_decode(uint8 *encode, uint32 srcLen, uint8 *dstBuf)
{
    uint8 buf[4];
    uint8 buf1[3] = {0};
    uint32 srcOffset;
    uint32 dstLen;
    uint32 i;
    uint32 j;

    if (0 == srcLen)
        return 0;

    srcOffset = 0;
    dstLen = 0;

    for (i = 0; i < (srcLen - 4) >> 2; i++)
    {
        for (j = 0; j < 4; j++)
        {
            buf[j] = _mr_decode_table(encode[srcOffset + j]);
            if (buf[j] == 0xff)
                return -1;
        }

        dstBuf[dstLen] = ((buf[1] & 0x30) >> 4) | (buf[0] << 2);
        dstBuf[dstLen + 1] = ((buf[2] & 0x3C) >> 2) | ((buf[1] & 0xF) << 4);
        dstBuf[dstLen + 2] = (buf[3] & 0x3F) | ((buf[2] & 3) << 6);

        dstLen += 3;
        srcOffset += 4;
    }

    for (i = 0; i < 4; i++)
    {
        buf[i] = _mr_decode_table(encode[srcOffset + i]);
        if (buf[i] == 0xff)
            return -1;
    }

    if (encode[srcLen - 2] == 0x3d)
        j = 2;
    else if (encode[srcLen - 1] == 0x3d)
        j = 1;
    else
        j = 0;

    buf1[0] = ((buf[1] & 0x30) >> 4) | (buf[0] << 2);
    buf1[1] = ((buf[2] & 0x3C) >> 2) | ((buf[1] & 0xF) << 4);
    buf1[2] = (buf[3] & 0x3F) | ((buf[2] & 3) << 6);

    for (i = 0; i < 3 - j; i++)
        dstBuf[dstLen + i] = buf1[i];

    dstLen += i;

    return dstLen;
}

/*-----------------------------------*/
// 数据读取&写入

int32 _mr_smsGetBytes(uint32 offset, void *buf, uint32 bufLen)
{
    if (offset >= 0 && offset < SMS_CFG_SIZE && bufLen + offset < SMS_CFG_SIZE)
    {
        memcpy(buf, mr_sms_cfg_buf + offset, bufLen);
        return 0;
    }

    return -1;
}

int32 _mr_smsSetBytes(uint32 offset, const void *data, uint32 dataLen)
{
    if (offset >= 0 && offset < SMS_CFG_SIZE && dataLen + offset < SMS_CFG_SIZE)
    {
        mr_sms_cfg_need_save = 1;
        memcpy(mr_sms_cfg_buf + offset, data, dataLen);
        return 0;
    }

    return -1;
}

int32 _mr_smsGetNum(int32 index, char *buf)
{
    char dst[32];
    int32 len;
    int32 ret;

    _mr_smsGetBytes(SMS_CFG_BLOCK_SIZE + 32 * index, dst, 32);

    len = strlen(dst);
    if (len >= 32)
        len = 31;

    ret = _mr_decode(dst, len, buf);
    if (ret > 0 && ret < 32)
    {
        buf[ret] = '\0';
        return 0;
    }
    else
    {
        buf[0] = '\0';
        return -1;
    }
}

int32 _mr_smsAddNum(int32 index, const char *str)
{
    char dst[32] = {0};
    int32 len;

    len = strlen(str);
    if (len <= 21)
    {
        _mr_encode(str, len, dst);
        _mr_smsSetBytes(SMS_CFG_BLOCK_SIZE + 32 * index, dst, 32);
        return 0;
    }
    else
    {
        mr_printf("num too long");
        return -1;
    }
}

int32 _mr_smsDelNum(int32 index)
{
    char dst[32] = {0};

    _mr_smsSetBytes(SMS_CFG_BLOCK_SIZE + 32 * index, dst, 32);

    return 0;
}

/*-----------------------------------*/
// 功能函数

#ifndef NO_SMS_CFG
static int32 _mr_smsReplyServer(char *number, const char *old_IMSI)
{
    mr_userinfo info;
    uint32 len;
    char sms[160] = {0};
    uint8 buf[120] = {0};

    if (0 != mr_getUserInfo(&info))
        return -1;

    len = 0;
    buf[0] = 0xfa;
    buf[1] = 0xf1;
    len += 2;

    if (NULL != old_IMSI)
    {
        buf[len++] = 0x11;
        buf[len++] = 0x6;
        memcpy(buf + len, old_IMSI, 16);
        len += 16;
    }

    buf[len++] = 0x11;
    buf[len++] = 0x2;
    memcpy(buf + len, info.IMEI, 16);
    len += 16;

    buf[len++] = 0x11;
    buf[len++] = 0x3;
    memcpy(buf + len, info.IMSI, 16);
    len += 16;

    buf[len++] = 0x15;
    buf[len++] = 0x4;
    memcpy(buf + len, info.manufactory, 16);
    len += 16;

    buf[len++] = ((info.ver >> 24) & 0xff);
    buf[len++] = ((info.ver >> 16) & 0xff);
    buf[len++] = ((info.ver >> 8) & 0xff);
    buf[len++] = ((info.ver >> 0) & 0xff);

    _mr_encode(buf, len, sms);
    mr_sendSms(number, sms, 0x18);

    return 0;
}

static int32 _mr_smsUpdateURL(char *url, uint32 len)
{
    char Dst[SMS_CFG_BLOCK_SIZE] = {0};
    char flag;
    char ret;

    flag = 0x80;

    if (len <= 87)
    {
        _mr_smsSetBytes(6, &flag, 1);
        ret = _mr_encode(url, len, Dst);
        _mr_smsSetBytes(SMS_CFG_BLOCK_SIZE * 3, Dst, SMS_CFG_BLOCK_SIZE);
        return 0;
    }
    else
    {
        mr_printf("url too long");
        return -1;
    }
}

static int32 _mr_smsDsmSave(const void *src, uint32 srcLen)
{
    char Dst[SMS_CFG_BLOCK_SIZE] = {0};
    uint8 block;
    char flag;

    flag = 0x80;
    mr_printf("_mr_smsDsmSave");
    memcpy(Dst, src, srcLen);

    block = (uint8)Dst[2];
    if (block <= 31)
    {
        _mr_smsSetBytes(4, &flag, 1);
        _mr_smsSetBytes(32 + block, &flag, 1);
        _mr_smsSetBytes(SMS_CFG_BLOCK_SIZE * (block + 4), Dst, SMS_CFG_BLOCK_SIZE);
        return 0;
    }

    return -1;
}

static void *_mr_memfind_c(uint8 *data, int32 dlen, uint8 *find, int32 flen)
{
    uint8 *p;
    int32 leftlen;

    if (flen > 0 && flen <= dlen)
    {
        leftlen = dlen - flen + 1;
        while ( leftlen )
        {
            p = memchr(data, *(uint8 *)find, leftlen);
            if ( !p )
                break;
            p = (uint8 *)p + 1;
            if ( !memcmp(p, (uint8 *)find + 1, flen - 1) )
                break;
            leftlen -= p - data;
            data = p;
        }
    }

    return p;
}

static int32 _mr_getChunk(uint8 **src, uint32 *srcLen, uint8 **dst, uint32 *dstLen)
{
    int32 ret;

    if (*srcLen > 1)
    {
        *dstLen = **src;
        if (*dstLen < *srcLen && *dstLen > 0)
        {
            *dst = *src + 1;
            *src += *dstLen + 1;
            *srcLen = *srcLen - *dstLen - 1;
            ret = 0;
        }
        else
        {
            *dst = *src;
            *dstLen = *srcLen;
            ret = -1;
        }
    }
    else
    {
        *dst = *src;
        *dstLen = *srcLen;
        ret = 1;
    }

    return ret;
}

static uint32 _mr_u2c_c(uint8 *uniStr, uint32 uniLen, uint8 *gbBuf, uint32 gbLen)
{
    uint32 i;
    uint32 j;

    j = 0;
    i = 0;
    memset(gbBuf, 0, gbLen);

    while (i < uniLen - 1 && uniStr[i + 1] + uniStr[i] && j < gbLen && !uniStr[i])
    {
        gbBuf[j++] = uniStr[i + 1];
        i += 2;
    }

    return j;
}
#endif  // !NO_SMS_CFG

/*-----------------------------------*/
// 外部接口

int32 _mr_load_sms_cfg(void)
{
    int32 size;
    int32 fd;

    LOGI("mr_load_sms_cfg");

    mr_sms_cfg_need_save = 0;
    memset(mr_sms_cfg_buf, 0, SMS_CFG_SIZE);
    if (mr_info(dsm_cfg_data_file_name) == MR_IS_FILE)
    {
        fd = mr_open(dsm_cfg_data_file_name, MR_FILE_RDONLY);
        if (0 == fd)
            return MR_FAILED;
        size = mr_read(fd, mr_sms_cfg_buf, SMS_CFG_SIZE);
        mr_close(fd);

        if (size == SMS_CFG_SIZE)
            return MR_SUCCESS;

        fd = mr_open(dsm_cfg_data_file_name, MR_FILE_CREATE | MR_FILE_WRONLY);
        if (0 == fd)
            return MR_FAILED;
        mr_close(fd);
    }

    _mr_smsAddNum(0, "518869058");
    _mr_smsAddNum(1, "918869058");
    _mr_smsAddNum(3, "aa");

    return MR_SUCCESS;
}

int32 _mr_save_sms_cfg(int32 f)
{
    int32 ret = MR_SUCCESS;
    int32 fd;

    LOGI("mr_save_sms_cfg(%d)", f);

    if (mr_sms_cfg_need_save)
    {
        mr_sms_cfg_need_save = 0;
        fd = mr_open(dsm_cfg_data_file_name, MR_FILE_CREATE | MR_FILE_WRONLY);
        if (0 != fd)
        {
            ret = mr_seek(fd, 0, MR_SEEK_SET);
            if (ret == MR_SUCCESS)
                ret = mr_write(fd, mr_sms_cfg_buf, SMS_CFG_SIZE);
            mr_close(fd);
        }
        else
        {
            ret = MR_FAILED;
        }
    }

    return ret;
}

int32 _mr_smsIndiaction(uint8 *pContent, int32 nLen, uint8 *pNum, int32 type)
{
#ifndef NO_SMS_CFG 
    char dstBuf[160] = {0};
    int32 dstLen;
    int32 fd_sms;

    // 下面代码移动到上级函数，避免引入外部变量
    //if (mr_state == 1 || (mr_timer_run_without_pause && mr_state == 2))
    //{
    //    c_event_st.code = 7;
    //    c_event_st.param0 = (int32)pContent;
    //    c_event_st.param1 = (int32)pNum;
    //    c_event_st.param2 = (int32)type;
    //    c_event_st.param3 = (int32)nLen;
    //    _mr_TestComC(801, &c_event_st, 20, 1);
    //}

    if (nLen < 12 || nLen > 160 )
        return MR_IGNORE;

    if (0 != type)
    {
        uint8 *p1, *p2;
        uint8 gbBuf[70];
        uint32 gbLen;

        if (type != 1)
            return MR_IGNORE;

        p1 = _mr_memfind_c(pContent, nLen, "\0~\0#\0^\0~", 8);
        if (NULL == p1)
            return MR_IGNORE;

        p2 = _mr_memfind_c(p1, nLen - (p1 - pContent), "\0&\0^", 4);
        if (NULL == p2)
            return MR_IGNORE;

        gbLen = _mr_u2c_c(p1 + 8, p2 - p1 - 8, gbBuf, 70);
        dstLen = _mr_decode(gbBuf, gbLen, dstBuf);
    }
    else
    {
        if (pContent[0] != 'M' || pContent[1] != 'R' || pContent[2] != 'P' || pContent[3] != 'G' )
        {
            uint8 *p1, *p2;

            p1 = _mr_memfind_c(pContent, nLen, "~#^~", 4);
            if (NULL == p1)
                return MR_IGNORE;

            p2 = _mr_memfind_c(p1, nLen - (p1 - pContent), "&^", 2);
            if (NULL == p2)
                return MR_IGNORE;

            dstLen = _mr_decode(p1 + 4, p2 - p1 - 4, dstBuf);
        }
        else
        {
            dstLen = _mr_decode(pContent + 4, nLen - 4, dstBuf);
        }
    }

    if (dstLen <= 0)
        return MR_IGNORE;

    mr_printf("mr_smsIndiaction check ok!");
    fd_sms = _mr_load_sms_cfg();

    if (dstBuf[0] == 0xfc && dstBuf[1] == 0xfc)
    {
        uint32 chuckLen;
        uint8 *chuck;
        uint32 srcLen;
        uint8 *src;

        src = &dstBuf[2];
        srcLen = dstLen - 2;

        while (0 == _mr_getChunk(&src, &srcLen, &chuck, &chuckLen))
        {
            int32 ret = -1;

            switch (*chuck)
            {
            case 1:
                ret = _mr_smsAddNum(*(chuck + 1), chuck + 2);
                break;
            case 2:
                ret = _mr_smsDelNum(*(chuck + 1));
                break;
            case 3:
                ret = _mr_smsUpdateURL(chuck + 1, chuckLen - 1);
                break;
            case 4:
                ret = _mr_smsReplyServer(pNum, NULL);
                break;
            case 5:
                ret = _mr_smsSetBytes(5, chuck + 1, 1);
                break;
            case 6:
                ret = _mr_smsSetBytes((*(chuck + 1) << 8) + *(chuck + 2), chuck + 4, *(chuck + 3));
                break;
            case 7:
            case 17:
            case 27:
            case 37:
                ret = 0;
                break;
            default:
                break;
            }

            if (0 != ret)
                goto err;
        }
    }
    else if (dstBuf[0] == 0xfa && dstBuf[1] == 0xf2)
    {
        _mr_smsDsmSave(dstBuf, dstLen);
    }

    _mr_save_sms_cfg(fd_sms);
    return MR_SUCCESS;

err:
    _mr_save_sms_cfg(fd_sms);
    return MR_FAILED;
#else  // NO_SMS_CFG
    return MR_IGNORE;
#endif  // NO_SMS_CFG
}

int32 _mr_newSIMInd(int16 type, const void *old_IMSI)
{
#ifndef NO_SMS_CFG
    int fd_sms;
    int netId;
    uint8 dst;
    char num[32];

    LOGI("_mr_newSIMInd(%d, %#p)", type, old_IMSI);

    netId = mr_getNetworkID();
    if (!type || type == 1)
    {
        fd_sms = _mr_load_sms_cfg();
        _mr_save_sms_cfg(fd_sms);

        _mr_smsGetBytes(5, &dst, 1);
        if (dst >= 128)
        {
            _mr_smsGetNum(3, num);
            return _mr_smsReplyServer(num, old_IMSI);
        }

        if (netId)
        {
            if (netId > 0 && netId <= 2 && _mr_smsGetNum(1, num) != -1)
                return _mr_smsReplyServer(num, old_IMSI);
        }
        else
        {
            if (_mr_smsGetNum(0, num) != -1)
                return _mr_smsReplyServer(num, old_IMSI);
        }
    }
    else
    {
        return 0;
    }

    return -1;
#else  // NO_SMS_CFG
    return MR_IGNORE;
#endif  // NO_SMS_CFG
}
