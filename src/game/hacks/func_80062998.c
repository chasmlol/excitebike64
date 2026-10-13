#include "ctx.h"

typedef struct { s32 x, y, z; } Ent;
typedef struct { s32 x, y, z, w; } Pos;
typedef struct {
    u8 pad0[0x458];
    s32 s458;
    u8 pad45C[0x494 - 0x45C];
    f32 f494;
    f32 f498;
    f32 f49C;
    u8 pad4A0[0x4FC - 0x4A0];
    s32 s4FC;
    u8 pad500[0x538 - 0x500];
    f32 f538;
    f32 f53C;
    f32 f540;
    u8 pad544[0x7A5 - 0x544];
    u8 b7A5;
    u8 pad7A6[0x7E0 - 0x7A6];
} Bike;
typedef struct { u8 pad[0x1A]; s16 h; u8 pad2[0x40 - 0x1C]; } Obj;

extern Pos *D_800F143C;
extern s32 D_800F0B5C;
extern Ent *D_800F0B64;
extern s16 D_800C2C60[];
extern Bike D_800E37C0[];
extern s32 D_800E76D0;
extern Obj *D_800CCCD0;

void func_80062998(void)
{
    s32 c, s;
    s32 i, j, m;
    u32 k;
    Bike *b, *bb, *base;
    Pos *p;
    Ent *e;
    Obj *o;
    s32 dx, dz, a, v, t, r, w;
    s32 ang;

    k = ((u8 *)D_800F143C)[0xE] >> 4;
    c = D_800C2C60[k | 0xC00];
    s = D_800C2C60[k];
    for (m = 0; m < D_800F0B5C; m++) {
        D_800F0B64[m].z = 0;
    }
    for (m = 0; m < 8; m++) {
        if (D_800E37C0[m].b7A5 != 0 || D_800E37C0[m].s458 >= 0x15 || D_800E37C0[m].s4FC >= 0x15) continue;
        for (i = 0; i < D_800F0B5C; i++) {
            dx = (s32)(D_800E37C0[m].f538 * 1000.0f) - D_800F143C[i].x;
            if ((u32)(dx + 0x10000) > 0x20000) continue;
            dz = (s32)(D_800E37C0[m].f540 * 1000.0f) - D_800F143C[i].z;
            if ((u32)(dz + 0x10000) > 0x20000) continue;
            a = (dx * c + dz * s) >> 14;
            dx = (s32)(D_800E37C0[m].f494 * 1000.0f) - D_800F143C[i].x;
            if ((u32)(dx + 0x10000) > 0x20000) continue;
            dz = (s32)(D_800E37C0[m].f49C * 1000.0f) - D_800F143C[i].z;
            if ((u32)(dz + 0x10000) > 0x20000) continue;
            v = (dx * c + dz * s) >> 14;
            if (a > 0 && v < 0) {
                r = 0x100;
            } else if (a < 0 && v > 0) {
                r = 0x100;
            } else {
                if (a > 0) {
                    r = v;
                    if (a < r) r = a;
                    v = 400;
                    v -= r;
                    r = (v << 8) / 400;
                } else if (a < 0) {
                    r = v;
                    if (r < a) r = a;
                    v = r + 400;
                    r = (v << 8) / 400;
                } else {
                    goto store;
                }
                if (r < 0) r = 0;
                if (r >= 0x100) r = 0xFF;
            }
store:
            if (D_800F0B64[i].z < r) D_800F0B64[i].z = r;
        }
    }
    i = 0;
    if (D_800F0B5C > 0) {
        do {
            e = &D_800F0B64[i];
            t = e->z;
            w = (D_800C2C60[(((D_800E76D0 << 9) + i * 0x1000) >> 4) & 0xFFF] * (0xFF - t)) >> 15;
            w -= t;
            w /= 2;
            w -= 200;
            if (w < e->y - 16) {
                e->y = e->y - 16;
            } else {
                e->y = w;
            }
            D_800CCCD0[i].h = (e->x + e->y) >> 4;
            i++;
        } while (i < D_800F0B5C);
    }
}
