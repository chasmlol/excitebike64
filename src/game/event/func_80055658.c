#include "ctx.h"

typedef struct {
    s32 x;
    s32 y;
    s32 w;
    s32 h;
} Box;

typedef struct {
    u8 pad[0x664];
    s16 num;
    s16 ang;
    u8 pad2[0x7E0 - 0x668];
} Player;

extern Player D_800E37C0[];
extern Box D_800E1718[][4];
extern s32 D_800CCCA8;
extern s32 D_800C2598;
extern s32 D_800E1898;

extern s32 func_800C2224(void);
extern void func_800C1D54(s32 a, s32 b, s32 c, s32 d);
extern void *func_800C1394(void *a);
extern void func_800C1F2C(void *a, char *b);
extern void func_800C0ACC(void *a, s32 b, s32 c);

void *func_80055658(void *param_1, s32 param_2, void *param_3)
{
    Player *base = D_800E37C0;
    Player *pl = &base[param_2];
    s32 a = pl->ang;
    char buf[3];

    if (a > 0x180) {
        a = 0x300 - a;
    }
    param_1 = func_800C1394(param_1);
    func_800C2224();
    buf[0] = ((u8 *)&pl->num)[1] + '1';
    buf[1] = (pl->num < 3) ? ((u8 *)&pl->num)[1] + ':' : '=';
    buf[2] = 0;
    func_800C1F2C(param_3, buf);
    if (D_800CCCA8 == 2) {
        func_800C1D54(a << 9, a << 9, 0, 0);
        func_800C0ACC(param_3, D_800E1718[D_800E1898 - 1][D_800C2598].x + D_800E1718[D_800E1898 - 1][D_800C2598].w - 0x3C,
                      D_800E1718[D_800E1898 - 1][D_800C2598].y + 0x14);
        param_1 = func_800C1394(param_1);
        func_800C1D54(0x10000, 0x20000, 0, 0);
    } else {
        func_800C1D54(a << 8, a << 8, 0, 0);
        func_800C0ACC(param_3, D_800E1718[D_800E1898 - 1][D_800C2598].x + D_800E1718[D_800E1898 - 1][D_800C2598].w - 0x28,
                      D_800E1718[D_800E1898 - 1][D_800C2598].y + 0xA);
        param_1 = func_800C1394(param_1);
        func_800C1D54(0x10000, 0x10000, 0, 0);
    }
    func_800C2224();
    return param_1;
}
