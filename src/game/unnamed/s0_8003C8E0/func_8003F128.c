#include "ctx.h"

typedef struct {
    u8 pad0[0x410];
    u16 cnt;
    u8 pad1[6];
    s8 dir;
    u8 pad2[0x778 - 0x419];
    u32 t;
    u16 val;
    u8 pad3[0x7E0 - 0x77E];
} Ent;

extern Ent D_800E37C0[];
extern u32 D_800E1B50[];
extern u32 D_800E76D0;

void func_8003F128(void)
{
    s32 i;
    u32 t;
    s32 v;

    i = 0;
    t = D_800E76D0;
    do {
        if (D_800E37C0[i].dir > 0) {
            if ((s16)D_800E37C0[i].cnt < 0x1E) {
                D_800E37C0[i].cnt = D_800E37C0[i].cnt + 1;
            }
        } else {
            if ((s16)D_800E37C0[i].cnt > 0) {
                D_800E37C0[i].cnt = D_800E37C0[i].cnt - 2;
            }
        }
        if (D_800E1B50[i] & 0x2000) {
            D_800E37C0[i].t = t;
        }
        if ((s32)(t - D_800E37C0[i].t) < 0x3C) {
            v = D_800E37C0[i].val + 0x10;
            D_800E37C0[i].val = v;
            if ((s16)v > 0x100) {
                D_800E37C0[i].val = 0x100;
            }
        } else {
            v = D_800E37C0[i].val - 0x10;
            D_800E37C0[i].val = v;
            if ((s16)v < 0) {
                D_800E37C0[i].val = 0;
            }
        }
        i++;
    } while (i < 8);
}
