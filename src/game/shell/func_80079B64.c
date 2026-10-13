#include "ctx.h"

typedef struct {
    u8 *p;
    u8 pad0[0x60];
    s16 cnt;
    u8 pad1[6];
    u8 flag;
} Rec;

typedef struct {
    u32 flags;
    u8 pad0[8];
    u32 f0c;
    u8 pad1[0x400];
    s32 mode;
    u8 pad2[0x340];
    u32 f754;
} Gs;

extern Gs D_800EFD70;
extern Rec *D_800CA930;
extern s32 D_800CA920;
extern u8 D_800CA9C8;
extern u8 D_800F05F8;
extern u8 D_800F1900;

s32 func_8006F128(void);
void func_8006F5F8();

void func_80079B64(void)
{
    u8 *q;
    s32 old;
    s32 idx;
    u32 fl;
    Gs *g;
    Gs *g2;
    s32 m;
    s32 idx2;
    s32 mode;
    s32 nv;
    u32 v;
    u32 w;
    s32 md;
    s32 lv;
    u32 f;
    u8 *pb;
    u8 *e;
    u8 t8;
    Rec *r;
    u8 *p0;
    u8 *cb;
    u8 *pb2;

    fl = D_800EFD70.flags;
    g = &D_800EFD70;
    if ((fl & 0x28000) == 0 && D_800CA920 != 3) {
        v = fl | 0x20000;
        D_800EFD70.flags = v;
        mode = g->mode;
        switch (mode) {
        case 1: m = 3; break;
        case 2: m = 7; break;
        case 3: m = 0x1f; break;
        }
        func_8006F128();
        r = D_800CA930;
        p0 = r->p;
        cb = &D_800CA9C8;
        idx = ((p0 - cb) * 0x38E38E39) >> 2;
        if (idx < 5) if (idx >= 0) {
            pb = r->p;
            if ((s32)*pb <= (s32)r->cnt + 1) {
                if (!(idx == 4 && D_800F05F8 != 0) && r->flag == 0) {
                    g2 = &D_800EFD70;
                    q = &D_800F1900;
                    lv = q[0x2C0];
                    md = g2->mode;
                    if (md == lv && ((old = q[0x2BF]) >> idx & 1) == 0) {
                        v = old | (1 << idx);
                        q[0x2BF] = v;
                        if ((((u8)v) & m) == m) {
                            if (md != 3) {
                                t8 = q[0x2C0];
                                D_800EFD70.f0c = 0;
                                do {
                                q[0x2BF] = 0;
                                q[0x2C0] = t8 + 1;
                                } while (0);
                                D_800EFD70.flags |= 0x400000;
                            } else {
                                D_800EFD70.flags |= 0x800000;
                            }
                        } else {
                            D_800EFD70.flags |= 0x100000;
                        }
                    }
                }
            }
            idx2 = ((D_800CA930->p - &D_800CA9C8) * 0x38E38E39) >> 2;
            if (idx2 != 4 || D_800F05F8 == 0) {
                pb2 = D_800CA930->p;
                e = pb2 + (D_800EFD70.mode - 1);
                if (D_800CA930->cnt == e[1]) {
                    nv = D_800CA930->cnt + 1;
                    if (nv < *pb2) {
                        e[1] = nv;
                    } else if (D_800CA930->flag == 0) {
                        e[1] = nv;
                    }
                }
            }
            func_8006F5F8(1, 0);
        }
        f = func_8006F128();
        D_800EFD70.f754 = f;
        if (f != 0) {
            D_800EFD70.flags |= 0x200000;
        }
    }
}
