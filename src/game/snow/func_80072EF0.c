#include "ctx.h"

typedef struct {
    u8 pad[0x10];
    s32 n;
    u16 *idx;
} Ent;

extern s32 D_800F21B0;
extern Ent D_800F21B8[];
extern u8 *D_800C9800;

void func_80072EF0(void) {
    s32 i;
    s32 j;
    for (i = 0; i < D_800F21B0; i++) {
        Ent *e = &D_800F21B8[i];
        for (j = 0; j < e->n; j++) {
            u8 *b = D_800C9800 + e->idx[j];
            *b &= 0xDF;
        }
    }
}
