#include "ctx.h"

typedef struct {
    char pad[0x2C];
    s32 f2C;
} Obj;

void func_800BCB04(s32 a);

extern Obj *D_800EFD64;
extern s32 D_800EE498;

void func_8004B6CC(void)
{
    if (D_800EFD64->f2C != 2) {
        func_800BCB04(D_800EE498);
    }
}
