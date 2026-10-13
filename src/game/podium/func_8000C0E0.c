#include "ctx.h"

typedef struct {
    u8 pad[0x14];
    u32 tex;
    u8 pad2[0xA80 - 0x18];
} Plr;

extern s32 D_800E1B9C;
extern u32 *D_800E1BC4;
extern s32 D_800E1BEC;
extern s32 D_800E30A8;
extern s32 D_800E1BF0[];
extern s32 D_800E1B80;
extern s32 D_800E1B78;
extern s32 D_800E1BC8[][3];
extern s16 D_800E1B88[][3];
extern Plr D_800DC150[];
typedef struct { u8 pad[0x22]; s16 n; } Cnt;
extern Cnt D_800F05A8;

void func_80002418(Gfx *gdl, s32 x, s32 a2, s32 a3);
void func_8000B938(s32 p1, s32 p2, s32 p3, s32 p4, s32 p5, s32 p6, s32 p7, s32 p8, s32 p9);
Gfx *func_80013D70(s32 a0, s32 a1, void *a2, Gfx *gdl);

#define W2(pkt, a, b) { Gfx *_g = (Gfx *)(pkt); _g->words.w0 = (a); _g->words.w1 = (b); }
#define W2R(pkt, a, b) { Gfx *_g = (Gfx *)(pkt); _g->words.w1 = (b); _g->words.w0 = (a); }

Gfx *func_8000C0E0(Gfx *gdl, s32 x)
{
    s32 i;
    s32 off;
    Plr *pl;
    Plr *cur;
    s32 *pc;
    s16 *ps;

    func_80002418(gdl, x, 0, 1);
    W2(gdl++, 0xE3001001, 0x8000);
    W2(gdl++, 0xE200001C, 0x442230);
    W2(gdl++, 0xFCFFFFFF, 0xFFFCF87C);
    W2(gdl++, 0xFD100000, D_800E1BC4[1]);
    W2(gdl++, 0xE8000000, 0);
    W2(gdl++, 0xF5000100, 0x07000000);
    W2(gdl++, 0xE6000000, 0);
    W2(gdl++, 0xF0000000, 0x073FC000);
    W2(gdl++, 0xE7000000, 0);
    i = 0;
    func_8000B938(D_800E1BEC + D_800E30A8 * 0x100, 0, 0, D_800E1B9C, D_800E1BF0[0], D_800E1BF0[1], D_800E1BF0[2], 0x10, 0);
    W2(gdl++, 0xDA380003, D_800E1BEC + D_800E30A8 * 0x100);
    gdl = func_80013D70(0, D_800E1B80, &D_800E1BC4, gdl);
    for (; i < D_800F05A8.n; i++) {
        func_8000B938((s32)&((Mtx *)(D_800E1BEC + D_800E30A8 * 0x100))[i + 1], D_800E1B88[i][0], D_800E1B88[i][1], D_800E1B88[i][2], D_800E1BC8[i][0], D_800E1BC8[i][1], D_800E1BC8[i][2], 0x10, 0);
        W2(gdl++, 0xDA380003, (s32)&((Mtx *)(D_800E1BEC + D_800E30A8 * 0x100))[i + 1]);
        W2(gdl++, 0xFD100000, D_800DC150[i].tex);
        W2(gdl++, 0xE8000000, 0);
        W2(gdl++, 0xF5000100, 0x07000000);
        W2(gdl++, 0xE6000000, 0);
        W2(gdl++, 0xF0000000, 0x073FC000);
        W2(gdl++, 0xE7000000, 0);
        gdl = func_80013D70(0, D_800E1B78, &D_800DC150[i], gdl);
    }
    return gdl;
}
