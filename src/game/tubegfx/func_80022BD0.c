#include "ctx.h"

typedef struct {
    s32 f0;
    s32 f4;
    s32 f8;
    s32 fC;
    s32 pad[6];
    s32 f28;
} Blk5CC0;

extern s32 D_800C23F0;
extern u8 *D_800EFD64;
extern u8 D_800CB210[];
extern s32 D_800E7718;
extern u8 D_800D6A30[];
extern u8 D_800D6A34[];
extern u8 D_800D6A38[];
extern u8 D_800248EC[];
extern u8 D_80031D08[];
extern Blk5CC0 D_800F5CC0;
extern s32 D_800D1518, D_800D14E4, D_800D1510, D_800D1504, D_800D1508;
extern s32 D_800D1584, D_800D1588, D_800D158C, D_800D1500, D_800D14DC, D_800D14E0;
extern s32 D_800D1528, D_800E1898, D_800D07C0, D_800D06F4, D_800D07D4, D_800E7728;
extern s32 D_800C9574, D_800C9648, D_800E7824, D_800E783C, D_800E781C;
extern s32 D_800E7720, D_800E7710, D_800E771C, D_800E77F8;
extern s32 D_800E7808[3];
extern s32 *D_800C9644, *D_800C9640;
extern s32 D_800E78A0, D_800E7828, D_800E7724, D_800E7714;
extern s16 D_800E7730[];

s32 func_8005B58C(s32 key);
u32 func_8005AED8(s32 key);
void func_800BFC40(u8 *param_1, s32 param_2);
void func_800A4BEC(void);
void func_800A5A74(void);
s32 func_800BF79C(s32 a, s32 b);
s32 func_80027574(s32 a, s32 b, s32 c, s32 d);
void func_80024490(s32 a);
void func_80011490(s16 *a, s32 b, s32 c, s32 d, s32 e, s32 f, s32 g, s32 h);
void func_8002DA60(s32 a, s32 b, s32 c, s32 d);
void func_80065B10(s32 a, s32 b, s32 c, s32 d, s32 e);
s32 func_800BC908(s32 a, s32 b, s32 c, s32 d);

void func_80022BD0(void)
{
    s32 iVar1;
    s32 t;

    D_800C23F0 = 0x4000;
    iVar1 = (s32)(D_800EFD64 - D_800CB210) * -0x33333333 >> 5;
    D_800E7718 = 0;
    if (iVar1 == 0x18) {
        D_800E7718 = 1;
    }
    if (iVar1 == 0x16) {
        D_800E7718 = 1;
    }
    func_800BFC40(D_800D6A30, (s32)D_800248EC);
    func_800BFC40(D_800D6A34, (s32)D_80031D08);
    if ((*(u32 *)(D_800EFD64 + 0x50) & 2) != 0) {
        func_800A4BEC();
        func_800A5A74();
        if (D_800F5CC0.f28 != 0) {
            D_800C9574 = D_800F5CC0.f28;
        }
        D_800C9648 = 0;
        D_800E7824 = 0;
        D_800E783C = D_800F5CC0.f4;
        D_800E781C = D_800F5CC0.fC;
        D_800E7720 = func_800BF79C(D_800E7728, D_800D07C0);
        D_800E7710 = func_800BF79C(D_800E7728, D_800D06F4);
        D_800E771C = func_800BF79C(D_800E7728, D_800D07D4);
        D_800E7808[1] = 0;
        D_800E7808[2] = 0;
        D_800E77F8 = 0;
        D_800E7808[0] = D_800E781C;
        D_800C9644 = (s32 *)D_800F5CC0.f0;
        D_800E78A0 = func_80027574(D_800F5CC0.f0, (s32)&D_800E783C, 0, 0);
        D_800C9640 = (s32 *)D_800F5CC0.f8;
        D_800E7828 = func_80027574(D_800F5CC0.f8, (s32)D_800E7808, 0xC, 1);
        func_80024490((s32)D_800C9644);
    } else {
        D_800C9574 = func_8005AED8(D_800D1518);
        D_800C9648 = func_8005AED8(D_800D14E4);
        D_800E7824 = func_8005AED8(D_800D1510);
        D_800E783C = func_8005AED8(D_800D1504);
        D_800E781C = func_8005AED8(D_800D1508);
        D_800E7720 = func_8005AED8(D_800D1584);
        D_800E7710 = func_8005AED8(D_800D1588);
        D_800E771C = func_8005AED8(D_800D158C);
        D_800E77F8 = func_8005AED8(D_800D1500);
        D_800E7808[1] = D_800E77F8;
        D_800E7808[2] = 0;
        D_800E7808[0] = D_800E781C;
        D_800C9644 = (s32 *)func_8005AED8(D_800D14DC);
        D_800E78A0 = func_80027574((s32)D_800C9644, (s32)&D_800E783C, 0, 0);
        D_800C9640 = (s32 *)func_8005AED8(D_800D14E0);
        D_800E7828 = func_80027574((s32)D_800C9640, (s32)D_800E7808, 0xC, 1);
    }
    func_80011490(D_800E7730, 0, 0, 0, 0, 0, 0, 300);
    func_8002DA60((s32)D_800C9644, (s32)D_800C9640, D_800C9574, 0);
    iVar1 = func_8005B58C(D_800D1528);
    func_80065B10(D_800E7728, iVar1, 1, (s32)D_800C9644, D_800E781C);
    if ((D_800E1898 == 1) && (D_800E7718 == 0)) {
        t = *D_800C9644;
        D_800E7724 = t << 1;
        D_800E7714 = func_800BC908(t << 2, 0, (s32)D_800D6A38, 0xC9);
    } else {
        D_800E7714 = 0;
    }
}
