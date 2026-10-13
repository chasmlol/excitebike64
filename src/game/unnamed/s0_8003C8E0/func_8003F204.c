#include "ctx.h"

typedef struct {
    char pad[0x7A8];
    u8 cnt;
    char rest[0x37];
} Slot;

typedef struct {
    s32 f0;
    s32 f4;
} Cfg;

typedef struct {
    char pad[0x444];
    Cfg *cfg;
} Blk;

extern Slot D_800E37C0[];
extern u32 D_800E76D0;
extern Blk D_800C9A40;

void func_8003F204(void)
{
    s32 i = 0;

    do {
        if ((s8)D_800E37C0[i].cnt == 0) {
            i++;
            continue;
        }
        if ((D_800E76D0 & 1) != 0) {
            u8 v = D_800E37C0[i].cnt + 1;
            D_800E37C0[i].cnt = v;
            if ((s8)v >= D_800C9A40.cfg->f4) {
                D_800E37C0[i].cnt = 0;
            }
        }
        i++;
    } while (i < 8);
}
