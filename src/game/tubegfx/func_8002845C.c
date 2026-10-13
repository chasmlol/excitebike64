#include "ctx.h"
extern s32 D_800E77D8[];
extern s32 D_800E77A0[];
extern s32 D_800E15E0[];
extern s32 D_800E8508;
extern s32 D_800E8958;
void func_8002E920(s32, s32, s32);
void func_80010994(s32, s32, s32);

typedef struct {
    u8 pad0[0x4F8];
    s32 f4F8;
    u8 rest[0x7E0 - 0x4F8 - 4];
} Rec37;
extern Rec37 D_800E37C0[];

typedef struct {
    s32 a, b, c;
} Trip;
extern Trip D_800E7840[];

void func_8002845C(s32 param_1, s32 param_2, s32 param_3, s32 param_4)
{
    if (D_800E77D8[param_1] == 0 && D_800E37C0[param_1].f4F8 >= 0) {
        D_800E8508 = 0;
        func_8002E920(param_2, param_3, param_4);
        if (D_800E8508 != 0) D_800E77A0[param_1] = D_800E8958;
        else D_800E77A0[param_1] = D_800E15E0[param_1];
        D_800E77D8[param_1] = 0x78;
        D_800E7840[param_1].a = param_2 >> 4;
        D_800E7840[param_1].b = param_3 >> 4;
        D_800E7840[param_1].c = param_4 >> 4;
        func_80010994(6, param_1, 0xff);
    }
}
