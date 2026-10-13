#include "ctx.h"

extern s32 D_800E1E18;
extern u8 D_800E1C48[];
extern u8 D_800E1C68[];
extern u8 D_800C292C;
extern u8 D_800C292D;
extern u8 D_800C292E;

void func_8000CD00(void);
void func_800AD9C0(u8 *);
void func_800B0610(u8 *, s32 *, s32);
void func_800ADA40(u8 *);

void func_8008BE9C(void)
{
    s32 buf[2];
    s32 k;
    u8 *p;

    if (D_800E1E18 != 0) {
        func_8000CD00();
    }
    func_800AD9C0(D_800E1C48);
    func_800B0610(D_800E1C48, buf, 1);
    func_800ADA40(D_800E1C68);
    k = 0;
    D_800C292C = 0;
    D_800C292D = 0;
    p = D_800E1C68;
    do {
        if (p[3] == 0) {
            if (D_800C292C == 0) {
                D_800C292E = k;
            }
            D_800C292C = D_800C292C | (1 << k);
            D_800C292D = D_800C292D + 1;
        }
        k++;
        p += 4;
    } while (k < 4);
}
