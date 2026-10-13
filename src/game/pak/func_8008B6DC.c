#include "ctx.h"

extern s32 *D_800F49F8;
extern s32 D_800D13A0;
extern s32 D_800D1488;
extern s32 D_800D14A8;
extern s32 D_800D1418;
extern s32 D_800D1434;
extern s32 D_800CE164;

void func_800BFEA4(u32 *p);
void func_800C204C(s32 a);
void func_800BCB04(s32 a);
void func_8008C744(void);
void func_8008BC30(void);

void func_8008B6DC(void)
{
    func_800BFEA4((u32 *)(D_800F49F8[1] + D_800D13A0 * 4));
    func_800BFEA4((u32 *)(D_800F49F8[1] + D_800D1488 * 4));
    func_800BFEA4((u32 *)(D_800F49F8[1] + D_800D14A8 * 4));
    func_800BFEA4((u32 *)(D_800F49F8[1] + D_800D1418 * 4));
    func_800BFEA4((u32 *)(D_800F49F8[1] + D_800D1434 * 4));
    func_800C204C(D_800CE164);
    func_800BCB04(D_800CE164);
    func_8008C744();
    func_8008BC30();
}
