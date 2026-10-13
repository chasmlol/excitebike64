#include "ctx.h"

typedef struct {
    s32 w0;
    s32 count;
    s32 w2;
    s32 w3;
    s8 *data;
} EbSeq;

typedef struct {
    s32 pad0[4];
    EbSeq *seq;
    s32 pad1[10];
} EbRow;

typedef struct {
    u8 pad0[0x30];
    s32 f30;
    s32 pad34;
    s32 f38;
    s32 f3C;
    s16 f40;
    s16 f42;
    u8 pad44[0x370 - 0x44];
    u8 f370[0x3AC - 0x370];
    s32 f3AC;
    u8 pad3B0[0x3D0 - 0x3B0];
    s8 f3D0;
    u8 pad3D1[0x658 - 0x3D1];
    s32 *f658;
    u8 pad65C[0x7E0 - 0x65C];
} Blk;

typedef struct {
    s32 a, b, c, d;
} Rec16;

extern s32 D_800C2358;
extern Blk D_800E37C0[];
extern EbRow D_800C9A40[];

void func_80013650(s32 *a, u8 *b);
void func_80011A58(s32 *a, s32 b, s32 c, s32 d);
void func_80011C1C(s32 *a, s32 *b, s32 *c);
void func_8003C4A4(s32 *a, s32 b);

void func_8003EAC4(s32 *p1, s32 p2)
{
    Blk *blk;
    Blk *gp;
    s32 v;
    s32 n;
    s32 nn;
    s32 idx;
    s32 i;
    EbSeq *e;
    s8 *bp;
    s8 *d;
    s32 k;
    Rec16 *src;
    Rec16 *dst;
    Rec16 *end;
    s32 a88[10];
    s32 a60[18];

    if (D_800C2358 == 0) {
        gp = D_800E37C0;
        blk = &gp[p2];
        v = blk->f38;
        n = blk->f3C;
        func_80013650(p1 + 0x10, D_800E37C0[p2].f370);
        v = v - 0x32;
        if (v > 200) {
            v = 200;
        }
        if (v < -0x3c) {
            v = -0x3c;
        }
        p1[0x42] = v - 0x222;
        p1[0x43] = 0x106 - (v * 2) / 5;
        func_80011A58(p1 + 0xe4, n * 0x18, 0, 0);
        func_80011A58(a88, 4000, 0, blk->f30);
        func_80011A58(a60, -4000, 0, 0);
        func_80011C1C(a88, a60, p1 + 0x24);
        func_80011A58(p1 + 100, blk->f40, 0, 0);
        func_80011A58(p1 + 0x104, blk->f42, 0, 0);
        if (blk->f3D0 != -1) {
            e = D_800C9A40[blk->f3D0].seq;
            if (e != 0) {
                idx = blk->f3AC >> 8;
                if (idx < 0) {
                    idx = 0;
                }
                if (idx >= e->count) {
                    idx = e->count - 1;
                }
                d = e->data;
                k = 6;
                for (i = 1; i < 2; i++) {
                    bp = d + idx * k * 2 + k + i * 3;
                    func_80011A58(p1 + i * 0x20 + 4, bp[0] << 8, bp[1] << 8, bp[2] << 8);
                }
            }
        }
        func_8003C4A4(p1, 10);
        dst = (Rec16 *)D_800E37C0[p2].f658;
        src = (Rec16 *)p1;
        end = src + 0x50;
        do {
            *dst = *src;
            src++;
            dst++;
        } while (src != end);
    }
}
