#include "ctx.h"

typedef struct { u8 pad[0xC]; u16 f; u8 rest[0x32]; } Blk;
typedef struct { s32 f0; u32 flags; s16 n; s16 pad; u32 mask; } Rec;
typedef struct { s32 f0; s32 f4; Rec *recs; } Grp;

extern Blk D_800CE280[];
extern Grp D_800CEB9C;
void func_80090398(void *p);

void func_8009B820(void)
{
    s32 i;
    s32 k;
    s32 t0;
    s32 v;
    Blk *p;
    s32 w;
    s32 r;

    for (k = 0; k < 0x1D; k++) {
        p = &D_800CE280[k];
        v = p->f;
        if (v & 2) {
            w = v | 0x10;
            p->f = w;
            if (w & 0x100) {
                r = w | 0x20;
            } else {
                r = w & ~0x20;
            }
        } else {
            r = v & ~0x10;
            r &= ~0x20;
        }
        p->f = r;
    }
    for (i = 0; i < 4; i++) {
        switch (i) {
        case 0:
            t0 = 0;
            break;
        case 1:
            t0 = 2;
            break;
        case 2:
            t0 = 6;
            break;
        case 3:
            t0 = 0x12;
            break;
        }
        D_800CEB9C.recs[i].mask = 0;
        for (k = 0; k < D_800CEB9C.recs[i].n; k++) {
            if (D_800CE280[t0 + k].f & 0x30) {
                D_800CEB9C.recs[i].mask |= 1 << k;
            }
        }
        if (D_800CEB9C.recs[i].mask == 0) {
            D_800CEB9C.recs[i].flags &= ~0x10;
        } else {
            D_800CEB9C.recs[i].flags |= 0x10;
        }
    }
    func_80090398(&D_800CEB9C);
}
