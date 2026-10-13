#include "ctx.h"

typedef struct {
    u8 pad[0x46];
    s16 f46;
    u8 p2[2];
    s16 f4A;
    u8 p3[0x6C - 0x4C];
} Seg;
typedef struct {
    u8 pad[0x628];
    s16 f628;
    u8 p1[0x6ED - 0x62A];
    u8 f6ED;
    u8 p2[0x7E0 - 0x6EE];
} Ent;
extern s32 D_800EE488;
extern Seg *D_800EE498;
extern s32 D_800EFD70;
extern s16 D_800F05CA;
extern Ent D_800E37C0[];
extern void func_800511B8(s32 v);

#define SEG(i) ((Seg *)((s32)((i) * 0x6C) + (s32)D_800EE498))

void func_8004B494(Ent *p, s32 a1)
{
    s32 h;
    s32 cur;
    s32 iv;
    s32 v;

    if (a1 == D_800EE488) {
        if (p->f628 == SEG(a1)->f4A) {
            p->f6ED--;
        }
    }
    h = p->f628;
    cur = D_800EE488;
    if (h == cur || h == SEG(cur)->f46) {
        iv = SEG(cur)->f4A;
        if (a1 == iv || a1 == SEG(iv)->f4A) {
            if ((s8)p->f6ED == 0 && (D_800EFD70 & 1) == 0) {
                v = p - D_800E37C0;
                if (v < D_800F05CA) {
                    func_800511B8(v);
                    return;
                }
            }
            p->f6ED++;
        }
    }
}
