#include "ctx.h"

extern s32 D_800F5A60[];
extern s32 D_800F5A68[];
extern s32 D_800F5A40;
extern s32 D_800F5A54;
extern s32 D_800F5A58;
extern s32 D_800CEC04;
extern s32 D_800CA920;

void func_800A1A68(void);
void func_800BCB04(s32);

void func_800A19BC(void)
{
    s32 i;

    func_800A1A68();
    for (i = 0; i < 2; i++) {
        func_800BCB04(D_800F5A60[i]);
        func_800BCB04(D_800F5A68[i]);
    }
    func_800BCB04(D_800F5A40);
    func_800BCB04(D_800F5A54);
    func_800BCB04(D_800F5A58);
    D_800CEC04 = 0;
    D_800CA920 = 6;
}
