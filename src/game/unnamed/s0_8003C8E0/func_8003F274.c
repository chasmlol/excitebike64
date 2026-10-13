#include "ctx.h"

typedef struct { s32 w[3]; } V3;
typedef struct { s8 v[3]; } B3;
typedef struct { s32 w[9]; } M9;
typedef struct {
    s32 f0;
    V3 pos;          /* 0x04 */
    M9 m;            /* 0x10 */
    u8 pad34[0x0C];  /* 0x34 */
    M9 m2;           /* 0x40 */
    u16 c[3];        /* 0x64 */
    u16 c2[3];       /* 0x6A */
    u8 pad70[0x10];
} Row;

typedef struct {
    u8 pad[0xC];
    s32 *tbl;
    u8 pad2[0x3C - 0x10];
} TEnt;

extern TEnt D_800C9A40[];

void func_80012EBC(s32 *param_1, u16 *param_2, u16 *param_3, u16 *param_4);
void func_80011A58(s32 *param_1, s32 param_2, s32 param_3, s32 param_4);
void func_80011C1C(s32 *a, s32 *b, s32 *c);
void func_8003C4A4(s32 *param_1, s32 param_2);

void func_8003F274(Row *p1, s32 param_2, s32 p3, s32 p4)
{
    s32 *tbl;
    s32 half;
    B3 *pb;
    s32 k;
    s32 sc;
    s32 j; s32 t;
    u16 a, b, c;
    V3 save;

    tbl = D_800C9A40[param_2].tbl;
    half = (tbl[0] * 3 + 7) / 2;
    if (p4 < 0) p4 = 0;
    if (tbl[1] - 1 < p4) p4 = tbl[1] - 1;
    pb = (B3 *)((u8 *)tbl[4] + half * p4 * 2 + 6);

    for (k = 0; k < 15; k++) {
        func_80012EBC((s32 *)&p1[k].m, &a, &b, &c);
        p1[k].c[0] = a;
        p1[k].c[1] = b;
        p1[k].c[2] = c;
    }
    k = 0x100;
    p1[0].c2[0] = *(u16 *)((u8 *)p1 + 6);
    p1[0].c2[1] = *(u16 *)((u8 *)p1 + 0xA);
    p1[0].c2[2] = *(u16 *)((u8 *)p1 + 0xE);
    save = p1[p3].pos;
    for (j = 0; j < 3; j++) {
        
        u16 c0 = p1[p3 + j].c[0];
        s8 x = pb[p3 + j].v[0];
        s8 y;
        s8 z;
        t = (s16)((x << 8) - c0) * k; if (t) p4 = 1; else p4 = 2;
        y = pb[p3 + j].v[1];
        t = (s16)((y << 8) - p1[p3 + j].c[1]) * k; if (t) p4 = 3; else p4 = 4;
        z = pb[p3 + j].v[2];
        t = (s16)((z << 8) - p1[p3 + j].c[2]) * k; if (t) p4 = 5; else p4 = 6;
        func_80011A58((s32 *)&p1[p3 + j].m, (s16)(x << 8), (s16)(y << 8), (s16)(z << 8));
    }
    func_80011C1C((s32 *)&p1[p3].m, (s32 *)((u8 *)p1 + 0xC0), (s32 *)&p1[p3].m2);
    p1[p3].m = p1[p3].m2;
    *(M9 *)&p1[p3].pos = *(M9 *)p1[p3].pad34;
    p1[p3 + 1].f0 = 0;
    p1[p3 + 2].f0 = 1;
    func_8003C4A4((s32 *)&p1[p3], 3);
    p1[p3 + 1].f0 = p3;
    p1[p3 + 2].f0 = p3 + 1;
    p1[p3].pos = save;
}
