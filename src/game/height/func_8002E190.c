#include "ctx.h"

extern s32 D_800C9800;
extern s32 D_800E8D60;
extern s32 D_800E8510;
extern s32 D_800E8E6C;
extern s32 *D_800E8500;
extern s32 *D_800E8948;
void func_800BCB04(s32 a);

void func_8002E190(void)
{
    u32 i;
    if (D_800C9800 != 0) {
        func_800BCB04(D_800C9800);
        D_800C9800 = 0;
    }
    if (D_800E8D60 != 0) {
        func_800BCB04(D_800E8D60);
        D_800E8D60 = 0;
    }
    if (D_800E8510 != 0) {
        func_800BCB04(D_800E8510);
        D_800E8510 = 0;
    }
    if (D_800E8E6C != 0) {
        func_800BCB04(D_800E8E6C);
        D_800E8E6C = 0;
    }
    if (D_800E8500 != 0) {
        i = 0;
        if (*D_800E8948 != 0) {
            do {
                if (D_800E8500[i] != 0) {
                    func_800BCB04(D_800E8500[i]);
                }
                i++;
            } while (i < (u32)*D_800E8948);
        }
        func_800BCB04(D_800E8500);
        D_800E8500 = 0;
    }
}
