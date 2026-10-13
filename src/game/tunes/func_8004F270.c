#include "ctx.h"

typedef struct {
    u8 pad0[0xA8];
    s32 a8;
    s32 ac;
    s32 b0;
    s32 b4;
    s32 b8;
    s32 bc;
    u8 pad1[0x14];
    s32 d4;
    s32 d8;
} Obj;

typedef struct {
    u8 pad0[8];
    s32 e8;
    s32 ec;
    s32 e10;
} Ent;

extern s32 D_800C2358;
extern Obj *D_800EFD24;
extern Ent *D_800EFD4C;
extern s32 D_800EECC8;
extern s32 D_800EECD4;
extern s32 D_800EECDC;

int func_8004F270(s32 param_1, s32 param_2, s32 param_3)
{
    Ent *iVar3;

    if (D_800C2358 != 0) {
        return D_800C2358;
    }
    if (D_800EFD24->a8 != -1 && param_2 <= D_800EECC8) {
        return 1;
    }
    iVar3 = D_800EFD4C + param_1;
    D_800EFD24->d4 = 0;
    D_800EFD24->d8 = 0;
    D_800EFD24->a8 = param_1;
    D_800EFD24->ac = 0;
    D_800EFD24->b8 = iVar3->e8 + D_800EECD4;
    D_800EECC8 = param_2;
    D_800EFD24->bc = iVar3->ec;
    D_800EECDC = param_3;
    D_800EFD24->b4 = iVar3->e10;
    return 0;
}
