#include "ctx.h"

void func_800B2550(s32 a);
void func_800B2AB0(void *p);
void func_800B3020(s32 a);
void func_800B2CC0(void *p);
void func_8000ED10(u8 *a0, u8 a1, s32 a2);
void func_800AD990(void);

extern u8 D_800D1FF0[];

void func_8000E9C0(void)
{
    func_800B2550(10);
    func_800B2AB0(D_800D1FF0);
    func_800B3020(0);
    func_800B2CC0((void *)0x80200000);
    func_800B3020(0);
    func_800B2CC0((void *)0x80200000);
    func_8000ED10((u8 *)0x80200000, 0xFF, 0x25800);
    func_800AD990();
    __asm__ volatile("break 0x407");
    while (1) {}
}
