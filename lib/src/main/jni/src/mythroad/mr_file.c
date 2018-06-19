/*-----------------------------------*/
// mrp文件读取

#include <string.h>
#include "mr_helper.h"
#include "mrporting.h"
#include "mythroad.h"
#include "mr_utils.h"

uint8	*mr_gzInBuf;
uint8   *mr_gzOutBuf;
uint8   *LG_gzinptr;
uint8   *LG_gzoutcnt;

#define _mr_readFileShowInfo(f,n) \
	do \
	{ \
		mr_printf("_mr_readFile:%s err, code=%d", f, n); \
		LOGE("_mr_readFile:%s err, code=%d, packge=%s", f, n, pack_filename); \
	} while (0)

typedef struct {
    char magic[4];
    uint32 info_size;
    uint32 mrp_file_size;
    uint32 list_offset;
} mrp_head_st;

void *_mr_readFile(const char *filename, int32 *filelen, int32 lookfor)
{
    char in_file_name[128];
    uint32 in_file_name_len;
    int32 in_file_size;
    int32 in_file_offset;
    uint8 *in_file_data;

    int32 ret;
    int32 offset;
    int32 findok;
    int32 notfree;

    int32 gz_out_size;
    int32 gz_method;

#ifdef LOG_FILE
    LOGI("_mr_readFile(pack=%s, file=%s, look=%d)", pack_filename, filename, lookfor);
#endif

    findok = 0;
    notfree = 0;

    if (pack_filename[0] != '*' && pack_filename[0] != '$')
    {
        int32 fd;
        mrp_head_st head;
        uint8 *list_data;
        uint32 list_size;

        fd = mr_open(pack_filename, MR_FILE_RDONLY);
        if (fd == 0)
        {
            _mr_readFileShowInfo(filename, 2002);
            return NULL;
        }

        memset(&head, 0, 16);
        ret = mr_read(fd, &head, 16);
        if (ret != 16 || memcmp(head.magic, "MRPG", 4))
        {
            mr_close(fd);
            _mr_readFileShowInfo(filename, 3001);
            return NULL;
        }

        if ( head.info_size <= 232 )
        {
            mr_close(fd);
            _mr_readFileShowInfo(filename, 3051);
            return NULL;
        }

        list_size = head.info_size + 8 - head.list_offset;
        list_data = (uint8 *)mr_malloc(list_size);
        offset = 0;
        if (NULL == list_data)
        {
            mr_close(fd);
            _mr_readFileShowInfo(filename, 3003);
            return NULL;
        }

        ret = mr_seek(fd, head.list_offset - 16, MR_SEEK_CUR);
        if (ret < 0)
        {
            mr_close(fd);
            mr_free(list_data, list_size);
            _mr_readFileShowInfo(filename, 3002);
            return NULL;
        }

        ret = mr_read(fd, list_data, list_size);
        if (ret != list_size)
        {
            mr_close(fd);
            mr_free(list_data, list_size);
            _mr_readFileShowInfo(filename, 3003);
            return NULL;
        }

        while (0 == findok)
        {
            memcpy(&in_file_name_len, list_data + offset, 4);
            offset += 4;
            if (offset + in_file_name_len > list_size || in_file_name_len < 1 || in_file_name_len >= 0x80)
            {
                mr_close(fd);
                mr_free(list_data, list_size);
                _mr_readFileShowInfo(filename, 3004);
                return NULL;
            }

            memset(in_file_name, 0, 0x80);
            memcpy(in_file_name, list_data + offset, in_file_name_len);
            offset += in_file_name_len;
            if (strcmp(filename, in_file_name))
            {
                offset += 12;
                if (offset >= list_size)
                {
                    mr_close(fd);
                    mr_free(list_data, list_size);
                    _mr_readFileShowInfo(filename, 3006);
                    return NULL;
                }
            }
            else
            {
                if (lookfor == 1)
                {
                    mr_close(fd);
                    mr_free(list_data, list_size);
                    return (void*)1;
                }

                findok = 1;
                memcpy(&in_file_offset, &list_data[offset], 4);
                offset += 4;
                memcpy(&in_file_size, &list_data[offset], 4);
                offset += 4;
                if (in_file_size + in_file_offset > head.mrp_file_size)
                {
                    mr_close(fd);
                    mr_free(list_data, list_size);
                    _mr_readFileShowInfo(filename, 3005);
                    return NULL;
                }
            }
        }

        mr_free(list_data, list_size);
        *filelen = in_file_size;
        in_file_data = (uint8 *)mr_malloc(in_file_size);
        if (NULL == in_file_data)
        {
            mr_close(fd);
            _mr_readFileShowInfo(filename, 3007);
            return NULL;
        }

        ret = mr_seek(fd, in_file_offset, MR_SEEK_SET);
        if (ret < 0)
        {
            mr_free(in_file_data, in_file_size);
            mr_close(fd);
            _mr_readFileShowInfo(filename, 3008);
            return NULL;
        }

        for (offset = 0; offset < in_file_size; offset += ret)
        {
            ret = mr_read(fd, in_file_data + offset, in_file_size - offset);
            if (ret <= 0)
            {
                mr_free(in_file_data, in_file_size);
                mr_close(fd);
                _mr_readFileShowInfo(filename, 3009);
                return NULL;
            }
        }
        mr_close(fd);
    }
    else
    {
        uint8 *ram_data;
        uint32 ram_data_len;
        uint32 data_len;

        offset = 0;
        if (pack_filename[0] == '*')
            ram_data = mr_m0_files[pack_filename[1] - 'A'];
        else
            ram_data = mr_ram_file;
        if (NULL == ram_data)
        {
            _mr_readFileShowInfo(filename, 1001);
            return NULL;
        }

        offset += 4;
        memcpy(&data_len, ram_data + offset, 4);
        offset += 4;
        if (pack_filename[0] == '$')
        {
            ram_data_len = mr_ram_file_len;

            // 不用校验权限了
            /*if ((bi & 2) && _mr_isMr(ram_data + 52))
            {
                _mr_readFileShowInfo("unauthorized", 3);
                return NULL;
            }*/
        }
        else
        {
            memcpy(&ram_data_len, ram_data + offset, 4);
        }

        offset += data_len;
        while (0 == findok)
        {
            if (offset + 4 >= ram_data_len || data_len < 1 || data_len >= 1024000)
            {
                _mr_readFileShowInfo(filename, 1004);
                return NULL;
            }

            memcpy(&data_len, ram_data + offset, 4);
            offset += 4;
            if (offset + data_len >= ram_data_len || data_len < 1 || data_len >= 128)
            {
                _mr_readFileShowInfo(filename, 1002);
                return NULL;
            }

            memset(in_file_name, 0, 128);
            memcpy(in_file_name, ram_data + offset, data_len);
            offset += data_len;
            if (strcmp(filename, in_file_name))
            {
                memcpy(&data_len, ram_data + offset, 4);
                offset += data_len + 4;
            }
            else
            {
                if (lookfor == 1)
                    return (void*)1;
                findok = 1;
                memcpy(&data_len, ram_data + offset, 4);
                offset += 4;
                if (offset + data_len > ram_data_len || data_len < 1 || data_len >= 1024000)
                {
                    _mr_readFileShowInfo(filename, 1003);
                    return NULL;
                }
            }
        }

        in_file_size = data_len;
        *filelen = in_file_size;
        if (*filelen <= 0)
        {
            _mr_readFileShowInfo(filename, 1005);
            return NULL;
        }

        in_file_data = ram_data + offset;
        if (lookfor == 2)
            return in_file_data;
        notfree = 1;

        //这里其实没实现（MTK 内置MRP）
        //return in_file_data;
    }

    mr_gzInBuf = in_file_data;
    mr_gzOutBuf = NULL;
    LG_gzoutcnt = NULL;
    LG_gzinptr = NULL;

	//检查是不是 gz 文件
	//LOGI("head[%x][%x][%x][%x]", in_file_data[0],in_file_data[1],in_file_data[2],in_file_data[3]);
	if(in_file_data[0] != 0x1f || in_file_data[1] != 0x8b){ //非 gz 压缩文件
		gz_method = -1;
		//LOGI("_mr_readFile: \"%s\", not zip!");
		*filelen = in_file_size;
		mr_gzOutBuf = in_file_data;
	}else
		gz_method = 0; //mr_get_method(in_file_size);

    if (gz_method >= 0)
    {
        //gz_out_size = (uint8)in_file_data[in_file_size - 4];
        //gz_out_size |= (uint8)in_file_data[in_file_size - 3] << 8;
        //gz_out_size |= (uint8)in_file_data[in_file_size - 2] << 16;
        //gz_out_size |= (uint8)in_file_data[in_file_size - 1] << 24;
        //*filelen = gz_out_size;

        if (MR_SUCCESS != mr_unzip(in_file_data, in_file_size, &mr_gzOutBuf, &gz_out_size))
        {
			//LOGE("_mr_readFile: \"%s\" Unzip err!, in_file_data=%#p,in_file_size=%d,packge=%s",
			//	filename, in_file_data, in_file_size, pack_filename);
			//LOGE(" outbuf=%#p, outsize=%d", mr_gzOutBuf, gz_out_size);
//            mr_printf("_mr_readFile: \"%s\" Unzip err!", filename);
        }

        *filelen = gz_out_size;

        if (!notfree)
            mr_free(in_file_data, in_file_size);
    }

    return mr_gzOutBuf;
}

