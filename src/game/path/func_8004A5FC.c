#include "ctx.h"

extern s32 D_800EFDBC;
extern u8 *D_800EFD64;
extern u32 *D_800F5CE4;
extern u32 *D_800EE49C;
extern s32 D_800EE488;
extern s32 D_800EE494;
extern s32 D_800D10F0;
extern s32 D_800D0D50;
extern s32 D_800C2280;
extern s32 D_800D14F8;
extern s32 D_800C29E0;
extern u8 *D_800CA930;
extern u8 D_800CADE4[];

void func_800C0258(s32 a0);
s32 func_8005B58C(s32 key);
void func_800BFEA4(u32 *p);
void func_80048F58(void);
void func_800492D0(u32 *p);
void func_8004A334(void);
void func_80049D24(void);
void func_800BCB04(s32 a);
u32 func_800BF79C(u32 *p, s32 b);

void func_8004A5FC(u32 *param_1)
{
    s32 iVar1;
    u32 uVar2;

    if (D_800EFDBC == 6) {
        func_80048F58();
        goto end;
    }
    if ((*(u32 *)(D_800EFD64 + 0x50) & 2) != 0) {
        D_800EE49C = D_800F5CE4;
        func_800492D0(D_800F5CE4);
        func_8004A334();
        func_80049D24();
        D_800EE488 = 1;
        D_800EE494 = 1;
        func_800BCB04((s32)D_800EE49C);
        goto end;
    }
    if (*(s32 *)(D_800EFD64 + 0x2c) != 2) {
        if (D_800EFDBC == 9 && (D_800CADE4[*(s16 *)(D_800CA930 + 0x64) * 40 + 0xC] & 2)) {
            iVar1 = D_800D10F0;
            goto L134;
        }
        if (*(volatile s32 *)&D_800EFDBC == 7 && (D_800C2280 & 8)) {
            iVar1 = D_800D0D50;
            goto L134;
        }
        iVar1 = func_8005B58C(D_800D14F8);
L134:
        func_800C0258(D_800C29E0);
        uVar2 = func_800BF79C(param_1, iVar1);
        D_800EE49C = (u32 *)(uVar2 & 0xfffffffc);
        func_800492D0(D_800EE49C);
        func_800BFEA4((u32 *)(param_1[1] + iVar1 * 4));
        func_800C0258(0);
    }
end:
    return;
}
