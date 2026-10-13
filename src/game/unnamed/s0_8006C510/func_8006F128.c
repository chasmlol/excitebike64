#include "ctx.h"

typedef struct { u8 pad[0x410]; s32 cur; } S70;
typedef struct { u8 pad[0x2BF]; u8 bits; u8 cnt; } S19;
typedef struct { u8 pad[8]; u16 flags; u8 pad2[6]; } Ent;

extern S70 D_800EFD70;
extern S19 D_800F1900;
extern s32 D_800F0180;
extern u8 D_800F1BC1;
extern u32 D_800CA9C4;
extern Ent D_800CD370[];
extern Ent D_800CD430[];

static inline s32 active(s32 n)
{
    if (D_800F0180 < D_800F1900.cnt) return 1;
    if (D_800F1900.cnt == D_800F0180) {
        if (n == 0) return 1;
        return (D_800F1900.bits >> (n - 1)) & 1;
    }
    return 0;
}

s32 func_8006F128(void)
{
    s32 saved = D_800EFD70.cur;
    s32 i, j, act, idx, k;
    s32 ret;
    u32 g;
    s32 hv;
    u32 bit;
    Ent *e;

    for (i = 0; i < 4; i++) {
        k = i * 5;
        switch (i) {
        case 0:
        case 1:
            D_800EFD70.cur = 1;
            break;
        case 2:
            D_800EFD70.cur = 2;
            break;
        case 3:
            D_800EFD70.cur = 3;
            break;
        }
        act = 0;
        if (D_800EFD70.cur < D_800F1900.cnt) {
            act = 1;
        } else if (D_800F1900.cnt == D_800EFD70.cur) {
            if (i == 0 || ((D_800F1900.bits >> (i - 1)) & 1)) act = 1;
        }
        j = 0;
        idx = k;
        do {
            if (act) {
                bit = 1 << idx;
                g = D_800CA9C4 | bit;
                e = (Ent *)(idx << 4);
                e = (Ent *)((u32)e + (u32)D_800CD430);
                hv = e->flags & ~1;
                D_800CA9C4 = g;
                e->flags = hv;
            } else {
                bit = 1 << idx;
                bit = ~bit;
                e = (Ent *)(idx << 4);
                e = (Ent *)((u32)e + (u32)D_800CD430);
                g = D_800CA9C4 & bit;
                hv = e->flags | 1;
                D_800CA9C4 = g;
                e->flags = hv;
            }
            j++;
            idx++;
        } while (j < 5);
    }

    ret = 0;
    if (D_800F1BC1 == 7) {
        ret = (D_800CD370[2].flags & 1) << 1;
        D_800CD370[2].flags &= ~1;
    } else {
        D_800CD370[2].flags |= 1;
    }

    D_800F0180 = 1;
    if (!(D_800F0180 < D_800F1900.cnt || (D_800F1900.cnt == D_800F0180 && (D_800F1900.bits & 4)))) D_800CD370[3].flags |= 1;

    D_800F0180 = 2;
    if ((D_800F0180 < D_800F1900.cnt || (D_800F1900.cnt == D_800F0180 && (D_800F1900.bits & 8)))) {
        if (D_800CD370[4].flags & 1) ret = 4;
        D_800CD370[4].flags &= ~1;
    } else {
        D_800CD370[4].flags |= 1;
    }

    D_800F0180 = 3;
    if ((D_800F0180 < D_800F1900.cnt || (D_800F1900.cnt == D_800F0180 && (D_800F1900.bits & 16)))) {
        if (D_800CD370[5].flags & 1) ret = 5;
        D_800CD370[5].flags &= ~1;
    } else {
        D_800CD370[5].flags |= 1;
    }

    D_800F0180 = saved;
    return ret;
}
