#include "ctx.h"

typedef struct {
    u8 pad0[0x48];
    f32 f48, f4c, f50, f54, f58, f5c;
    u8 pad1[0x7A5 - 0x60];
    u8 flag;
    u8 pad2[0x7E0 - 0x7A6];
} Ent;

typedef struct {
    u8 pad0[0xC];
    s16 c0, c1, c2;
    u8 pad1[0x24 - 0x12];
    s16 d0, d1, d2;
    s16 st;
} Item;

typedef struct {
    s32 f0;
    s32 n;
    s32 flag;
    s32 f0c, f10, f14, f18, f1c, f20, f24, f28;
    Item *list;
} Elem;

extern Ent D_800E37C0[8];
extern s32 D_800CCCE0;
extern Elem *D_800F1444;
extern s32 *D_800E7834;
extern s32 D_800C9640;
void func_80010994(s32 k, s32 n, s32 vol);
u32 func_8000EFE8(void);

void func_800674A4(void)
{
    s32 h, i, j, u, sel, x, y, z, dx, dy;
    Ent *s;
    Elem *e;
    Item *it;

    for (i = 0; i < 8; i++) {
        s = &D_800E37C0[i];
        if (s->flag != 0) continue;
        for (j = 0; j < D_800CCCE0; j++) {
            e = &D_800F1444[j];
            if (e->flag < 0 && (h = e->f0 - 4, !(D_800E7834[(h - D_800C9640) >> 5] & 6))) {
                x = (s32)(s->f48 * 1000.0f) >> 4;
                y = (s32)(s->f50 * 1000.0f) >> 4;
                u = (x - e->f0c) * e->f24 + (y - e->f14) * e->f28;
                u >>= 14;
                z = (s32)(s->f4c * 1000.0f) >> 4;
                if (e->f10 - e->f0c < 30 && e->f18 - e->f14 < 30) {
                    sel = (u32)(u + 30) < 61;
                } else {
                    sel = (u32)(u + 100) < 201;
                }
                if (sel) {
                    u = e->f10 - e->f0c;
                    if (u < 0) u = -u;
                    dy = e->f18 - e->f14;
                    if (dy < 0) dy = -dy;
                    if (dy < u) {
                        if (x < e->f10 && x < e->f0c) continue;
                        if (e->f10 < x && e->f0c < x) continue;
                    } else {
                        if (y < e->f18 && y < e->f14) continue;
                        if (e->f18 < y && e->f14 < y) continue;
                    }
                    if (z < e->f1c - 50) continue;
                    if (e->f20 + 50 < z) continue;
                    func_80010994(4, i, 0xFF);
                    e->flag = 0;
                    for (u = 0; u < e->n; u++) {
                        it = &e->list[u];
                        if (u & 1) {
                            it->st = 3;
                            it->c0 = it[-1].c0;
                            it->c1 = it[-1].c1;
                            it->c2 = it[-1].c2;
                            it->d0 = it[-1].d0;
                            it->d1 = it[-1].d1;
                            it->d2 = it[-1].d2;
                        } else {
                            it->c0 = (s32)((s->f54 * 1000.0f) / 60.0f);
                            it->c1 = (s32)((s->f58 * 1000.0f) / 60.0f);
                            it->c2 = (s32)((s->f5c * 1000.0f) / 60.0f);
                            it->d0 = (func_8000EFE8() - 0x80) * 4;
                            it->d1 = (func_8000EFE8() - 0x80) * 5;
                            it->d2 = (func_8000EFE8() - 0x80) * 2;
                            it->st = 3;
                        }
                    }
                }
            }
        }
    }
}
