#include "ctx.h"

extern u32 D_80018288;
extern u8 D_80018430;
extern u8 D_80018432;

s32 boot_func_80001BF0(u32 addr);
void boot_func_80001BB8(u32 addr, u32 val);
void boot_func_80001C2C(s32 ms);
u32 boot_func_80002E04(void);
void boot_func_80002E10(u32 v);
void boot_func_8000223C(s32 n);
void boot_func_800023EC(void);

void boot_func_800025B8(void)
{
    s32 bVar1;
    s32 iVar2;
    s32 iVar3;
    u32 uVar4;

    do {
        bVar1 = 1;
        while ((iVar2 = boot_func_80001BF0(0xB1FFFFF0)) == 0) {
            boot_func_80001C2C(1000);
        }
        if (iVar2 == 2) {
            boot_func_80001BB8(0xB1FFFFFC, 0);
            boot_func_80001C2C(1000);
            goto LAB_800027b8;
        }
        boot_func_80001BB8(0xB1FFFFFC, 0x101);
        while ((iVar3 = boot_func_80001BF0(0xB1FFFFF0)) == iVar2) {
            boot_func_80001C2C(500);
        }
        iVar3 = boot_func_80001BF0(0xB1FFFFF0);
        if (iVar3 == 2) {
            boot_func_80001BB8(0xB1FFFFFC, 0);
            goto LAB_800027b8;
        }
        switch (iVar2) {
        case 0x10:
        default:
            break;
        case 0x11:
            bVar1 = 0;
            break;
        case 0x12:
            D_80018430 = 0;
            D_80018432 = 0;
            D_80018288 = D_80018288 & 0xfffffffe;
            uVar4 = boot_func_80002E04();
            boot_func_80002E10(uVar4 & 0xfffffffe);
            break;
        case 0x13:
            D_80018432 = 0;
            D_80018430 = 0;
            break;
        case 0x14:
            D_80018432 = 0xff;
            break;
        case 0x15:
            D_80018432 = 0;
            break;
        case 0x16:
            boot_func_80001BB8(0xB1FE0000, (u32)D_80018432 << 24 | (u32)D_80018430 << 16);
            break;
        case 0x17:
            boot_func_8000223C(2);
            break;
        case 0x18:
            boot_func_8000223C(1);
            break;
        case 0x19:
            boot_func_800023EC();
            break;
        }
        boot_func_80001BB8(0xB1FFFFFC, 0);
        while ((iVar2 = boot_func_80001BF0(0xB1FFFFF0)) == 1) {
            boot_func_80001C2C(500);
        }
LAB_800027b8:
        if (!bVar1 && D_80018432 != 0) {
            boot_func_80001BB8(0xB1FFFFF0, 0);
            return;
        }
    } while (1);
}
