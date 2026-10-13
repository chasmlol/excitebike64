#include "ctx.h"

typedef struct {
    u8 unk0[0x46];
    s16 next;
    s16 branch;
    s16 unk4A;
    s16 unk4C;
    u8 unk4E[0x6C - 0x4E];
} Seg;

extern s16 *D_800E80C0;
extern Seg *D_800EE498;
void func_80028790(void) {
    s32 j;
    s32 i;
    s32 n;

    i = 0;
    n = 0;
    do {
        D_800E80C0[i] = n;
        if (D_800EE498[i].branch != -1) {
            j = D_800EE498[i].branch;
            n++;
            do {
                D_800E80C0[j] = n;
                j = D_800EE498[j].next;
                n++;
            } while (D_800EE498[j].unk4C == -1);
        } else {
            n++;
        }
        i = D_800EE498[i].next;
    } while (i != 0);
}
