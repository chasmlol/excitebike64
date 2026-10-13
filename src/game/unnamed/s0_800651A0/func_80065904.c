#include "ctx.h"

extern s32 D_800CCCA0;
extern s32 D_800CCCA8;
extern s32 D_800CCCAC[];
extern s32 D_800CCCB8[];
extern s32 D_800F13EC;
extern s32 D_800F13F0;
extern u8 D_800F0CB0[];

void func_800B3020(s32);
void func_800AD910(s32, s32);
void func_800B2AB0(void *);
void func_800B2B00(s32);

void func_80065904(s32 p)
{
    func_800B3020(1);
    D_800CCCA0 = 3;
    func_800AD910(0, 0x800000);
    func_800B2AB0(D_800F0CB0 + p * 0x50);
    func_800B2B00(0xA2);
    *(volatile s32 *)&D_800F13EC = D_800CCCAC[p];
    *(volatile s32 *)&D_800F13F0 = D_800CCCB8[p];
    *(volatile s32 *)&D_800CCCA8 = p;
    func_800B3020(1);
}
