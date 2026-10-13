#include "ctx.h"

typedef struct {
    u8 pad0[0xC];
    s32 fC;
} Sub;

typedef struct {
    u32 n;
    Sub *sub;
    u8 pad8[0x18];
} Ent;

extern s32 D_800E30A8;
extern u8 *D_800C29D8[];
extern s32 D_800C29E0;
extern u8 *D_800EFD64;
extern s32 D_800E77F8;
extern s32 D_800E7790;
extern s32 *D_800E77C8;
extern s32 *D_800C9580;
extern s32 *D_800C9584;
extern s32 D_800C8C7C;
extern char D_800D6A50[];

void func_80014A68(void *a0);
void func_80014A74(void *a0);
s32 func_800BD924(Ent *p);
void func_80027AB4(void);
void func_800AEC34(void *dst, void *src, s32 n);
Ent *func_800BD798(Ent *p, s32 a1, s32 a2, char *a3, s32 a4);
void *func_800BC908(s32 a0, s32 a1, char *a2, s32 a3);
s32 func_80013D70(u32 a0, Sub *a1, s32 a2, s32 a3);
void func_800285C4(void);
void func_80027934(void);

s32 **func_80027574(Ent *h, s32 p2, u32 p3, s32 p4)
{
    u8 *buf;
    s32 off;
    s32 n;
    u32 i;
    s32 **tbl;
    s32 *t;
    s32 sz;
    s32 *nb;
    s32 j;
    s32 d;

    buf = D_800C29D8[1 - D_800E30A8];
    if (!(*(s32 *)(D_800EFD64 + 0x50) & 2)) {
        i = h->sub->fC - (s32)h;
        n = func_800BD924(h);
        func_800AEC34(buf, (u8 *)h + i, n - i);
        h = func_800BD798(h, i, 0, D_800D6A50, 0x51B);
        do { } while (0);
        sz = 0;
        d = (s32)buf - ((s32)h + i);
        if (h->n != 0) {
            do {
                h[sz].sub->fC += d;
                sz++;
            } while (sz < h->n);
        }
    }
    tbl = func_800BC908(h->n * 4, 0, D_800D6A50, 0x524);
    func_80014A68(p4 ? func_800285C4 : NULL);
    if (p4 && D_800E77F8) {
        func_80027AB4();
        D_800C9584 = func_800BC908((h->n + 1) * 4, 0, D_800D6A50, 0x52E);
        D_800E77C8 = D_800C9580 = (s32 *)(D_800C29E0 + 0x1E000);
    }
    for (i = 0; i < h->n; i++) {
        D_800C8C7C = -1;
        t = func_800BC908(func_80013D70(p3 | 1, h[i].sub, p2, D_800C29E0) - D_800C29E0, 0, D_800D6A50, 0x541);
        tbl[i] = t;
        if (t == NULL) {
            for (;;) {
            }
        }
        if (p4 && D_800E77F8) {
            func_80014A74(func_80027934);
            D_800C9584[i] = (s32)D_800E77C8;
            D_800E7790 = 0;
        }
        D_800C8C7C = -1;
        func_80013D70(p3 | 1, h[i].sub, p2, (s32)tbl[i]);
        if (p4 && D_800E77F8) {
            D_800E77C8 += D_800E7790;
            func_80014A74(NULL);
        }
    }
    if (p4 && D_800E77F8) {
        D_800C9584[i] = (s32)D_800E77C8;
        i++;
        sz = (s32)D_800E77C8 - (s32)D_800C9580;
        if (sz != 0) {
            sz *= 4;
            nb = func_800BC908(sz, 0, D_800D6A50, 0x565);
            func_800AEC34(nb, D_800C9580, sz);
            for (j = 0; j < (s32)i; j++) {
                D_800C9584[j] = (s32)(nb + ((D_800C9584[j] - (s32)D_800C9580) >> 2));
            }
            D_800C9580 = nb;
        } else {
            D_800C9580 = NULL;
        }
    }
    func_80014A68(NULL);
    return tbl;
}