int32 mrc_GetMrpInfoOpen(const char *filename, int32 *memfile)
{
    int32 fd = 0;

    if (NULL != memfile && NULL != filename)
    {
        if (*filename == '*')
        {
            *memfile = 1;
            fd = (int32)mr_m0_files[pack_filename[1] - 'A'];
        }
        else if (*filename == '$')
        {
            *memfile = 1;
            fd = (int32)mr_ram_file;
        }
        else
        {
            *memfile = 0;
            fd = mr_open(filename, MR_FILE_RDONLY);
        }
    }

#ifdef LOG_FILE
	LOGI("mrc_GetMrpInfoOpen(%s) mem=%d", filename, *memfile);
#endif

    return fd;
}

int32 mrc_GetMrpInfoClose(int32 memfile, int32 fd)
{
    if (0 != fd && 0 == memfile)
        return mr_close(fd);
    return 0;
}

int32 mrc_GetMrpInfoRead(int32 memfile, int32 fd, int32 offset, void *Dst, uint32 Size)
{
    int32 ret = -1;

    if (NULL != Dst)
    {
        if (memfile)
        {
            memcpy(Dst, (uint8 *)fd + offset, Size);
            ret = Size;
        }
        else
        {
            ret = mr_seek(fd, offset, MR_SEEK_SET);
            if (0 == ret)
                ret = mr_read(fd, Dst, Size);
        }
    }

    return ret;
}

