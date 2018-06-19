/*-----------------------------------*/
// 图像和字符绘制

#include <string.h>

#include "mr_helper.h"
#include "mrporting.h"
#include "mr_utils.h"
#include "mythroad.h"


#include "../font/TSFFont.h"


/* return 5/6/5 bit r, g or b component of 16 bit pixel*/
#define PIXEL565RED(pixel)        (((pixel) >> 11) & 0x1f)
#define PIXEL565GREEN(pixel)      (((pixel) >> 5) & 0x3f)
#define PIXEL565BLUE(pixel)       ((pixel) & 0x1f)



uint16 *mr_screenBuf;
int32 mr_screen_w;
int32 mr_screen_h;
int32 mr_screen_bit;

void _DrawPoint(int16 x, int16 y, uint16 nativecolor)
{
#ifdef LOG_DRAW
	LOGI("_DrawPoint(%d,%d,%#04x)", x, y, nativecolor);
#endif

	if(x<0 || y<0 || x>mr_screen_w-1 || y>mr_screen_h-1)
		return;

    *(mr_screenBuf + mr_screen_w * y + x) = nativecolor;
}

void DrawRect(int16 sx, int16 sy, int16 w, int16 h, uint8 cr, uint8 cg, uint8 cb)
{
	uint16 color;
	int16 x_d, y_d;
	int16 x_min, y_min;
	int16 x_max, y_max;
	uint16 *Src;
	uint16 *Dst;

#ifdef LOG_DRAW
	LOGI("DrawRect(%d,%d,%d,%d,%d,%d%,d)", sx, sy, w, h, cr, cg, cb);
#endif

	y_max = MIN(h + sy, mr_screen_h);
	x_max = MIN(w + sx, mr_screen_w);
	y_min = MAX(sy, 0);
	x_min = MAX(sx, 0);

	color = (cb >> 3) + ((cg >> 2) << 5) + ((cr >> 3) << 11);
	if (y_max > y_min && x_max > x_min)
	{
		Dst = Src = mr_screenBuf + mr_screen_w * y_min + x_min;
		for (x_d = x_min; x_d < x_max; x_d++)
		{
			*Dst++ = color;
		}

		if ((int)Src & 0x3)
		{
			Src++;
			for (y_d = y_min + 1; y_d < y_max; y_d++)
			{
				Dst = mr_screenBuf + mr_screen_w * y_d + x_min;
				*Dst++ = color;
				memcpy(Dst, Src, 2 * (x_max - x_min - 1));
			}
		}
		else
		{
			for (y_d = y_min + 1; y_d < y_max; y_d++)
			{
				Dst = mr_screenBuf + mr_screen_w * y_d + x_min;
				memcpy(Dst, Src, 2 * (x_max - x_min));
			}
		}
	}
}

