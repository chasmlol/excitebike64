#include "ctx.h"

typedef struct {
    u8 pad0[0x50];
    u8 dc[0x39C];
    s32 cnt;
    u8 pad1[0x86];
    u16 pos;
} Blk;

extern Blk D_800EFD70;
extern u8 D_800C292C;

void func_8001102C(s32 param_1);
void func_8007AF38(s32 a, s32 b, s32 c);

void func_80074298(s32 param_1, s32 param_2, u32 *param_3, s8 *param_4)
{
    s32 iv4;
    s32 uv5;
    s32 uVar6;
    s32 cv;
    u32 flags;
    u16 u2;
    u16 uP;

    uVar6 = (s16)D_800EFD70.pos;
    D_800EFD70.cnt = D_800EFD70.cnt + 1;
    if (D_800EFD70.cnt == 0x78) {
        D_800EFD70.cnt = 0;
    }
    flags = *param_3;
    if ((flags & 0x20) != 0) {
        func_8007AF38(*(s32 *)(param_1 + 0x28), 0, 0);
        iv4 = 2;
        goto CALLPT;
    }
    if ((flags & 4) != 0) {
        do {
            uP = D_800EFD70.pos;
            u2 = uP - 1;
            D_800EFD70.pos = u2;
            if ((s32)((u32)u2 << 16) < 0) {
                D_800EFD70.pos = uP + 3;
            }
        } while (((D_800C292C >> (s16)D_800EFD70.pos) & 1) == 0);
        goto LAB_800743d8;
    }
    if ((flags & 8) != 0) {
        do {
            uP = D_800EFD70.pos;
            u2 = uP + 1;
            D_800EFD70.pos = u2;
            if ((s16)u2 > 3) {
                D_800EFD70.pos = uP - 3;
            }
        } while (((D_800C292C >> (s16)D_800EFD70.pos) & 1) == 0);
        goto LAB_800743d8;
    }
    if ((flags & 0x10) == 0) {
        goto LAB_800743d8;
    }
    cv = D_800EFD70.dc[uVar6];
    D_800EFD70.dc[uVar6] = cv + 1;
    if ((s8)(cv + 1) > 3) {
        D_800EFD70.dc[(s16)D_800EFD70.pos] = D_800EFD70.dc[(s16)D_800EFD70.pos] - 4;
    }
    iv4 = 1;
CALLPT:
    func_8001102C(iv4);
LAB_800743d8:
    {
        u8 bits;
        bits = D_800C292C;
        if (((bits >> (s16)D_800EFD70.pos) & 1) == 0) {
            do {
                uv5 = (D_800EFD70.pos + 1) & 3;
                D_800EFD70.pos = uv5;
            } while (uv5 != uVar6 && ((bits >> uv5) & 1) == 0);
        }
    }
    if (uVar6 != (s16)D_800EFD70.pos) {
        func_8001102C(0);
    }
    return;
}
