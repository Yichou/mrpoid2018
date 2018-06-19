#ifndef _MR_DRAW_H
#define _MR_DRAW_H

#include "mr_helper.h"

extern uint16 *mr_screenBuf;
extern int32 mr_screen_w;
extern int32 mr_screen_h;
extern int32 mr_screen_bit;

void _DrawPoint(int16 x, int16 y, uint16 nativecolor);
void DrawRect(int16 sx, int16 sy, int16 w, int16 h, uint8 cr, uint8 cg, uint8 cb);
void _DrawBitmap(uint16* p, int16 x, int16 y, uint16 w, uint16 h,
                 uint16 rop,  uint16 transcoler, int16 sx, int16 sy, int16 mw);
void _DrawBitmapEx(mr_bitmapDrawSt *srcbmp, mr_bitmapDrawSt *dstbmp,
		uint16 w, uint16 h, mr_transMatrixSt *trans, uint16 transcoler);
int _BitmapCheck(uint16*p, int16 x, int16 y, uint16 w, uint16 h,
                        uint16 transcoler, uint16 color_check);
int32 _mr_EffSetCon(int16 x, int16 y, int16 w, int16 h, int16 perr, int16 perg, int16 perb);
void mr_platDrawChar(uint16 ch, int32 x, int32 y, int32 color);
int32 _DrawText(char* pcText, int16 x, int16 y,
                uint8 r, uint8 g, uint8 b, int is_unicode, uint16 font);
int32 _DrawTextEx(char* pcText, int16 x, int16 y, mr_screenRectSt rect,
		mr_colourSt colorst, int flag, uint16 font);

void mr_transbitmapDraw();
void mr_drawRegion();

#endif // _MR_DRAW_H