void _DrawBitmap(uint16* p, int16 x, int16 y, uint16 w, uint16 h,
                 uint16 rop,  uint16 transcoler, int16 sx, int16 sy, int16 mw)
{
	uint16 *dest = NULL, *src = NULL;
	uint32 row = 0, col = 0;

#ifdef LOG_DRAW
	LOGI("_DrawBitmap:%#p,%d,%d,%d,%d,rop=%d,sx=%d,sy=%d,mw=%d",
			p, x, y, w, h, rop, sx, sy, mw);
#endif

	//各种校验
	if(!p || !w || !h || mw <= 0
			|| sx > mw-1 //超出图片宽度
			|| x + w <= 0 //超出屏幕左侧
			|| x > mr_screen_w-1
			|| y > mr_screen_h-1
			|| y + h <= 0 //超出屏幕顶部
			)
		return;

	if(sx < 0) sx = 0;
	if(sy < 0) sy = 0;

	if(sx+w > mw-1) w = mw-sx;
	if(w == 0) return;

	if (x < 0) {
		w = w + x;
		sx = sx - x;
		x = 0;
	}
	if (y < 0) {
		h = h + y;
		sy = sy - y;
		y = 0;
	}
	if ((x + w) > mr_screen_w-1) {
		w = mr_screen_w - x;
	}
	if ((y + h) > mr_screen_h-1) {
		h = mr_screen_h - y;
	}

	dest = mr_screenBuf + (y * mr_screen_w + x);
	src = p + (sy * mw + sx);

	switch (rop) {
	case BM_TRANSPARENT:
		for (row = 0; row < h; row++) {
			for (col = 0; col < w; col++) {
				if (transcoler != *src)
					*dest = *src;
				dest++;
				src++;
			}
			dest += mr_screen_w - w;
			src += mw - w;
		}
		break;

	case BM_OR:
		for (row = 0; row < h; row++) {
			for (col = 0; col < w; col++) {
				*dest++ |= *src++;
			}
			dest += mr_screen_w - w;
			src += mw - w;
		}
		break;

	case BM_XOR:
		for (row = 0; row < h; row++) {
			for (col = 0; col < w; col++) {
				*dest++ ^= *src++;
			}
			dest += mr_screen_w - w;
			src += mw - w;
		}
		break;

	case BM_NOT:
		for (row = 0; row < h; row++) {
			for (col = 0; col < w; col++) {
				*dest++ = ~(*src++);
			}
			dest += mr_screen_w - w;
			src += mw - w;
		}
		break;

	case BM_MERGENOT:
		for (row = 0; row < h; row++) {
			for (col = 0; col < w; col++) {
				*dest++ |= ~(*src++);
			}
			dest += mr_screen_w - w;
			src += mw - w;
		}
		break;

	case BM_ANDNOT:
		for (row = 0; row < h; row++) {
			for (col = 0; col < w; col++) {
				*dest++ &= (~*src++);
			}
			dest += mr_screen_w - w;
			src += mw - w;
		}
		break;

	case BM_AND:
		for (row = 0; row < h; row++) {
			for (col = 0; col < w; col++) {
				*dest++ &= *src++;
			}
			dest += mr_screen_w - w;
			src += mw - w;
		}
		break;

	case BM_GRAY:
		for (row = 0; row < h; row++) {
			for (col = 0; col < w; col++) {
				if (transcoler != *src){
					uint32 r = PIXEL565RED(*src);
					uint32 g = PIXEL565GREEN(*src);
					uint32 b = PIXEL565BLUE(*src);

					r = g = b = 0.299*r + 0.587*g + 0.114*b;
					*dest = MAKERGB(r, g, b);
				}
				dest++;
				src++;
			}
			dest += mr_screen_w - w;
			src += mw - w;
		}
		break;

	case BM_REVERSE:
		for (row = 0; row < h; row++) {
			for (col = 0; col < w; col++) {
				if (transcoler != *src)
					*dest = ~*src;
				dest++;
				src++;
			}
			dest += mr_screen_w - w;
			src += mw - w;
		}
		break;

	case BM_COPY:
	default:
	{
		/*uint32 w_bytes;

		w_bytes = w * 2;
		for (row = 0; row < h; row++) {
			memcpy(dest, src, w_bytes);
			dest += mr_screen_w - w;
			src += mw - w;
		}*/
		for (row = 0; row < h; row++) {
			for (col = 0; col < w; col++) {
				*dest++ = *src++;
			}
			dest += mr_screen_w - w;
			src += mw - w;
		}
		break;
	}
	}
}