int32 mrc_GetMrpInfoEx(int32 memfile, int32 fd, int32 type, void *Dst, uint32 Size)
{
    uint32 ulNum;
    int32 ret;

    ret = -1;
    memset(Dst, 0, Size);

    switch (type)
    {
    case 1:
        if (Size >= 12)
        {
            char *p;

            ret = mrc_GetMrpInfoRead(memfile, fd, type, Dst, 12);
            p = strchr(Dst, '.');
            if (NULL != p)
                memset(p, 0, (char*)Dst + 12 - p);
            else
                ret = -1;
        }
        break;

    case 2:
        if (Size >= 24)
            ret = mrc_GetMrpInfoRead(memfile, fd, 28, Dst, 24);
        break;

    case  3:
        if (Size >= 4)
        {
            ret = mrc_GetMrpInfoRead(memfile, fd, 192, Dst, 4);
            ulNum = *(uint32 *)Dst;
            ulNum = mrc_ntohl(ulNum);
            memcpy(Dst, &ulNum, 4);
        }
        break;

    case 4:
        if (Size >= 4)
        {
            ret = mrc_GetMrpInfoRead(memfile, fd, 196, Dst, 4);
            ulNum = *(uint32 *)Dst;
            ulNum = mrc_ntohl(ulNum);
            memcpy(Dst, &ulNum, 4);
        }
        break;

    case 5:
        if (Size >= 40)
            ret = mrc_GetMrpInfoRead(memfile, fd, 88, Dst, 40);
        break;

    case 6:
        if (Size >= 64)
            ret = mrc_GetMrpInfoRead(memfile, fd, 128, Dst, 64);
        break;

    case 7:
        if (Size >= 2)
        {
            uint16 usNum;

            ret = mrc_GetMrpInfoRead(memfile, fd, 200, Dst, 2);
            usNum = *((uint8 *)Dst + 1) + (*(uint8 *)Dst << 8);
            memcpy(Dst, &usNum, 2);
        }
        break;

    case 8:
        if (Size >= 2)
        {
            *(uint8 *)Dst = 0;
            *((uint8 *)Dst + 1) = 0;
            ret = 2;
            mrc_GetMrpInfoRead(memfile, fd, 203, Dst, 1);
            mrc_GetMrpInfoRead(memfile, fd, 209, Dst + 1, 1);
        }
        break;

    case 9:
        ret = mrc_GetMrpInfoRead(memfile, fd, 200, Dst, Size);
        break;

    default:
        break;
    }

    return ret;
}
