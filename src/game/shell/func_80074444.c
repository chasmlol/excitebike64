typedef unsigned char u8;
typedef short s16;
typedef int s32;
typedef unsigned int u32;

typedef struct {
    u8 p000[0x17C];
    s32 f17C;
    u8 p180[0x2E8 - 0x180];
    s32 f2E8;
    u8 p2EC[0x3EC - 0x2EC];
    s32 f3EC;
    u8 p3F0[0x44A - 0x3F0];
    s16 f44A;
    u8 p44C[0x450 - 0x44C];
    s32 f450;
    u8 p454[0x750 - 0x454];
    s32 f750;
} GameState;

extern GameState D_800EFD70;
extern void func_8001102C(s32 p);
extern void func_8007AF38(s32 a0, s32 a1, s32 a2, s32 a3);

void func_80074444(s32 param_1, s32 param_2, u32 *param_3, s32 param_4)
{
    s32 t;
    s32 t1;
    s16 w;
    u32 flags;
    s32 c1;
    s32 c2;
    s32 c3;
    s32 c4;
    s32 c5;

    c1 = D_800EFD70.f17C + 1;
    D_800EFD70.f17C = c1;
    if (c1 == 0x3c) {
        D_800EFD70.f17C = 0;
    }
    c2 = D_800EFD70.f3EC + 1;
    D_800EFD70.f3EC = c2;
    if (c2 == 0x40) {
        D_800EFD70.f3EC = 0;
    }
    if (D_800EFD70.f750 != 0) {
        c3 = D_800EFD70.f750 - 1;
        D_800EFD70.f750 = c3;
        if (c3 == 0) {
            param_4 = 0xb;
            if (D_800EFD70.f2E8 != 0xb) {
                if (D_800EFD70.f2E8 == 0xd) {
                    D_800EFD70.f2E8 = 0xb;
                    D_800EFD70.f750 = 0x3b;
                }
            } else {
                D_800EFD70.f2E8 = 0xc;
            }
        }
    }
    flags = *param_3;
    if ((flags & 0x20) != 0) {
        func_8007AF38(*(s32 *)(param_1 + 0x28), 0, 0, param_4);
        func_8001102C(2);
        return;
    }
    t1 = D_800EFD70.f450;
    if ((flags & 1) != 0) {
        if (t1 % 5 != 0) {
            D_800EFD70.f450 = D_800EFD70.f450 - 1;
        }
    }
    if ((*param_3 & 2) != 0) {
        c4 = D_800EFD70.f450;
        if (c4 % 5 < 4 && c4 + 1 < D_800EFD70.f44A) {
            D_800EFD70.f450 = c4 + 1;
        }
    }
    if ((*param_3 & 4) != 0) {
        c5 = D_800EFD70.f450;
        if (c5 / 5 + (c5 >> 31) != (c5 >> 31)) {
            D_800EFD70.f450 = c5 - 5;
        }
    }
    if ((*param_3 & 8) != 0) {
        t = D_800EFD70.f450;
        w = D_800EFD70.f44A;
        if (t + 5 < w) {
            D_800EFD70.f450 = t + 5;
        } else if (t / 5 < (s16)(w / 5)) {
            D_800EFD70.f450 = w - 1;
        }
    }
    if (t1 != D_800EFD70.f450) {
        D_800EFD70.f2E8 = 0xd;
        D_800EFD70.f750 = 6;
        func_8001102C(0);
    }
}
