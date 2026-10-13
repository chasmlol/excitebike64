#include "ctx.h"

typedef struct Plr {
    u8 pad0[0xA8];
    s32 cur;   /* A8 */
    s32 fAC;   /* AC */
    s32 fB0;
    s32 fB4;
    s32 fB8;
    s32 fBC;
    u8 pad1[0xD4 - 0xC0];
    s32 fD4;
    s32 fD8;
    u8 pad2[0xE0 - 0xDC];
    s32 fE0;
} Plr;

typedef struct Rec {
    s32 f0;
    s32 f4;
    s32 f8;
    s32 fC;
    s32 f10;
} Rec;

typedef struct { u8 pad[0x662]; s16 f; u8 rest[0x7E0 - 0x664]; } Tbl7E0;

extern s32 D_800CA858[];
extern s32 D_800CA850;
extern s32 D_800CA820[];
extern Plr *D_800EFD24;
extern s32 D_800EECC8;
extern s32 D_800EFD00[];
extern Rec *D_800EFD4C;
extern s32 D_800EECD4;
extern s32 D_800EECDC;
extern s32 D_800EECB8;
extern s32 D_800E76D0;
extern s16 D_800F05CA;
extern s32 D_800C2358;
extern Tbl7E0 D_800E37C0[];
void func_800AEC34(void *dst, void *src, s32 n);

void func_8004C644(void)
{
    s32 v;
    s32 n;
    s32 lo;
    s32 t;
    s32 f;
    s32 c;
    s32 e;
    Plr *p;
    Rec *r;

    if (D_800CA858[0] == 0) {
        return;
    }
    if (D_800EFD24->cur != -1 && D_800EECC8 >= 0) {
        return;
    }
    v = D_800EFD00[0];
    if (v < 0) {
        t = (v >> 16) & 0xFF;
        if (t == 1) {
            n = (v >> 8) & 0xFF;
            n = D_800E37C0[n].f;
            lo = v & 0xFF;
            if (lo < n) {
                n = lo;
            }
            lo = D_800F05CA - 2;
            if (lo < n) {
                n = lo;
            }
        } else if (t == 2) {
            n = (v >> 8) & 0xFF;
            n = D_800E37C0[n].f;
            lo = v & 0xFF;
            if (n < lo) {
                n = lo;
            }
            if (n <= 0) {
                n = 1;
            }
        } else {
            goto bad;
        }
        v = D_800CA820[n];
        goto ok;
    bad:
        D_800CA858[0]--;
        return;
    }
ok:
    if (D_800C2358 == 0) {
        if (D_800EFD24->cur != -1 && D_800EECC8 >= 0) {
            f = 1;
        } else {
            D_800EFD24->cur = v;
            r = &D_800EFD4C[v];
            D_800EFD24->fD4 = 0;
            D_800EFD24->fD8 = 0;
            D_800EFD24->fAC = 0;
            D_800EFD24->fB8 = r->f8 + D_800EECD4;
            D_800EECC8 = 0;
            D_800EFD24->fBC = r->fC;
            D_800EECDC = 0;
            D_800EFD24->fB4 = r->f10;
            f = 0;
        }
    }
    if (f == 0) {
        p = D_800EFD24;
        p->fD4 = 1;
        e = D_800CA850;
        c = --D_800CA858[0];
        p->fE0 = e;
        if (c != 0) {
            func_800AEC34(D_800EFD00, D_800EFD00 + 1, c << 2);
        }
        D_800EECB8 = D_800E76D0;
    }
}
