#include "ctx.h"

typedef struct {
    u8 pad[4];
    u8 *img;
} Tex;

typedef struct {
    u8 pad[4];
    Tex *tex;
    u8 pad2[0x50];
    s32 *obj;
} Res;

extern Res *D_800CE200;
extern s32 D_800F13F0;
extern s32 *func_800C1E60(s32 *p1, s32 p2, s32 p3, s32 p4, s32 p5);
extern void func_800C1F2C(s32 *p1, s32 p2);
extern void func_800C0ACC(s32 *p1, s32 p2, s32 p3);
extern s32 *func_8000A690(s32 a0, s32 a1, u8 *a2, Gfx *a3, s32 a4, s32 a5, s32 a6, s32 a7, s32 a8);

#define PKT(a, b) { Gfx *_g = (Gfx *)(gdl++); _g->words.w0 = (a); _g->words.w1 = (b); }

s32 *func_8008F230(Gfx *gdl, s32 a1, s32 a2, s32 a3)
{
    s32 *ret;
    u8 *tb;

    PKT(0xE3001001, 0x8000);
    PKT(0xFC119623, 0xFF2FFFFF);
    PKT(0xFD100000, (u32)D_800CE200->tex->img);
    PKT(0xE8000000, 0);
    PKT(0xF5000100, 0x07000000);
    PKT(0xE6000000, 0);
    PKT(0xF0000000, 0x073FC000);
    PKT(0xE7000000, 0);
    PKT(0xD7000002, 0x80008000);
    PKT(0xE3000C00, 0);
    PKT(0xE200001C, 0x0C084000);
    tb = (u8 *)D_800CE200->tex + 0x10;
    ret = func_8000A690(0x140 - tb[0] * 4, 0x78 - (tb[1] >> 1), tb, gdl, a2, 1, 1, 3, 0);
    func_800C1E60(D_800CE200->obj, 0x40, 5, 2, 0x21);
    func_800C1F2C(D_800CE200->obj, a3);
    func_800C0ACC(D_800CE200->obj, 0x140, D_800F13F0 / 2);
    return ret;
}
