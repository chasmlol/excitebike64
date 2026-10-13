#include "ctx.h"

extern u8 *D_800CEA40;
typedef struct { s16 f0; s16 f2; u8 rest[0x3C]; } Tab64;
extern Tab64 D_800CE280[];
extern s16 D_800F55B8[];
extern u8 D_800C2C60[];

#define CEA_IDX (*(s32 *)(D_800CEA40 + 0x16FC))
#define LIM ((s32)D_800CE280[CEA_IDX + 0x12].f2 << 1)

int func_8009D224(int param_1, u16 *param_2)
{
    int t0;
    int q;
    int quo;
    int res;
    int n;
    u8 *tb;
    u8 *tb2;
    s16 sv;
    u16 uv;

    if (param_1 - 1 > 0 && param_1 - 1 < LIM) {
        s16 *gb = D_800F55B8;
        s32 k = param_1 - 2;
        t0 = gb[CEA_IDX * 15 + k] + 0xFA;
    } else {
        t0 = 0xFA;
    }
    tb = D_800C2C60;
    if (param_1 - 1 > 0) {
        q = (((u32)param_2[param_1 - 2] + 0x1555) >> 3) & 0x1ffe;
    } else {
        q = 0x2AA;
    }
    sv = *(s16 *)(tb + q);
    uv = *(u16 *)(tb + q);
    if (sv == 0) {
        uv = 1;
    }
    tb2 = D_800C2C60;
    if (param_1 - 1 > 0) {
        q = ((((param_2[param_1 - 2] + 0x1555) >> 4) - 0x400) & 0xfff) << 1;
    } else {
        q = 0x1AAA;
    }
    quo = (*(s16 *)(tb2 + q) * 0xFA) / (s16)uv;
    if (param_1 - 1 > 0 && param_1 - 1 < LIM) {
        s16 *gb = D_800F55B8;
        s32 k = param_1 - 2;
        quo += gb[CEA_IDX * 15 + k];
    }
    res = quo;
    if (t0 < res) {
        res = t0;
    }
    return res;
}
