#include "ctx.h"

typedef struct {
    u8 pad0[0x2C];
    s32 state;
    u8 pad1[0x20];
    u32 flags;
} Obj;

extern Obj *D_800EFD64;
extern s32 D_800D14E8;
extern s32 D_800F0BD8;
extern s32 D_800F0CA8;

s32 func_8005B58C(s32 key);
u32 func_8005AED8(s32 key);

void func_80064CA8(void)
{
    if (D_800EFD64->state != 2 && (D_800EFD64->flags & 2) == 0) {
        D_800F0BD8 = func_8005B58C(D_800D14E8);
        D_800F0CA8 = func_8005AED8(D_800D14E8);
    }
}
