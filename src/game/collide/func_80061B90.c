#include "ctx.h"

extern f32 D_800F0B18;
extern u8 D_800F0616;
extern u8 D_800E3808[];

void func_80061D54(u8 *p);
s32 func_80060538(u8 *a, u8 *b, u8 *c, u8 *d, u8 *e, u8 *f);
void func_800607BC(u8 *a, u8 *b, u8 *c, u8 *d, u8 *e, u8 *f);
s32 func_800610B0(u8 *a, u8 *b);

s32 func_80061B90(u8 *p1, u8 *p2)
{
    u8 *s0;
    u8 *v;
    u8 *tb;
    s32 a, b;

    if (p2 == p1) {
        return 0;
    }
    v = *(u8 **)(p1 + 0x7C);
    D_800F0B18 = 0.0f;
    s0 = v;
    if (s0 != 0 && *s0 != 0) {
        *s0 = 0;
        s0 = *(u8 **)(s0 + 0x24);
        while (s0 != 0) {
            func_80061D54(s0);
            s0 = *(u8 **)(s0 + 0x20);
        }
    }
    if (*(u8 **)(p1 + 0x180) == *(u8 **)(p2 + 0x180) &&
        *(*(u8 **)(p1 + 0x180) + 0x44) != 2) {
        if (func_80060538(*(u8 **)(p1 + 0x7C), *(u8 **)(p2 + 0x7C), p1 + 0x58, p2 + 0x58, p1, p2) & 0xff) {
            return 0;
        }
        *(*(u8 **)(p1 + 0x180) + 0x44) = 2;
    }
    func_800607BC(*(u8 **)(p1 + 0x7C), *(u8 **)(p2 + 0x7C), p1 + 0x58, p2 + 0x58, p1, p2);
    if (0.0f < D_800F0B18) {
        if (func_800610B0(p1, p2) != 0) {
            p2[0x17D] = p2[0x17D] | 1;
            if ((p1[0x17C] == 4 && p2[0x17C] == 0) || (p2[0x17C] == 4 && p1[0x17C] == 0)) {
                if (p1[0x17C] == 4) {
                    p1 = p2;
                }
            } else {
                return 1;
            }
            tb = (u8 *)D_800E3808;
            if (p1 == tb) {
                D_800F0616 = 0;
            } else if (p1 == tb + 0x7E0) {
                D_800F0616 = 1;
            } else if (p1 == tb + 0xFC0) {
                D_800F0616 = 2;
            } else if (p1 == tb + 0x17A0) {
                D_800F0616 = 3;
            }
            return 1;
        }
        p1[0x17E] = 0xf;
        p2[0x17E] = 0xf;
    }
    return 0;
}
