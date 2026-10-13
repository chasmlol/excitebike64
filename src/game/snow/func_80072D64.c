#include "ctx.h"

typedef struct {
    s32 f0;
    s32 id;
    s32 f8;
    s32 idx;
    s32 n;
    u16 *tab;
} Ent;

extern s32 D_800F21B0;
extern Ent D_800F21B8[];
extern u8 *D_800C9800;
extern u32 *D_800E7834;
void func_80067818(s32 x);

void func_80072D64(s32 id)
{
    s32 i;
    s32 k;

    for (i = 0; i < D_800F21B0; i++) {
        if (D_800F21B8[i].id == id) {
            break;
        }
    }
    if (i != D_800F21B0) {
        for (k = 0; k < D_800F21B8[i].n; k++) {
            D_800C9800[D_800F21B8[i].tab[k]] |= 0x20;
        }
        k = D_800F21B8[i].idx;
        if (D_800E7834[k] & 1) {
            func_80067818(k);
        } else {
            D_800E7834[k] |= 8;
        }
    }
}
