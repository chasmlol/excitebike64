#include "ctx.h"

typedef struct {
    u8 unk0[0x44C];
    f32 unk44C;
    f32 unk450;
    u8 unk454[0x7E0 - 0x454];
} Racer;

extern Racer D_800E37C0[];
extern u8 D_800F0C90[];

void func_80064A8C(s32 arg0, s32 count) {
    s32 i;


    for (i = 0; i < count; i++) {
        if (count == 1) {
            D_800E37C0[D_800F0C90[i]].unk450 = 0.0f;
        } else {
            D_800E37C0[D_800F0C90[i]].unk450 = (i - (count - 1) * 0.5f) / ((count - 1) * 5.0f);
        }
        if (D_800E37C0[D_800F0C90[i]].unk450 == 0.1f) {
            D_800E37C0[D_800F0C90[i]].unk450 += 0.01f;
        } else {
            D_800E37C0[D_800F0C90[i]].unk450 -= 0.01f;
        }
        D_800E37C0[D_800F0C90[i]].unk44C = 1.03f - D_800E37C0[D_800F0C90[i]].unk450;
    }
}
