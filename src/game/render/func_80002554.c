#include "ctx.h"

typedef struct { u8 pad0[0x5AA]; s16 sta; u8 pad1[0x7E0 - 0x5AC]; } E37;
typedef struct { u8 pad0[0x2C]; s32 mode; u8 pad1[0xA0 - 0x30]; } Trk;
typedef struct { u32 bits; u8 pad[0x1C]; } Cell;
typedef struct { s32 x, y, w, h; } Box;

extern s32 D_800C2384;
extern s32 D_800C2594;
extern s32 D_800C9654[];
extern Cell *D_800C9644;
extern s32 D_800C9550[];
extern s32 D_800E1898;
extern s32 D_800E1A58[];
extern E37 D_800E37C0[];
extern s16 D_800E3D6A;
extern s32 D_800E8090[][3];
extern Box D_800E1718[][4];
extern f32 D_800E1AA0[];
extern f32 D_800E1AB0[];
extern Trk *D_800EFD64;
extern Trk D_800CB210[];

s32 func_80033AC8(s32 x, s32 y, s32 z, s32 *p4, s32 *p5, s32 *p6, s32 *p7, s32 p8, s32 p9);

s32 func_80002554(s32 x, s32 y, s32 z, s32 k, s32 c, s32 q, s32 n)
{
    s32 sx, sy, sz, d;
    s32 i3, i4, m;
    s32 x0, x1, y0, y1;
    Box *b;
    u8 *bb;

    if (D_800C2384 != 0 && n == 0) {
        return 3;
    }
    if (!(D_800E37C0[n].sta == 9 && D_800C9654[k] == n)) {
        if ((u32)D_800EFD64->mode < 2 && c != -1) {
            Cell *cp = &D_800C9644[c];
            if (!((cp[1].bits >> (k * 2)) & 1)) {
                return 0;
            }
        }
    }
    if (D_800EFD64->mode == 0) {
        if (func_80033AC8(x >> 4, y >> 4, z >> 4, &sx, &sy, &sz, &d, q, k) != 0) {
            return 0;
        }
    } else {
        if (func_80033AC8((x - D_800E8090[k][0]) >> 4, (y - D_800E8090[k][1]) >> 4,
                          (z - D_800E8090[k][2]) >> 4, &sx, &sy, &sz, &d, q, k) != 0) {
            return 0;
        }
    }
    if (d <= 0) {
        return 0;
    }
    if (D_800EFD64->mode == 2 && d > 8000) {
        return 0;
    }
    D_800E1A58[n] = d;
    bb = (u8 *)D_800E1718;
    b = (Box *)(bb + (k * 16 + (D_800C2594 - 1) * 64));
    x0 = b->x;
    x1 = b->w + x0;
    y0 = b->y;
    y1 = b->h + y0;
    i3 = (D_800E1AA0[k] * 90.0f) / d;
    i4 = (D_800E1AB0[k] * 90.0f) / d;
    if (sx + i3 < x0) {
        return 0;
    }
    if (x1 < sx - i3) {
        return 0;
    }
    if (sy + i4 < y0) {
        return 0;
    }
    if (y1 < sy - i4) {
        return 0;
    }
    if (D_800EFD64 - D_800CB210 == 20 && D_800E3D6A == 4) {
        return 3;
    }
    m = D_800C9550[D_800E1898 - 1];
    if (m * 6 / 10 < d) {
        return 1;
    }
    if (m * 4 / 10 < d) {
        return 2;
    }
    return 3;
}
