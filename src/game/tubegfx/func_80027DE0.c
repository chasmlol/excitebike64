#include "ctx.h"

typedef struct { s32 x, y, z; } V3i;
typedef struct { u8 pad[0x628]; s16 f628; u8 rest[0x7E0 - 0x62A]; } Rec7E0;
typedef struct { u8 pad[0x46]; s16 next; u8 pad2[0x60 - 0x48]; u32 bits; u8 rest[0x6C - 0x64]; } Node;
typedef struct { u8 pad[0x58]; u16 a[4]; u16 b[4]; u8 rest[0xA0 - 0x68]; } Trk;

extern V3i D_800E8090[];
extern Rec7E0 D_800E37C0[];
extern s32 D_800C9654[];
extern Node *D_800EE498;
extern s32 D_800E1898;
extern Trk *D_800EFD64;
extern Trk D_800CB210[];

void func_8004AB24(f32 *param_1, s32 *param_2, f32 *param_3, s32 param_4);

void func_80027DE0(s32 a, s32 *b, s32 *c)
{
    f32 v[3];
    s32 n;
    f32 t, fa, fb, fx, fy;
    s32 i, sh, sh4;
    s32 a0, a1, b0, b1;
    u32 w, w2;
    Node *nd, *base;
    s32 x, y, j, g;

    v[0] = (f32)D_800E8090[a].x / 1000.0f;
    v[1] = (f32)D_800E8090[a].y / 1000.0f;
    v[2] = (f32)D_800E8090[a].z / 1000.0f;
    func_8004AB24(v, &n, &t, D_800E37C0[D_800C9654[a]].f628);
    base = D_800EE498;
    g = D_800E1898;
    i = g - 1;
    nd = (Node *)(n * sizeof(Node) + (u32)base);
    j = nd->next;
    if (i > 2) {
        i = 2;
    }
    i *= 8;
    sh4 = i + 4;
    w = nd->bits;
    a0 = (w >> sh4) & 0xF;
    if (a0 >= 8) a0 -= 16;
    a1 = (w >> i) & 0xF;
    if (a1 >= 8) a1 -= 16;
    w2 = ((Node *)(j * sizeof(Node) + (u32)base))->bits;
    b0 = (w2 >> sh4) & 0xF;
    if (b0 >= 8) b0 -= 16;
    b1 = (w2 >> i) & 0xF;
    if (b1 >= 8) b1 -= 16;
    b0 -= a0;
    b0 *= 16;
    b1 -= a1;
    b1 *= 2;
    fa = (f32)b0 * t;
    fb = (f32)b1 * t;
    a0 *= 16;
    a1 *= 2;
    fx = (f32)a0 + fa;
    fy = (f32)a1 + fb;
    x = (s32)fx;
    y = (s32)fy;
    x = D_800EFD64->a[g - 1] - x;
    y = D_800EFD64->b[g - 1] - y;
    if (y < x) {
        x = y;
    }
    if (D_800EFD64 - D_800CB210 == 8 && g == 1) {
        x = 1000;
        y = 0x41A;
    }
    *b = x;
    *c = y;
}
