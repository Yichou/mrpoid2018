#ifndef _MR_UTILS_H
#define _MR_UTILS_H

#include "mr_helper.h"

uint32 mr_updcrc(uint8 *s, uint32 n);
void mr_md5_init(md5_state_t *pms);
void mr_md5_append(md5_state_t *pms, const md5_byte_t *data, int nbytes);
void mr_md5_finish(md5_state_t *pms, md5_byte_t digest[16]);
int32 mr_unzip(uint8* inputbuf, int32 inputlen, uint8** outputbuf, int32* outputlen);
uint16 *mr_c2u(char *cp, int32 *err, int32 *size);
int32 _mr_isMr(void *buf);

#endif  // _MR_UTILS_H
