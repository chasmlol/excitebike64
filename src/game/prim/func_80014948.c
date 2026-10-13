#include "ctx.h"

typedef struct {
    u8 w;
    u8 h;
    u8 flags;
    u8 pad;
    u8 *data;
} Img;

typedef struct {
    u32 count;
    u32 pad;
    Img img[1];
} ImgSet;

static inline void flip(u8 *pa, s32 row, s32 h)
{
    s32 j, k;
    u8 *pb;
    u8 tmp;

    pb = pa + row * (h - 1);
    for (j = 0; j < h / 2; j++) {
        for (k = 0; k < row; k++) {
            tmp = *pa;
            *pa++ = *pb;
            *pb++ = tmp;
        }
        pb -= row * 2;
    }
}

void func_80014948(ImgSet *s)
{
    u32 i;

    for (i = 0; i < s->count; i++) {
        if (!(s->img[i].flags & 1)) {
            flip(s->img[i].data, s->img[i].w * 4, s->img[i].h);
        } else {
            flip(s->img[i].data, s->img[i].w * 8, s->img[i].h);
        }
    }
}
