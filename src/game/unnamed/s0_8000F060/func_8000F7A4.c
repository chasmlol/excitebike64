#include "ctx.h"

typedef struct Ent {
    s32 x, y, z;
    u8 pad0C[0x5AA - 0xC];
    s16 f5AA;
    u8 pad5AC[0x6E8 - 0x5AC];
    u8 f6E8;
    u8 pad6E9[0x7A5 - 0x6E9];
    u8 f7A5;
    u8 pad7A6[0x7E0 - 0x7A6];
} Ent;

typedef struct Slot {
    u8 pad00[0x28];
    s32 vol;
    s32 best;
    s32 dist;
    s32 delta;
    u8 pad38[0x54 - 0x38];
    s32 taken;
    u8 pad58[0x64 - 0x58];
} Slot;

typedef struct Pt {
    s32 x, y, z;
} Pt;

typedef struct Trk {
    u8 pad[0xA0];
} Trk;

extern void func_8004C4E4(void);
extern void func_8000FB44(s32);

extern s32 D_800C2B64;
extern s32 D_800C2B58;
extern Pt D_800E8090[];
extern s32 D_800C2594;
extern Trk *D_800EFD64;
extern Trk D_800CB210[];
extern s32 D_800E1898;
extern s32 D_800C2B48[];
extern Ent D_800E37C0[];
extern Slot D_800E32A8[];

void func_8000F7A4(void)
{
    Pt *pts;
    Pt *pt;
    Slot *sl;
    Ent *e;
    Ent *base;
    s32 i, bi, dist, n, k, j, best, bv;
    f32 z, t, kk, d, bd, ex, ey, ez, v;

    func_8004C4E4();
    if (D_800C2B64 == 0) {
        return;
    }
    if (D_800C2B58 < 0) {
        D_800C2B58 = 0;
    }
    if (D_800C2B58 >= 0x100) {
        D_800C2B58 = 0xFF;
    }
    base = D_800E37C0;
    pts = D_800E8090;
    sl = D_800E32A8;
    e = base;
loop:
    if (e->f7A5 != 0) {
        sl->delta = 0;
        sl->best = 0;
        sl->dist = 0;
        sl->vol = 0;
    } else {
        bd = 0.0f;
        n = 0;
        if (n < D_800C2594) {
            do {
            do {
                pt = &pts[n];
                d = pt->x;
                d -= (f32)e->x;
                d *= d;
                d += ((f32)pt->y - (f32)e->y) * ((f32)pt->y - (f32)e->y);
                d += ((f32)pt->z - (f32)e->z) * ((f32)pt->z - (f32)e->z);
                if (d < bd || n == 0) {
                    bd = d;
                    bi = n;
                }
                n++;
            } while (n < D_800C2594);
            } while (0);
        }
        bd = __builtin_sqrtf(bd);
        if (D_800EFD64 - D_800CB210 == 20 && ((s32)e ^ (s32)base) == 0 && base->f5AA == 4) {
            bd = 10000.0f;
        }
        v = bd;
        dist = (s32)v;
        if (v < 5000.0f) {
            v = 5000.0f;
        }
        if (v > 30000.0f) {
            v = 30000.0f;
        }
        v = 30000.0f - v;
        v = v * 255.0f / 25000.0f;
        if (v < 0.0f) v = 0.0f;
        if (v > 0.0f) k = 1; else k = 2;
        if (v < 25000.0f) k = 1; else k = 2;
        if (v > 25000.0f) k = 1; else k = 2;
        if (v > 255.0f) v = 255.0f;


        
        sl->vol = (s32)v;
        if (sl->best == bi) {
            sl->delta = sl->dist - dist;
        } else {
            sl->delta = 0;
        }
        sl->best = bi;
        sl->dist = dist;
        sl->taken = 0;
    }
    e++;
    sl++;
    if ((s32)e < (s32)(base + 8)) {
        goto loop;
    }

    n = D_800C2B48[D_800E1898 - 1];
    for (k = 0; k < n; k++) {
        bv = -1;
        best = 0;
        for (i = 0; i < 8; i++) {
            if (D_800E37C0[i].f7A5 == 0 && D_800E32A8[i].taken == 0) {
                if (bv < D_800E32A8[i].vol ||
                    (D_800E37C0[i].f6E8 == 0 && D_800E37C0[best].f6E8 != 0)) {
                    bv = D_800E32A8[i].vol;
                    best = i;
                }
            }
        }
        if (bv == -1) {
            break;
        }
        D_800E32A8[best].taken = 1;
    }
    for (i = 0; i < 8; i++) {
        if (D_800E32A8[i].taken == 0) {
            D_800E32A8[i].vol = 0;
        }
        func_8000FB44(i);
    }
}
