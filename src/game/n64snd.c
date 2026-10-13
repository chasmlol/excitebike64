#include "game.h"

s32 func_800BBBC0(s32 param_1, s32 param_2, s32 param_3, s32 param_4, u32 *param_5, s32 *param_6)
{
    s32 iVar1;
    s32 r;

    D_800D5638 = 0;
    D_800D5620 = 0;
    if ((param_2 & 3) != 3) {
        return 1;
    }
    {
        D_80100624 = param_2;
        D_80100628 = (s32)func_800AD600(param_3);
        iVar1 = 1;
        *param_6 = D_80100628;
        if (param_2 & 1) {
            iVar1 = 2;
        }
        if (param_2 & 2) {
            iVar1 = iVar1 << 1;
        }
        D_80100618 = ((((u32)(D_80100628 * param_4) / 1000 + 0x3f) >> 6) << 6);
        *param_5 = D_80100618;
        D_80100618 = D_80100618 * 2;
        D_80100614 = iVar1 * D_80100618;
        D_80100610 = (s32)func_800BC908(D_80100614 * 4, 0, D_800DB1F0, 0xd7);
        D_8010061C = (s32 *)func_800BC908(D_80100614 * 2 + 0x10, 0, D_800DB1F0, 0xd9);
        D_80100620 = D_8010061C;
        if (((u32)D_80100620 & 0xff) == 0) {
            D_8010061C = D_80100620 + 4;
        }
        if (D_8010061C == 0) {
            return 1;
        }
        {
            r = func_800BA6B0(param_1, param_2, param_3, param_4);
            if (r != 0) {
                func_800BCB04((s32)D_80100620);
                func_800BCB04(D_80100610);
                return r;
            }
            D_800D5630 = 0;
            D_800D5628 = 0;
            func_8000ED10((u8 *)D_8010061C, 0, D_80100614 << 1);
            func_800AD720((s32)D_8010061C, D_80100614);
            func_800AD720((s32)D_8010061C + D_80100614, D_80100614);
            D_800D562C = 1;
            return 0;
        }
    }
}

void func_800BBDE0(void)
{
    s32 uVar1;
    s32 *piVar3;
    u32 h;
    s32 iVar4;

    uVar1 = func_800AD5F0();
    if (uVar1 >= 0) {
        iVar4 = 0;
        if ((uVar1 & 0x40000000) == 0) {
            D_800D56BC = D_800D56BC + 1;
        }
        D_800D5628 = D_800D5628 ^ 1;
        func_800AD720(D_8010061C + D_80100614 * D_800D5628, D_80100614);
        piVar3 = D_80100610;
        do {
            if (D_800D5630_2 != 0) {
                (*D_800D5630_2)();
            }
            func_800BB09C(piVar3, D_80100618 >> 1);
            if (D_800D56B0_2 != 0) {
                (*D_800D56B0_2)(piVar3, D_80100618 >> 1);
            }
            h = D_80100618 >> 1;
            if (D_80100624 & 1) {
                piVar3 += h * 2;
            } else {
                piVar3 += h;
            }
            iVar4 = iVar4 + 1;
        } while (iVar4 < 2);
        func_800BB7B4((s32)D_80100610, (s32)(D_8010061C + D_80100614 * D_800D5628), D_80100618);
        if (D_800D5638 != 0 || D_800D5620 != 0) {
            func_8000ED10(D_8010061C + D_80100614 * D_800D5628, 0, D_80100614);
            if (D_800D5638 != 0) {
                D_800D5638 = D_800D5638 - 1;
            }
        }
        func_800AD910(D_8010061C + D_80100614 * D_800D5628, D_80100614);
        D_8010062C = D_8010062C + 1;
    }
}

s32 func_800BBFC0(s32 *arg0, s32 *arg1, s32 *arg2, s32 *arg3) {
    func_800BB47C(arg0, arg1, arg2);
    *arg1 = 3;
    *arg3 = 20;
    if (*arg2 > 22050) {
        *arg2 = 22050;
    }
    return 0;
}

void func_800BC024(void)
{
    D_800D562C = 0;
    func_800BB49C();
    func_800BCB04(D_80100610);
    func_800BCB04(D_80100620);
}

void func_800BC064(s32 a, s32 b, s32 c, s32 d) {
    func_800BAB1C(a, b, c, d);
}

void func_800BC080(s32 a)
{
    func_800BAC58(a);
}

void func_800BC09C(s32 a, s32 b, s32 c, s32 d, s32 e) {
    func_800BAD40(a, b, c, d, e);
}

void func_800BC0BC(u32 a) {
    func_800BB4EC(a);
}

void func_800BC0D8(s32 param_1, s32 *param_2)
{
    func_800BB590(param_1, param_2);
}

void func_800BC0F4(s32 a)
{
    func_800BB5CC_2(a);
}

void func_800BC110(s32 a)
{
    func_800BB9FC_2(a);
}

void func_800BC12C(s32 a)
{
    func_800BAE54_2(a);
}

void func_800BC148(s32 a)
{
    func_800BB608_2(a);
}

void func_800BC164(s32 a)
{
    func_800BAF14_2(a);
}

void func_800BC180(s32 a)
{
    func_800BAFD8_2(a);
}

void func_800BC19C(s32 a)
{
    func_800BB6CC_2(a);
}

void func_800BC1B8(s32 a)
{
    func_800BB6EC(a);
}

void func_800BC1D4(s32 a)
{
    func_800BB710_2(a);
}

void func_800BC1F0(s32 a)
{
    func_800BB780(a);
}

void func_800BC20C(s32 a)
{
    func_800BB7A4(a);
}

u32 func_800BC228(void) {
    u32 r = func_800BB7A4_2();
    return r / (D_80100618 >> 1);
}

void func_800BC264(s32 v)
{
    D_800D5630 = v;
}

void func_800BC270(s32 a)
{
    func_800BB85C(a);
}

void func_800BC28C(s32 a)
{
    func_800BB920(a);
}

void func_800BC2A8(s32 a)
{
    func_800BB978(a);
}

void func_800BC2C4(s32 a)
{
    func_800BB9C0(a);
}

void func_800BC2E0(void)
{
}

void func_800BC2E8(void)
{
}

void func_800BC2F0(void)
{
}

void func_800BC2F8(s32 a0)
{
    if (a0 != 0) {
        D_800D5620 = 1;
        func_8000ED10(D_8010061C, 0, D_80100614 << 1);
        func_800AD910(D_8010061C, D_80100614 << 1);
        D_800D5638 = 3;
    } else {
        D_800D5620 = 0;
    }
}

void func_800BC370(void)
{
    func_8000ED10(D_8010061C, 0, D_80100614 << 1);
    func_800AD910((u32)D_8010061C, D_80100614 << 1);
    D_800D5638 = 3;
}
