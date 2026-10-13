#include "ctx.h"

extern void func_800B31C0(void *dst, void *src);
extern void func_8000ED10(u8 *a0, u8 a1, s32 a2);
extern u8 *D_800CEA40;
extern u8 D_800F4F08[];

void func_8009F1F8(s32 param_1) {
    s32 p = param_1;
    *(s32 *)(D_800CEA40 + 0x1754) = 0;
    func_800B31C0(D_800CEA40 + 0x1760, D_800F4F08 + (p << 6));
    func_8000ED10(*(u8 **)((s32)D_800CEA40 + (p << 2) + 0x174C), 0, 0xF00);
}
