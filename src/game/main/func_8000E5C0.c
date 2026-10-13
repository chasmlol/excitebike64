#include "ctx.h"

extern s32 D_80000318;
extern u8 D_800731D0[];
extern u8 D_800A42C0[];
extern u8 D_800A9C20[];
extern u8 D_800D6014[];
extern u8 D_80105680[];

void func_8000ECB8(s32 code) __attribute__((noreturn));
void func_800B3020(s32 x);
void func_8000ED10(u8 *a0, u8 a1, s32 a2);
s32 func_800BD698(s32 p0, s32 p1, s32 a, s32 b);
void func_800BCCF0(u32 a, u32 b, u32 c, s32 d);

void func_8000E5C0(void)
{
    u32 top;
    u32 s0;
    u32 s2;
    u32 s3;
    u32 s1;
    u32 t;

    top = ((u32)D_80000318 | 0x80000000) - 0x100;
    s0 = (u32)D_800731D0;
    s2 = (u32)D_800A42C0;
    s3 = (u32)D_800A9C20;
    if (s0 & 0xF) {
        func_8000ECB8(0x33);
    }
    if (s2 & 0xF) {
        func_8000ECB8(0x34);
    }
    if (s3 & 0xF) {
        func_8000ECB8(0x35);
    }
    s1 = (u32)D_80105680;
    s1 = (s1 + 0xFF) & ~0xFF;
    func_800B3020(1);
    func_8000ED10((u8 *)s1, 0, top - s1);
    func_800BD698(0, 0, s0, top - s0);
    t = 0x30;
    func_800BCCF0(s0 + 0x30, s2 - t - s0, (u32)D_800D6014, 0x4FA);
    t = 0x10;
    func_800BCCF0(s2 + 0x10, s3 - t - s2, (u32)D_800D6014, 0x4FB);
    func_800BCCF0(s3 + 0x10, s1 - t - s3, (u32)D_800D6014, 0x4FC);
}