void _DrawBitmapEx(mr_bitmapDrawSt *srcbmp, mr_bitmapDrawSt *dstbmp,
		uint16 w, uint16 h, mr_transMatrixSt *trans, uint16 transcoler) {
	int dy1, dy2;
	int dx1, dx2;
	int dy_max1, dy_max2;
	int dy_min1, dy_min2;

	uint16 *src;
	uint16 *dst;
	int src_tx, src_ty;
	int16 dst_cx, dst_cy;
	int16 dst_min_x, dst_min_y;
	int16 dst_max_x, dst_max_y;
	int16 d_x, d_y;
	int tA, tB, tC, tD;
	int transMod;
	int rop;

#ifdef LOG_DRAW
	LOGI("_DrawBitmapEx(...)");
#endif

	tA = trans->A;
	tB = trans->B;
	tC = trans->C;
	tD = trans->D;
	dst_cx = (w >> 1) + dstbmp->x;
	dst_cy = (h >> 1) + dstbmp->y;
	transMod = tD * tA - tC * tB;

	dst_max_y = (h * abs(tD) + w * abs(tC)) >> 9;
	if (dst_max_y >= dstbmp->h - dst_cy)
		dst_max_y = dstbmp->h - dst_cy;

	dst_min_y = -dst_max_y;
	if (dst_min_y <= -dst_cy)
		dst_min_y = -dst_cy;

	for (d_y = dst_min_y; d_y < dst_max_y; d_y++) {
		if (tD) {
			dy1 = (d_y * tB + (transMod * w >> 9)) / tD;
			dy2 = (d_y * tB - (transMod * w >> 9)) / tD;
			dy_max1 = MAX(dy1, dy2);
		} else {
			dy_max1 = 999;
		}

		if (tC) {
			dy1 = (d_y * tA + (transMod * h >> 9)) / tC;
			dy2 = (d_y * tA - (transMod * h >> 9)) / tC;
			dy_max2 = MAX(dy1, dy2);
		} else {
			dy_max2 = 999;
		}

		if (dy_max1 >= dy_max2) {
			if (tC) {
				dx1 = (d_y * tA + (transMod * h >> 9)) / tC;
				dx2 = (d_y * tA - (transMod * h >> 9)) / tC;
				dst_max_x = MAX(dx1, dx2);
			} else {
				dst_max_x = 999;
			}
		} else {
			if (tD) {
				dx1 = (d_y * tB + (transMod * w >> 9)) / tD;
				dx2 = (d_y * tB - (transMod * w >> 9)) / tD;
				dst_max_x = MAX(dx1, dx2);
			} else {
				dst_max_x = 999;
			}
		}

		if (tD) {
			dy1 = (d_y * tB - (transMod * w >> 9)) / tD;
			dy2 = (d_y * tB + (transMod * w >> 9)) / tD;
			dy_min1 = MIN(dy1, dy2);
		} else {
			dy_min1 = -999;
		}

		if (tC) {
			dy1 = (d_y * tA - (transMod * h >> 9)) / tC;
			dy2 = (d_y * tA + (transMod * h >> 9)) / tC;
			dy_min2 = MIN(dy1, dy2);
		} else {
			dy_min2 = -999;
		}

		if (dy_min1 <= dy_min2) {
			if (tC) {
				dx1 = (d_y * tA - (transMod * h >> 9)) / tC;
				dx2 = (d_y * tA + (transMod * h >> 9)) / tC;
				dst_min_x = MIN(dx1, dx2);
			} else {
				dst_min_x = -999;
			}
		} else {
			if (tD) {
				dx1 = (d_y * tB - (transMod * w >> 9)) / tD;
				dx2 = (d_y * tB + (transMod * w >> 9)) / tD;
				dst_min_x = MIN(dx1, dx2);
			} else {
				dst_min_x = -999;
			}
		}

		if (dst_max_x >= dstbmp->w - dst_cx)
			dst_max_x = dstbmp->w - dst_cx;
		if (dst_min_x <= -dst_cx)
			dst_min_x = -dst_cx;

		dst = dstbmp->p + dstbmp->w * (dst_cy + d_y) + dst_cx + dst_min_x;
		rop = trans->rop;
		if (rop == BM_COPY) {
			for (d_x = dst_min_x; d_x < dst_max_x; d_x++) {
				src_ty = (h >> 1) + ((d_y * tA - d_x * tC) << 8) / transMod;
				src_tx = (w >> 1) + ((d_x * tD - d_y * tB) << 8) / transMod;
				if (src_ty < h && src_ty >= 0 && src_tx < w && src_tx >= 0) {
					src = srcbmp->p + srcbmp->w * (srcbmp->y + src_ty)
							+ srcbmp->x + src_tx;
					*dst = *src;
				}
				dst++;
			}
		} else if (rop == BM_TRANSPARENT) {
			for (d_x = dst_min_x; d_x < dst_max_x; d_x++) {
				src_ty = (h >> 1) + ((d_y * tA - d_x * tC) << 8) / transMod;
				src_tx = (w >> 1) + ((d_x * tD - d_y * tB) << 8) / transMod;
				if (src_ty < h && src_ty >= 0 && src_tx < w && src_tx >= 0) {
					src = srcbmp->p + srcbmp->w * (srcbmp->y + src_ty)
							+ srcbmp->x + src_tx;
					if (*src != transcoler)
						*dst = *src;
				}
				dst++;
			}
		}
	}
}

int _BitmapCheck(uint16 *p, int16 x, int16 y, uint16 w, uint16 h,
                        uint16 transcoler, uint16 color_check)
{
	int count;
	int16 x_d, y_d;
	int16 x_min, y_min;
	int16 x_max, y_max;
	uint16 *p_pic;
	uint16 *p_scr;

#ifdef LOG_DRAW
	LOGI("_BitmapCheck(%#p,%d,%d,%d,%d)", p, x, y, w, h);
#endif

	y_max = MIN(h + y, mr_screen_h);
	x_max = MIN(w + x, mr_screen_w);
	y_min = MAX(y, 0);
	x_min = MAX(x, 0);

	count = 0;
	for (y_d = y_min; y_d < y_max; y_d++)
	{
		p_scr = mr_screenBuf + 2 * (mr_screen_w * y_d + x_min);
		p_pic = p + w * (y_d - y) + x_min - x;
		for (x_d = x_min; x_d < x_max; x_d++)
		{
			if (*p_pic != transcoler && *p_scr != color_check)
			{
				count++;
			}
			p_scr++;
			p_pic++;
		}
	}

	return count;
}

