#include "ctx.h"

typedef struct {
    u8 pad0[0x3D3];
    s8 f3D3;
    s8 hist[8];
    u8 pad1[0x3FC - 0x3DC];
    u8 idx;
    u8 cnt;
    u8 pad2[0x408 - 0x3FE];
    s32 f408;
    u8 pad3[0x5B0 - 0x40C];
    u32 btn;
    u32 prev;
    u8 pad4[0x7E0 - 0x5B8];
} Ent;

typedef struct {
    u8 pad[0x1C];
    s32 mask;
    u8 rest[0x1C];
} Move;

typedef struct {
    u8 pad[0x1C];
    s32 sh[1];
} Cfg;

extern Move D_800C9A40[];
extern u32 D_800C2280;
extern s32 D_800E76D0;
extern Ent D_800E37C0[];
extern Cfg D_800EFD70;
extern u8 *D_800F2110[];
extern u8 *D_800F2128[];

void func_8006F8D4(Ent *e)
{
    u32 w;
    s32 x, y, dir;
    u8 flag;
    s32 k;
    s32 i;
    u8 **tab;
    u8 *p;

    if ((e->prev & 0x100) && !(e->btn & 0x100)) {
        e->f3D3 = -1;
        e->hist[e->idx] = 1;
        return;
    }
    w = e->btn;
    y = w >> 28;
    if (w & 2) y = -y;
    x = ((s32)w >> 24) & 0xF;
    if (w & 8) x = -x;
    if (abs(x) < 8 && abs(y) < 8) {
        dir = 2;
    } else {
        dir = 3;
        if (y < x) dir = 5;
        if (-y < x) dir++;
    }
    flag = 0;
    if (e->hist[e->idx] == dir && e->cnt < 0xF0) {
        e->cnt++;
    } else {
        flag = 1;
        e->idx = (e->idx - 1) & 7;
        e->hist[e->idx] = dir;
        e->cnt = 0;
    }
    if ((e->btn & 0x100) && (!(e->prev & 0x100) || flag)) {
        if (e->btn & 0x10000) {
            p = D_800F2128[dir - 2];
        } else {
            p = D_800F2110[dir - 2];
        }
        do {
            for (i = 0; i < 7; i++) {
                if (p[i + 1] == 0 &&
                    (((D_800C9A40[p[0]].mask >> D_800EFD70.sh[e - D_800E37C0]) & 1) ||
                     (D_800C2280 & 0x200))) {
                    e->f408 = D_800E76D0;
                    e->f3D3 = p[0];
                }
                if ((p + i)[1] != e->hist[(e->idx + i) & 7]) break;
            }
            if ((p + i)[1] < e->hist[(e->idx + i) & 7]) return;
            p += 8;
        } while (i != 0);
    }
}
