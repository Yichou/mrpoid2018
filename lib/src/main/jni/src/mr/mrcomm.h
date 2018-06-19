
#ifndef MRCOMM_H
#define MRCOMM_H


#define MR_SCREEN_W mr_screen_w
#define MR_SCREEN_MAX_W MR_SCREEN_W
#define MR_SCREEN_H mr_screen_h
#ifdef MR_ANYKA_MOD
#define MR_SCREEN_DEEP 3
#else
#define MR_SCREEN_DEEP 2
#endif

#define BITMAPMAX  30
#define SPRITEMAX  10
#define TILEMAX    3

#define SOUNDMAX   5

#define MR_SPRITE_INDEX_MASK       (0x03FF) // mask of bits used for tile index
#define MR_SPRITE_TRANSPARENT         (0x0400)

#define MR_TILE_SHIFT         (11)

#define MR_ROTATE_0         (0)
#define MR_ROTATE_90         (1)
#define MR_ROTATE_180         (2)
#define MR_ROTATE_270         (3)


#endif
