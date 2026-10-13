#include "ctx.h"

extern u8 D_800E1C48[];

void func_8000CD00(void);
void func_800AD910(u8 *a0, s32 a1);
void func_800AC460(u8 *a0, u8 a1, u8 *a2);
void func_800AC240(u8 *a0, u8 a1, u8 *a2);
void func_800AD7E0(s32 a0, s32 a1);

void func_8000D1A8(s32 p1, u8 *p2, s32 p3)
{
    u8 loc[8];
    s32 i;
    s32 j;
    s32 k;
    s32 base;
    s32 w;

    func_8000CD00();
    func_800AD910(p2, p3 << 3);
    i = 0;
    if (p3 > 0) {
        do {
            w = p1 + i;
            base = i * 8;
            do {
                func_800AC460(D_800E1C48, (u8)w, p2 + base);
                for (j = 0; j < 8; j++) {
                    loc[j] = p2[base + j] ^ 0xAA;
                }
                func_800AC240(D_800E1C48, (u8)w, loc);
                func_800AD7E0(0, 0x400000);
                for (k = 0; k < 8; k++) {
                    if (loc[k] != p2[base + k]) {
                        break;
                    }
                }
            } while (k != 8);
            i++;
        } while (i < p3);
    }
}
