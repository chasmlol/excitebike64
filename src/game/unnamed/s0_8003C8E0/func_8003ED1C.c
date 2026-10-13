#include "ctx.h"

typedef struct {
    s32 w00;
    s32 w04;
    s32 w08;
    u8 _0C[0x22 - 0x0C];
    u16 f22;
    u8 _24[0x44 - 0x24];
    u8 f44;
    u8 _45[0x3BC - 0x45];
    s16 f3BC;
    u8 _3BE[2];
    s16 f3C0;
    u8 _3C2[0x419 - 0x3C2];
    u8 f419;
    u8 f41A;
    u8 _41B[0x7A8 - 0x41B];
    s8 f7A8;
    u8 _7A9[0x7E0 - 0x7A9];
} Rec;

extern s32 D_800EE140;
extern Rec D_800E37C0[];
extern u8 D_800EA7B8[];
extern u32 D_800E3770[];
u32 func_8000EFE8(void);
void func_8001254C(s32 a, s32 b, u16 *c, s32 *d);

void func_8003ED1C(void)
{
    s32 i, j, best, bi, dx, dz, dist;
    u16 ang, sang;
    s16 v;

    if (D_800EE140 != 0) return;
    for (i = 0; i < 8; i++) {
        best = 625;
        bi = -1;
        if (D_800E37C0[i].f44) {
            D_800E37C0[i].f3BC = 0;
            if (i) dx = 1; else dx = 2;
            continue;
        }
        if (((D_800E3770[i] & 0x20) && D_800E37C0[i].f41A && D_800E37C0[i].f419) || D_800E37C0[i].f7A8) {
            D_800EA7B8[i] = 1;
            v = 24000;
        } else {
            D_800EA7B8[i] = 0;
            for (j = 0; j < 8; j++) {
                if (j == i) continue;
                dx = (D_800E37C0[j].w00 >> 4) - (D_800E37C0[i].w00 >> 4);
                dz = (D_800E37C0[j].w08 >> 4) - (D_800E37C0[i].w08 >> 4);
                while (dx > 0xFFFF) { dx >>= 1; dz >>= 1; }
                while (dx < -0xFFFF) { dx >>= 1; dz >>= 1; }
                while (dz > 0xFFFF) { dz >>= 1; dx >>= 1; }
                while (dz < -0xFFFF) { dz >>= 1; dx >>= 1; }
                func_8001254C(dx, dz, &ang, &dist);
                if (dist < best) {
                    best = dist;
                    bi = j;
                    sang = ang;
                }
            }
            v = 0;
            if (bi != -1 && D_800E37C0[i].f3C0 <= 0) v = -sang - D_800E37C0[i].f22;
            if (v < -0x4000) v = -0x4000;
            if (v > 0x4000) v = 0x4000;
        }
        if (D_800E37C0[i].f3BC < v) D_800E37C0[i].f3BC += 0x300;
        if (D_800E37C0[i].f3BC > v) D_800E37C0[i].f3BC -= 0x300;
        if (--D_800E37C0[i].f3C0 < -0x78) D_800E37C0[i].f3C0 = (s32)(func_8000EFE8() * 300) / 256 + 150;
    }
}
