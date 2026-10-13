#include "ctx.h"

typedef struct {
    s32 a;
    s32 b;
} Pair;

typedef struct {
    u32 flags;          /* 0x000 */
    u8 pad4[0x18];
    s32 sel[4];         /* 0x01C */
    u8 pad2C[0x10];
    s32 count;          /* 0x03C */
    u8 pad40[0xC];
    s32 f4C;            /* 0x04C */
    s32 f50;
    s32 f54;            /* 0x054 */
    u8 pad58[0x2A8 - 0x58];
    Pair f2A8[4];       /* 0x2A8 */
    s8 f2C8[4];         /* 0x2C8 */
    u8 pad2CC[0x14];
    s16 f2E0[4];        /* 0x2E0 */
    u8 pad2E8[0x3C0 - 0x2E8];
    s32 f3C0;           /* 0x3C0 */
    s32 f3C4;           /* 0x3C4 */
    u8 pad3C8[0x408 - 0x3C8];
    s16 f408[2];        /* 0x408 */
    u8 f40C[4];         /* 0x40C */
} State;

typedef struct {
    s32 pad0;
    s32 f4;
} Sub;

typedef struct {
    u8 pad0[0xC];
    Sub *fC;
    u8 pad10[0x18];
    s32 f28;
} Obj;

extern State D_800EFD70;
extern u8 D_800CD090[];
extern u8 D_800C292C;
extern u32 D_800E30B0;
extern u32 D_800F0128;
extern s32 D_800C9570;
extern u8 D_800CD8D0[];

void func_8001102C(s32 x);
void func_8007AF38(s32 a, s32 b, s32 c);
void func_80017338(void *p);

void func_80075F3C(Obj *p1, s32 p2, u32 *p3)
{
    State *g;
    State *h;
    State *k;
    State *m;
    s32 i;
    s32 ok;
    s32 idx;
    s32 t;
    u32 mask;

    g = &D_800EFD70;
    for (i = 0; i < 4; i++) {
        if (D_800EFD70.f2E0[i] != 0) {
            D_800EFD70.f2E0[i]--;
        }
        D_800EFD70.f2A8[i].a = (D_800EFD70.f2A8[i].a + 0x23) & 0xFFFF;
    }

    ok = 1;
    h = &D_800EFD70;
    for (i = 0; i < h->count; i++) {
        if (!(D_800EFD70.flags & (0x400 << i))) {
            ok = 0;
        }
    }
    func_80017338(D_800CD090);

    k = &D_800EFD70;
    if (k->count == 1) {
        if (*p3 & 0x780) func_8001102C(0);
        if (*p3 & 0x200) k->f3C0 = (k->f3C0 + 5) % 6;
        if (*p3 & 0x400) k->f3C0 = (k->f3C0 + 1) % 6;
        if (k->f4C != 8) {
            if (*p3 & 0x80) k->f3C4 = (k->f3C4 + 5) % 6;
            if (*p3 & 0x100) k->f3C4 = (k->f3C4 + 1) % 6;
        }
    }

    if ((*p3 & 0x50) && ok) {
        func_8001102C(1);
        if (D_800F0128 < 8) {
            if (D_800F0128 >= 2) {
                func_8007AF38(0, 1, 1);
            } else {
                func_8007AF38(p1->fC->f4, 1, 1);
            }
        } else {
            func_8007AF38(p1->fC->f4, 1, 1);
        }
        return;
    }
    if (*p3 & 0x20) {
        if (!(D_800EFD70.flags & 0x400)) {
            if (p1->f28 != 0) {
                if (p1->f28 == (s32)D_800CD8D0 && D_800C9570 != 0) {
                    D_800EFD70.flags |= 0x2000000;
                }
                func_8007AF38(p1->f28, 0, 0);
                func_8001102C(2);
            }
            return;
        }
    }
    m = &D_800EFD70;
    if (m->f54 != 3) return;
    for (i = 0; i < D_800EFD70.count; i++) {
        if (!((D_800C292C >> (idx = D_800EFD70.f2C8[i])) & 1)) continue;
        if (D_800EFD70.f40C[i] == 0 && !(D_800EFD70.flags & (0x400 << i))) {
            if (p3[idx] & 4) {
                t = D_800EFD70.sel[i];
                D_800EFD70.sel[i] = t - 1;
                if (t - 1 < 0) D_800EFD70.sel[i] = t + 5;
                if (D_800EFD70.count == 1) D_800EFD70.f3C0 = (D_800EFD70.f3C0 + 5) % 6;
                func_8001102C(0);
            }
            if (p3[D_800EFD70.f2C8[i]] & 8) {
                t = D_800EFD70.sel[i];
                D_800EFD70.sel[i] = t + 1;
                if (t + 1 >= 6) D_800EFD70.sel[i] = t - 5;
                if (D_800EFD70.count == 1) D_800EFD70.f3C0 = (D_800EFD70.f3C0 + 1) % 6;
                func_8001102C(0);
            }
        }
        if (D_800EFD70.count >= 3 || D_800EFD70.f40C[i] == 0) {
            if (!(D_800EFD70.flags & (0x400 << i)) && (p3[D_800EFD70.f2C8[i]] & 0x50)) {
                D_800EFD70.f2E0[i] = 0x28 - D_800EFD70.f2E0[i];
                D_800EFD70.flags |= 0x400 << i;
                func_8001102C(1);
            } else if ((D_800E30B0 & 0x20000) && D_800EFD70.count == 1) {
                D_800EFD70.f40C[i] = 1;
                D_800EFD70.f408[i] = 0x3D;
            }
        } else {
            if (D_800EFD70.f40C[i] == 1) {
                if (--D_800EFD70.f408[i] <= 0) {
                    D_800EFD70.f40C[i] = 2;
                } else if (!(D_800E30B0 & 0x20000)) {
                    D_800EFD70.f40C[i] = 3;
                    D_800EFD70.f408[i] = 0x3E - D_800EFD70.f408[i];
                }
            } else if (D_800EFD70.f40C[i] == 2) {
                if (!(D_800E30B0 & 0x20000)) {
                    D_800EFD70.f40C[i] = 3;
                    D_800EFD70.f408[i] = 0x3D;
                }
            } else if (D_800EFD70.f40C[i] == 3) {
                if (D_800E30B0 & 0x20000) {
                    D_800EFD70.f40C[i] = 1;
                    D_800EFD70.f408[i] = 0x3E - D_800EFD70.f408[i];
                }
                if (--D_800EFD70.f408[i] <= 0) {
                    D_800EFD70.f40C[i] = 0;
                }
            }
        }
        mask = 0x400 << i;
        if ((D_800EFD70.flags & mask) && (p3[D_800EFD70.f2C8[i]] & 0x20)) {
            func_8001102C(2);
            D_800EFD70.f2E0[i] = 0x28 - D_800EFD70.f2E0[i];
            D_800EFD70.flags &= ~mask;
        }
    }
}
