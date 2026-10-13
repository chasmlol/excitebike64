#include "ctx.h"

typedef struct { u16 c[256]; } Pal;
typedef struct { u8 r, g, b; } Rgb;
typedef struct { s32 pad0; Pal *pal; } Img;
typedef struct { s32 pad0; u32 *tbl; } ResFile;
typedef struct { s32 *f0, *f4, *f8, *fC; u8 pad[0x18]; } Ent28;
typedef struct { void *a[2]; void *b[2]; } Rec16;

typedef struct {
    void *f0;
    void *f4;
    Img *f8;
    Pal *fC;
    Pal *f10;
    Pal *f14;
    Pal *f18;
    u16 pal[256];
    Rgb col;
    u8 pad21F;
    void *f220;
    void *a224[2];
    void *a22C[2];
    void *a234[2];
    void *b23C[2];
    u8 rest[0xA80 - 0x244];
} Plr;

typedef struct {
    u32 flags;
    u8 pad4[0x18];
    s32 w1C[4];
    u8 pad2C[0x43C - 0x2C];
    u8 b43C[8];
} Glob;

extern s32 D_800C2358;
extern s32 *D_800C23C8[];
extern Ent28 D_800C26B8[];
extern u8 *D_800C29E0;
extern s32 D_800C8C70;
extern s32 D_800D06E8;
extern s32 D_800D06FC;
extern s32 D_800D0C88;
extern char D_800D5EBC[];
extern Plr D_800DC150[];
extern Rec16 D_800E1568[];
extern Rgb *D_800E15D8;
extern Img *D_800E1600[][6];
extern Pal *D_800E1880[];
extern u8 *D_800E1A78;
extern void *D_800E1A80[3];
extern Glob D_800EFD70;

void func_800C0258(s32 a0);
void func_8000ED10(u8 *a0, u8 a1, s32 a2);
void func_80014C80(u32 *param_1);
void func_80014CCC(u32 *param_1);
void func_800BFEA4(u32 *p);
void func_8003C720(u8 *tbl, u32 *list);
void func_80014AE4(void **param_1, u32 *param_2, s32 param_3, u32 param_4);
s32 func_800C0174(ResFile *r, s32 idx);
void *func_800BF79C(ResFile *r, s32 id);
void *func_800BC908(s32 size, s32 flags, char *file, s32 line);
void func_800AEC34(void *dst, void *src, s32 n);
void func_80069CA0(s32 a, s32 b);
void func_800712E4(s32 a);
Gfx *func_80013D70(s32 flags, u32 *mdl, s32 segs, u8 *gdl);

