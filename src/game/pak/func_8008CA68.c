#include "ctx.h"

extern u8 D_800C292C;
extern u8 D_800C292D;
extern u8 D_800C292E;
extern u8 D_800E1C68[];
extern u8 D_800E1C48[];

void func_800B4830(u8 *a0, u8 *a1);

void func_8008CA68(u8 *param_1)
{
    s32 i;
    u8 *q;

    func_8000CD00();
    i = 0;
    D_800C292C = 0;
    D_800C292D = 0;
    do {
        q = D_800E1C68 + i * 4;
        if (q[3] == 0) {
            if (D_800C292C == 0) {
                D_800C292E = i;
            }
            D_800C292C = D_800C292C | (1 << i);
            D_800C292D = D_800C292D + 1;
        }
        i++;
    } while (i < 4);
    func_800B4830(D_800E1C48, param_1);
}
