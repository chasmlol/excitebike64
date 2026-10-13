#include "ctx.h"

extern s32 *D_800CCFE0;
extern s32 D_800D1524;
extern s32 D_800D0000;
u32 func_8004F3D0(u32 key);
u32 func_8005AED8(s32 key);

void func_800718F0(void)
{
    s32 i;
    u32 off;
    u32 r;
    D_800CCFE0 = (s32 *)func_8005AED8(D_800D1524);
    if (D_800CCFE0 != 0 && D_800CCFE0[0] > 0) {
        i = 0;
        do {
            off = (u32)i << 4;
            r = func_8004F3D0(*(u32 *)((u8 *)D_800CCFE0 + off + 0x10));
            i++;
            *(u32 *)((u8 *)D_800CCFE0 + off + 0x10) = r;
        } while (i < D_800CCFE0[0]);
    }
}
