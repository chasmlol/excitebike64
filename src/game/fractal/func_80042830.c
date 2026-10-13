#include "ctx.h"

typedef struct {
    s32 x;
    s32 z;
    s16 vis;
    s16 pad;
    u8 pad2[0x310];
    s32 tex;
    u8 rest[0xE4];
} Tile;

extern Tile *D_800EE138;
extern s32 D_800EE12C;
extern s32 D_800EE130;
extern s32 D_800EE134;
void func_80043864(Tile *t, s32 x, s32 z);
void func_80042A5C(Tile *t, s32 a);

void func_80042830(s32 px, s32 pz)
{
    s32 i, j, x, z, idx, tx, tz, q, wa, xa;

    px /= 2000;
    pz /= 2000;
    D_800EE130 = (px + 0x40) >> 7;
    D_800EE134 = (pz + 0x40) >> 7;
    for (i = -2; i < 2; i++) {
        for (j = -2; j < 2; j++) {
            z = D_800EE134 + i;
            q = (z & 3) * 4;
            x = D_800EE130 + j;
            idx = q + (x & 3);
            if (D_800EE138[idx].x != x || D_800EE138[idx].z != z) {
                D_800EE138[idx].tex = D_800EE12C + ((idx / 4) * 0x10000 + (idx % 4) * 0x80);
                func_80043864(&D_800EE138[idx], x, z);
            }
            D_800EE138[idx].vis = 1;
            tx = D_800EE138[idx].x * 0x80;
            tz = D_800EE138[idx].z << 7;
            wa = tx + 0x7f;
            xa = tz + 0x7f;
            if (px + 0x7f < tx) D_800EE138[idx].vis = 0;
            if (pz + 0x7f < tz) D_800EE138[idx].vis = 0;
            if (wa < px - 0x80) D_800EE138[idx].vis = 0;
            if (xa < pz - 0x80) D_800EE138[idx].vis = 0;
        }
    }
    for (i = 0; i < 16; i++) {
        if (D_800EE138[i].vis != 0) func_80042A5C(&D_800EE138[i], -1);
    }
}
