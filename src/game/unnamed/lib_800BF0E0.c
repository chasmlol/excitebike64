#include "game.h"

/* Numeric addresses (0x800D1DF0 / 0x800D1DF8) give one 'lw' macro each (lui+lw in the same reg);
   'r + t - t' references t in the then-arm so flow marks it global (combine drops the use later),
   global-alloc then gives r its $v0 preference and the flag gets $v1. */
s32 func_800BF0E0(void)
{
    s32 t = *(s32 *)0x800D1DF0;
    s32 r = 0;
    if (t != 0) {
        r = *(s32 *)0x800D1DF8 + t - t;
    }
    return r;
}

u32 *func_800BF100(void)
{
    u32 uVar1;
    u32 v;
    u32 t1;
    u32 t2;
    u32 t3;
    u32 t4;
    u32 old;
    u32 m;
    u8 *pb4;
    u8 *q;
    u32 *pb;
    register u32 *p0 asm("$16");

    func_800AD554();
    if (D_800D5720 != 0) {
        __asm__ __volatile__("lui %0, 0x8010
	addiu %0, %0, 0x9B4" : "=r"(pb4));
        q = pb4 + 0x10;
        *(s32 *)0x800D5720 = 0;
        *pb4 = 0;
        *(u32 *)0x801009BC = 0xB0000000;
        *(u8 *)0x801009B9 = 0;
        *(s32 *)0x801009C0 = 0;
        func_800B29A0(q, 0x60);
        while ((*(volatile u32 *)0xA4600010 & 3) != 0) {
        }
        t1 = *(volatile u32 *)0xA4600014;
        t2 = *(volatile u32 *)0xA460001C;
        t3 = *(volatile u32 *)0xA4600020;
        t4 = *(volatile u32 *)0xA4600018;
        __asm__ __volatile__("lui %0, 0x8010
	addiu %0, %0, 0x9BC" : "=r"(pb));
        v = *pb;
        __asm__ __volatile__("");
        *(volatile u32 *)0xA4600014 = 0xFF;
        *(volatile u32 *)0xA460001C = 0;
        *(volatile u32 *)0xA4600020 = 3;
        *(volatile u32 *)0xA4600018 = 0xFF;
        uVar1 = *(u32 *)(v | 0xA0000000);
        *(volatile u32 *)0xA4600014 = t1;
        *(volatile u32 *)0xA460001C = t2;
        *(volatile u32 *)0xA4600020 = t3;
        *(volatile u32 *)0xA4600018 = t4;
        *(u8 *)0x801009B5 = (u8)uVar1;
        *(u8 *)0x801009B6 = (uVar1 >> 16) & 0xF;
        *(u8 *)0x801009B7 = (uVar1 >> 20) & 0xF;
        *(u8 *)0x801009B8 = (u8)(uVar1 >> 8);
        __asm__ ("" : "+r"(pb));
        p0 = pb - 3;
        m = func_800AC890();
        __asm__ __volatile__("lw %0, 0x800D1E0C" : "=r"(old));
        *(u32 *)0x800D1E0C = (u32)p0;
        pb[-3] = old;
        func_800AC900(m);
    }
    func_800AD5C0();
    {
        u32 *r = &D_801009B0;
        __asm__ __volatile__("" : "+r"(r));
        return r;
    }
}
