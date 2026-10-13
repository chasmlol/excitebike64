#include "ctx.h"

typedef struct { s32 w[4]; } Blk;

extern s32 D_800CCCA8;
extern Blk D_800C2474;
extern Blk D_800C2484;
extern Blk D_800C2494;
extern s32 D_800E1A54;
extern s16 D_800C25E8[];
extern volatile s32 D_800F13EC;
extern volatile s32 D_800F13F0;
typedef struct { u8 pad[0xA0]; } Rec;
extern Rec D_800CB210[];
extern s32 D_800E1A50;
extern s32 D_800DC0D8;
extern s32 D_800DC0DC;
extern Rec *D_800EFD64;
extern s32 D_800E1898;
extern u8 D_800DC0E0[];
extern s32 D_800C98B0;
extern s32 D_800DC144;

void func_8000BA58(void);
void func_8000ED10(u8 *a0, u8 a1, s32 a2);

void func_80000F0C(void)
{
    s32 n;

    if (D_800CCCA8 == 2) {
        D_800C2494 = D_800C2474;
        D_800E1A54 = 300;
    } else {
        D_800C2494 = D_800C2484;
        D_800E1A54 = 0xAA;
    }
    D_800C25E8[0] = D_800F13EC * 2;
    D_800C25E8[1] = D_800F13F0 * 2;
    D_800C25E8[4] = D_800F13EC * 2;
    D_800C25E8[5] = D_800F13F0 * 2;
    D_800E1A50 = D_800F13EC * 0x186 / 512;
    D_800DC0D8 = D_800F13EC * 0x28 / 512;
    D_800DC0DC = D_800F13F0 * 0x19 / 240;
    func_8000BA58();
    if ((D_800EFD64 - D_800CB210) == 8 && D_800E1898 >= 2) {
        func_8000ED10(D_800DC0E0, 0, 3);
    }
    n = D_800EFD64 - D_800CB210;
    if (n == 8 || n == 0xB) {
        n = 4;
    } else {
        n = 3;
    }
    if (D_800E1898 >= n) {
        D_800C98B0 = 0;
        D_800DC144 = 0;
    }
}
