#include "ctx.h"

typedef struct {
    s32 w0;
    s32 w1;
    u8 x8;
    u8 x9;
    u8 b0a;
    u8 b0b;
    u8 b0c;
    u8 flag;
    s16 h0;
    s16 h1;
    s16 pad12;
} Ent14;

extern s32 D_800F4F8C;
extern Ent14 *D_800CE1FC;

void func_8008D3B4(s32 x, s32 y, s32 w0, s32 b0a, s32 w1, s32 b0b, s32 b0c, s32 h0, s32 h1)
{
    s32 idx = D_800F4F8C * y + x;

    if ((b0c & 2) || D_800CE1FC[idx].w0 != w0 || D_800CE1FC[idx].w1 != w1 || D_800CE1FC[idx].b0a != b0a ||
        D_800CE1FC[idx].b0b != b0b || D_800CE1FC[idx].b0c != b0c || D_800CE1FC[idx].h0 != h0 ||
        D_800CE1FC[idx].h1 != h1) {
        D_800CE1FC[idx].flag = 1;
    }
    D_800CE1FC[idx].w0 = w0;
    D_800CE1FC[idx].w1 = w1;
    D_800CE1FC[idx].b0a = b0a;
    D_800CE1FC[idx].b0b = b0b;
    D_800CE1FC[idx].b0c = b0c & ~2;
    D_800CE1FC[idx].h0 = h0;
    D_800CE1FC[idx].h1 = h1;
}
