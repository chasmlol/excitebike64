#include "ctx.h"

extern s32 D_800CEC00;
extern u8 *D_800F5A40;
extern u8 D_800F1BD4[];
extern s32 D_800CA920;
extern s32 *D_800C29CC;

#define NES ((u8 *)0x80380000)

typedef struct {
    u32 a;
    u32 b;
    u8 pad[0x1C];
} Ch;

typedef struct {
    u8 pad[0x1412C];
    Ch ch[4];
} Nes;

void func_8009FA04(volatile s32 *p)
{
    s32 w, v;
    s32 f;
    s32 k;
    s32 hi, lo;
    u8 b;
    u8 *m;
    u8 *g;

    w = *p;
    f = ((w & 0x800) != 0) << 4;
    if (w & 0x1000) f |= 0x20;
    if (w & 0x2000) f |= 0x40;
    if (w & 0x4000) f |= 0x80;
    if (w & 0x10) f |= 1;
    if (w & 0x20000) f |= 2;
    if (w & 0x20) f |= 2;
    if (w & 0x8000) f |= 4;
    if (w & 0x40) f |= 8;
    if (D_800CEC00 != 0) {
        if (--D_800CEC00 == 0) {
            __builtin_memcpy(NES, D_800F5A40, 0x800);
            __builtin_memcpy(NES + 0x12000, D_800F5A40 + 0x800, 0x2000);
            m = D_800F5A40;
            g = (u8 *)0x80390000;
            *(u16 *)(g + 0x4100) = *(u16 *)(m + 0x2800);
            *(u16 *)(g + 0x4114) = *(u16 *)(m + 0x2802);
            g[0x4102] = m[0x2804];
            g[0x4103] = m[0x2805];
            g[0x4104] = m[0x2806];
            g[0x4105] = m[0x2807];
            b = m[0x2808];
            g[0x410D] = b;
            *(u32 *)(g + 0x4118) = m[0x2809];
            g[0x411C] = m[0x280A];
            g[0x411D] = m[0x280B];
            g[0x411F] = m[0x280C];
            g[0x4108] = b & 0x80;
            g[0x4106] = b & 0x40;
            g[0x4107] = b & 0x10;
            g[0x4109] = b & 0x8;
            g[0x410A] = b & 0x4;
            g[0x4120] = 0;
            g[0x4121] = 0;
            *(u32 *)(g + 0x4124) = 0;
            *(u32 *)(g + 0x4128) = 0;
            g[0x410B] = b & 0x2;
            g[0x410C] = b & 0x1;
            for (k = 0; k < 4; k++) {
                ((Nes *)0x80380000)->ch[k].a = 0;
                ((Nes *)0x80380000)->ch[k].b = 0;
            }
            __builtin_memcpy(NES + 0x5E0, D_800F1BD4, 0x220);
        }
    }
    v = *p;
    lo = (v >> 24) & 0xF;
    hi = (v >> 28) & 0xF;
    if ((v & 1) && hi > 8) f |= 0x10;
    if ((*p & 2) && hi > 8) f |= 0x20;
    if ((*p & 4) && lo > 8) f |= 0x40;
    if ((*p & 8) && lo > 8) f |= 0x80;
    *(u8 *)0x80394120 = f;
    if ((*p & 0x28000) == 0x28000) {
        D_800CA920 = 2;
        D_800C29CC[4] = 3;
    }
}
