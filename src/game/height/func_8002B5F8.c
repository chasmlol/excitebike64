#include "ctx.h"

typedef struct {
    s32 w[6];
} Rec6;

extern s32 D_800C9684;
extern u8 D_800E7998[];
extern Rec6 D_800E8030[];

void func_8002B5F8(Rec6 *src)
{
    s32 i;
    s32 t2;
    s32 off;
    s32 v;
    u8 *base;
    s32 o;
    Rec6 *d;

    i = 0;
    base = D_800E7998;
    do {
        v = D_800C9684;
        o = (i * 0x18 + v * 0x180) + (s32)base;
        i++;
        *(Rec6 *)o = *src;
    } while (i < 16);
    D_800E8030[D_800C9684] = *src;
    d = &D_800E8030[D_800C9684];
    d->w[0] <<= 4;
    d->w[1] <<= 4;
    d->w[2] <<= 4;
}