void func_80000450(ResFile *r, s32 n, s32 **ids, s32 arg3, s32 arg4, s32 arg5)
{
    s32 buf[2];
    void *tmp;
    Ent28 *e;
    s32 c;
    s32 i;
    s32 k;
    s32 id;
    u32 *m;
    Rgb *col;
    void *x;
    s32 len;

    e = &D_800C26B8[arg5];
    if (D_800C2358 == 0) {
        if (arg5 == 0) {
            func_800712E4(n);
        }
        D_800DC150[n].f8 = func_800BF79C(r, *D_800C23C8[arg3]);
    }
    if (D_800EFD70.flags & 0x10) {
        D_800DC150[n].f18 = func_800BC908(0xC00, 0, D_800D5EBC, 0x1FF);
    } else {
        D_800DC150[n].f14 = func_800BC908(0x200, 0, D_800D5EBC, 0x203);
    }
    if (!(D_800EFD70.flags & 0x10)) {
        c = D_800EFD70.b43C[n];
    } else {
        c = D_800EFD70.w1C[n & 3] & 3;
    }
    col = &D_800DC150[n].col;
    func_8000ED10((u8 *)col, 0xFF, 3);
    k = func_800C0174(r, *ids[c]);
    if (D_800EFD70.flags & 0x10) {
        for (i = 0; i < 6; i++) {
            D_800E1600[n][i] = func_800BC908(k, 0, D_800D5EBC, 0x220);
            tmp = func_800BF79C(r, *ids[i]);
            func_80014C80((u32 *)&tmp);
            func_800AEC34(D_800E1600[n][i], tmp, k);
            func_800BFEA4(&r->tbl[*ids[i]]);
            func_80014CCC((u32 *)&D_800E1600[n][i]);
        }
        D_800DC150[n].f0 = D_800E1600[n][0];
        D_800DC150[n].fC = 0;
    } else if (n >= 6) {
        D_800DC150[n].f0 = func_800BF79C(r, D_800D0C88);
        D_800E1A80[0] = ((Img *)func_800BF79C(r, D_800D0C88))->pal;
        D_800E1A80[1] = ((Img *)func_800BF79C(r, D_800D06E8))->pal;
        D_800E1A80[2] = ((Img *)func_800BF79C(r, D_800D06FC))->pal;
        D_800DC150[n].fC = D_800E1A80[n % 3];
    } else {
        tmp = func_800BF79C(r, *ids[c]);
        D_800DC150[n].f0 = tmp;
        D_800DC150[n].fC = ((Img *)tmp)->pal;
        *col = D_800E15D8[c];
    }
    if (D_800C2358 != 0) {
        if (n < 6) {
            D_800DC150[n].f4 = func_800BF79C(r, *e->f4);
        } else {
            D_800DC150[n].f4 = func_800BF79C(r, *D_800C26B8[0].f4);
        }
    }
    if (D_800EFD70.flags & 0x10) {
        for (i = 0; i < 6; i++) {
            D_800DC150[n].f18[i] = *D_800E1600[n][i]->pal;
        }
    } else {
        *D_800DC150[n].f14 = *D_800DC150[n].fC;
    }
    if (D_800C2358 == 0) {
        if (arg4 == 0) {
            D_800DC150[n].f10 = D_800DC150[n].f8->pal;
        } else {
            D_800DC150[n].f10 = D_800E1880[arg3 * 3 + (arg4 - 1)];
        }
        __builtin_memcpy(D_800DC150[n].pal, D_800DC150[n].f10, sizeof(Pal));
        D_800DC150[n].pal[0] &= 0xFFFE;
        D_800DC150[n].f10->c[0] &= 0xFFFE;
        D_800DC150[n].f8->pal->c[0] &= 0xFFFE;
    }
    if (D_800E1568[arg5].a[0] == 0) {
        for (k = 0; k < 2; k++) {
            if (D_800C2358 != 0) {
                if (k == 0) {
                    continue;
                }
                id = *e->f0;
            } else if (k != 0) {
                id = *e->f8;
            } else {
                id = *e->fC;
            }
            func_800C0258((s32)(D_800C29E0 + 0x1E000));
            m = (u32 *)((u32)func_800BF79C(r, id) & ~3);
            func_8003C720(D_800E1A78, m);
            buf[0] = (s32)D_800DC150[n].f0;
            D_800C8C70 = 0x06000000;
            buf[1] = (s32)D_800DC150[n].f4;
            if (D_800C2358 != 0) {
                func_80014AE4(&D_800E1568[arg5].a[k], m, (s32)buf, 0xC);
                func_80069CA0((s32)D_800DC150[n].f4, 0);
                func_80069CA0((s32)D_800DC150[n].f0, 0);
            } else {
                func_80014AE4(&D_800E1568[arg5].a[k], m, (s32)buf, 0);
            }
            x = func_800BC908(m[0] << 4, 0, D_800D5EBC, 0x294);
            D_800E1568[arg5].b[k] = x;
            func_800AEC34(x, (void *)m[1], m[0] << 4);
            if (k == 0 && n == 0) {
                m[1] = 0x01000000;
                D_800C8C70 = 0x06000000;
                len = (u32)func_80013D70(0x23, m, 0, D_800C29E0) - (u32)D_800C29E0;
                x = func_800BC908(len, 0, D_800D5EBC, 0x2A0);
                D_800DC150[0].f220 = x;
                func_800AEC34(x, D_800C29E0, len);
            }
            func_800BFEA4(&r->tbl[id]);
            func_800C0258(0);
        }
    }
    D_800DC150[n].a22C[0] = D_800E1568[arg5].b[0];
    D_800DC150[n].a22C[1] = D_800E1568[arg5].b[1];
    D_800DC150[n].a224[0] = D_800E1568[arg5].a[0];
    D_800DC150[n].a224[1] = D_800E1568[arg5].a[1];
    for (k = 0; k < 2; k++) {
        D_800DC150[n].a234[k] = func_800BC908(0x3C0, 0, D_800D5EBC, 0x2B3);
        if (D_800C2358 == 0) {
            D_800DC150[n].b23C[k] = func_800BC908(0x280, 0, D_800D5EBC, 0x2B6);
        }
    }
}
