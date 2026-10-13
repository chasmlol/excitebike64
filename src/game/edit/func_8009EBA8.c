#include "ctx.h"

extern u8 *D_800CEA40;

typedef struct {
    s16 h0;
    s16 h1;
    u8 rest[60];
} Row;

extern Row D_800CE280[];

void func_8009EBA8(void)
{
    u8 *p = D_800CEA40;
    if (*(u32 *)(p + 0x174) & 1) {
        s32 idx = *(s32 *)(p + 0x16FC);
        *(s32 *)(p + 0x16F8) = D_800CE280[idx + 0x12].h1 * 2 - 1;
        return;
    }
    *(s32 *)(p + 0x16F8) = 1;
}