int32 _mr_EffSetCon(int16 x, int16 y, int16 w, int16 h, int16 perr, int16 perg, int16 perb)
{
	int16 x_d, y_d;
	int16 x_min, y_min;
	int16 x_max, y_max;
	uint16 pixel_new;
	uint16 pixel;
	uint16 *p;

#ifdef LOG_DRAW
	LOGI("_me_EffSetCon:x=%d,y=%d,w=%d,h=%d", x, y, w, h);
#endif

	y_max = MIN(h + y, mr_screen_h);
	x_max = MIN(w + x, mr_screen_w);
	y_min = MAX(y, 0);
	x_min = MAX(x, 0);

	for (y_d = y_min; y_d < y_max; y_d++)
	{
		p = mr_screenBuf + mr_screen_w * y_d + x_min;
		for (x_d = x_min; x_d < x_max; x_d++)
		{
			pixel = *p;
			pixel_new  = (perr * (pixel & 0xF800) >> 8) & 0xF800;
			pixel_new |= (perg * (pixel & 0x07E0) >> 8) & 0x07E0;
			pixel_new |= (perb * (pixel & 0x001F) >> 8) & 0x001F;
			*p = pixel_new;
			p++;
		}
	}

    return 0;
}


void mr_transbitmapDraw()
{
	LOGW("mr_transbitmapDraw not impl");
}
void mr_drawRegion()
{
	LOGW("mr_transbitmapDraw not impl");
}


//-----------------------------------------------------------
#define MAKERGB(r, g, b) \
	(uint16) ( ((uint32)(r>>3) << 11) + ((uint32)(g>>2) << 5) + ((uint32)(b>>3)) )

extern void mr_platDrawCharReal(uint16 ch, int32 x, int32 y, uint16 color);

void mr_platDrawChar(uint16 ch, int32 x, int32 y, int32 color)
{
#ifdef LOG_DRAW
	LOGI("mr_platDrawChar(%#x,%d,%d,%#04x)", ch, x, y, color);
#endif

	mr_platDrawCharReal(ch, x, y, color);
}

int32 _DrawText(char* pcText, int16 x, int16 y,
                uint8 r, uint8 g, uint8 b, int is_unicode, uint16 font)
{
	int32 len;
	uint8 *p;
	uint16 ch;
	uint16 color = MAKERGB(r, g, b);
	int fw, fh, sx;


#ifdef LOG_DRAW
    LOGI("_DrawText(%s,%d,%d,uni=%d,font=%d)", pcText, x, y, is_unicode, font);
#endif

    if (0 == is_unicode) {
        pcText = (char *)mr_c2u(pcText, NULL, &len);
    }

	sx = x;
	p = (uint8*)pcText;
	while(*p || *(p+1)){
		ch = (uint16) ((*p<<8) + *(p+1));

		mr_getCharBitmap(ch, font, &fw, &fh);
		mr_platDrawChar(ch, sx, y, color);

		sx += fw;
		p += 2;

		if(sx > mr_screen_w)
			break;
	}

	if(0 == is_unicode)
		mr_free(pcText, len);

    return MR_SUCCESS;
}

int32 _DrawTextEx(char* pcText, int16 x, int16 y, mr_screenRectSt rect, mr_colourSt colorst, int flag, uint16 font)
{
	int isUnicode, autoNewline;
	int32 len;
	uint8 *p;
	uint16 ch;
	uint16 color = MAKERGB(colorst.r, colorst.g, colorst.b);
	int fw, fh, sx, sy;
	int l, t, r, b;

#ifdef LOG_DRAW
    LOGI("_DrawTextEx(%s,%d,%d,flag=%d,font=%d)", pcText, x, y, flag, font);
#endif

	isUnicode = flag & DRAW_TEXT_EX_IS_UNICODE;
	autoNewline = flag & DRAW_TEXT_EX_IS_AUTO_NEWLINE;

	if(!isUnicode){
		pcText = (char *)mr_c2u(pcText, NULL, &len);
	}

	l = MAX(0, rect.x);
	t = MAX(0, rect.y);
	r = MIN(mr_screen_w-1, x+rect.w-1);
	b = MIN(mr_screen_h-1, y+rect.h-1);
	sx = x;
	sy = y;
	p = (uint8*)pcText;
	while(*p || *(p+1)){
		ch = (uint16) ((*p<<8) + *(p+1));

		mr_getCharBitmap(ch, font, &fw, &fh);
		
		if(sx<l || sx+fw>r || sy<t || sy>b)
			goto next;

		mr_platDrawChar(ch, sx, sy, color);

next:
		p += 2;
		if(autoNewline && (sx+fw > r)){
			sx = l;
			sy += fh;
			if(sy > b)
				break;
		}else {
			sx += fw;
			if(sx > r)
				break;
		}
	}

	if(0 == isUnicode)
		mr_free(pcText, len);

    return MR_SUCCESS;
}
