#include "ctx.h"

typedef struct {
    s32 x0, y0, x1, y1;
    s32 f10, f14, f18, f1C, f20, f24;
    s32 n;
    s32 f2C;
    u16 ang;
    s16 arr[3];
    u8 idx[4];
    void *buf;
} Track;

typedef struct {
    s32 a, b, c;
    s32 data;
} Res;

extern s32 D_800CCB20;
extern Track D_800CCB90[2];
extern s16 D_800C2C60[];
extern s32 D_800E8958;
extern s32 D_800E7728;
extern s32 D_800C29E0;
extern s32 *D_800CCC10[4];
extern s32 D_800F0B48[4];
extern s32 D_800E7808[];

void func_8001254C(s32 a, s32 b, u16 *c, s32 *d);
s32 func_8002E920(s32 x, s32 y, s32 z);
void *func_800BC908(s32 size, u32 flags, char *owner, s32 tag);
Res *func_800BF79C(s32 a, s32 b);
void func_800AEC34(s32 a, s32 b, s32 c);
s32 *func_800BD798(Res *p, u32 size, s32 a, char *b, s32 c);
s32 func_800BD924(Res *param);
void func_80014B84(s32 *p1, Res *a1, s32 *a2, u32 a3);

void func_80062244(void)
{
    s32 i, j;
    s32 r, cx, cy, t;
    Track *p;
    Res *res;
    s32 sp18;
    s32 d, e;

    D_800CCB20 = 0;
    for (i = 0; i < 2; i++) {
        p = &D_800CCB90[i];
        func_8001254C(p->x1 - p->x0, p->y1 - p->y0, &p->ang, &sp18);
        cx = (p->x0 + p->x1) / 2;
        cy = (p->y0 + p->y1) / 2;
        sp18 = p->n * 0x2CEC;
        p->f10 = cx + D_800C2C60[(((s16)p->ang >> 4) - 0x400) & 0xFFF] * sp18 / 0x4000;
        p->f18 = cy + D_800C2C60[p->ang >> 4] * sp18 / 0x4000;
        p->f1C = cx - D_800C2C60[(((s16)p->ang >> 4) - 0x400) & 0xFFF] * sp18 / 0x4000;
        p->f24 = cy - D_800C2C60[p->ang >> 4] * sp18 / 0x4000;
        p->f14 = func_8002E920(cx, 50000, cy);
        p->f20 = func_8002E920(cx, 50000, cy);
        p->f2C = D_800E8958;
        for (j = 0; j < p->n; j++) {
            p->arr[j] = (j << 8) / p->n;
            p->idx[j] = j;
        }
        p->buf = func_800BC908(p->n << 9, 0, "hacks.c", 0x116);
    }
    for (i = 0; i < 4; i++) {
        res = func_800BF79C(D_800E7728, *D_800CCC10[i]);
        sp18 = res->data - (s32)res;
        t = func_800BD924(res);
        d = 0x1E000;
        func_800AEC34(D_800C29E0 + d, res->data, t - sp18);
        d = 0;
        func_800BD798(res, sp18, d, "hacks.c", 0x124);
        e = 0x1E000;
        res->data = D_800C29E0 + e;
        func_80014B84(&D_800F0B48[i], res, D_800E7808, 0xC);
    }
}
