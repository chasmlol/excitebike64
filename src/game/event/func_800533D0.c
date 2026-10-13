#include "ctx.h"

extern void func_800C2224(void);
extern void func_800C1E60(s32 *a0, s32 a1, s32 a2, s32 a3, s32 a4);
extern void func_800C1F2C(s32 *a0, char *s);
extern void func_800C0ACC(s32 *a0, s32 a1, s32 a2);
extern void func_8000EE30(s32 v, char *buf);
extern Gfx *func_800C1394(Gfx *p);
extern Gfx *func_80009754(Gfx *a0, s32 a1, s32 a2, s32 a3);

extern u32 D_800E2038;
extern s32 *D_800F0530;
extern s32 D_800F13EC;
extern s32 D_800F13F0;
extern s32 D_800E1718[];
extern f32 D_800E37C0[];
extern volatile s32 D_800CCCA8;

Gfx *func_800533D0(Gfx *gdl, s32 p1, s32 p2, s32 p3)
{
    char buf[32];
    s32 x;
    s32 y;
    f32 fa;
    s32 *tab;

    func_800C2224();
    if (D_800E2038 & 0x20) {
        func_800C1E60(D_800F0530, 0x40, 1, 2, 0x10);
        func_800C1F2C(D_800F0530, "Testing User Track");
        tab = D_800E1718;
        x = D_800F13EC * 0x12;
        if (x < 0) {
            x = x + 0x1ff;
        }
        func_800C0ACC(D_800F0530, D_800E1718[0] + (x >> 9), tab[1] + (D_800F13F0 * 0x11) / 0xf0);
    }
    fa = D_800E37C0[0x108 + p3 * 0x1f8];
    if (0.0f < fa) {
        func_8000EE30((s32)(fa * 3600.0f / 1612.9f), buf);
    } else {
        func_8000EE30((s32)(-fa * 3600.0f / 1612.9f), buf);
    }
    func_800C1E60(D_800F0530, 0x40, 3, 2, 0x12);
    func_800C1F2C(D_800F0530, buf);
    x = 0x1ba;
    if (D_800CCCA8 == 2) {
        x = 0x228;
    }
    y = 0xd6;
    if (D_800CCCA8 == 2) {
        y = 0x180;
    }
    func_800C0ACC(D_800F0530, x, y);
    gdl = func_800C1394(gdl);
    gdl = func_80009754(gdl, p1, p2, p3);
    return gdl;
}
