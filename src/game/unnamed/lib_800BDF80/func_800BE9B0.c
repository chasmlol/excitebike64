#include "ctx.h"

extern s32 D_80100698;
extern u8 *D_801006A4;

s32 func_8000EDA0(u8 *param_1, u8 *param_2);

u8 *func_800BE9B0(u8 *param_1)
{
    u32 i;
    u8 *p;

    i = 0;
    p = D_801006A4;
    if (D_80100698 != 0) {
        do {
            if (func_8000EDA0(p, param_1) == 0) {
                break;
            }
            p += 0x20;
        } while (++i < D_80100698);
    }
    if (i == D_80100698) {
        return NULL;
    }
    return p;
}
