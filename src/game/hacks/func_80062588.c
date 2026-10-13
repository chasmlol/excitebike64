#include "ctx.h"

typedef struct {
    u8 pad0[0x28];
    s32 count;
    s32 id;
    u8 pad30[2];
    s16 t[3];
    u8 v[8];
} Ent;

typedef struct {
    u8 pad0[0x3D0];
    s8 f3D0;
    u8 pad3D1[0x6E8 - 0x3D1];
    u8 f6E8;
    u8 pad6E9[0x7A5 - 0x6E9];
    u8 f7A5;
    u8 pad7A6[0x7E0 - 0x7A6];
} Player;

extern u32 func_8000EFE8(void);
extern Ent D_800CCB90[];
extern s32 D_800E15E0[];
extern Player D_800E37C0[];
extern s32 D_800CCB20;

void func_80062588(void)
{
    s32 j;
    s32 i;
    Ent *e;

    for (j = 0; j < 2; j++) {
        e = &D_800CCB90[j];
        for (i = 0; i < e->count; i++) {
            if (++e->t[i] >= 0x100) {
                e->t[i] = 0;
                e->v[i] = func_8000EFE8() & 3;
            }
        }
    }

    for (i = 0; i < 8; i++) {
        if (D_800E37C0[i].f7A5 == 0 && D_800E37C0[i].f6E8 == 0 && D_800E37C0[i].f3D0 != -1 && D_800E37C0[i].f3D0 < 16) {
            if (D_800E15E0[i] == D_800CCB90[0].id || D_800E15E0[i] == D_800CCB90[1].id) {
                D_800CCB20 = 300;
            }
        }
    }

    if (D_800CCB20 > 0) {
        D_800CCB20--;
    }
}
