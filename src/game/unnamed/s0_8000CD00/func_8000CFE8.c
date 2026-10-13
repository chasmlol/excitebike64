#include "ctx.h"

extern u8 D_800E1C48[];
extern u8 D_800E1E50[];
extern u8 D_800C292C[];
extern u8 D_800E1C68[];
extern s32 D_800E1E18;

void func_800B05E0();
void func_800B1280();
s32 func_800ADC40();
void func_8008BC30();

int func_8000CFE8(void)
{
    int r;

    func_800B05E0(D_800E1C48, D_800E1E50, 4);
    func_800B1280(5, D_800E1C48, 1);
    r = func_800ADC40(D_800E1C48, D_800C292C, D_800E1C68);
    if (r != 0) {
        return r;
    }
    func_8008BC30();
    D_800E1E18 = 0;
    return 0;
}
