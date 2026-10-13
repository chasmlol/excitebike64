#include "ctx.h"

void func_800BCB04(s32);
void func_800A5840(void);
void func_80028278(void);
void func_8002E190(void);

extern s32 *D_800C9644;
extern s32 *D_800C9640;
extern s32 *D_800E78A0;
extern s32 *D_800E7828;
extern s32 *D_800EFD64;
extern s32 D_800E7714;

void func_80024708(void)
{
    u32 i;

    i = 0;
    if (*D_800C9644 != 0) {
        do {
            func_800BCB04(D_800E78A0[i]);
            i = i + 1;
        } while (i < (u32)*D_800C9644);
    }
    i = 0;
    if (*D_800C9640 != 0) {
        do {
            func_800BCB04(D_800E7828[i]);
            i = i + 1;
        } while (i < (u32)*D_800C9640);
    }
    func_800BCB04((s32)D_800E78A0);
    func_800BCB04((s32)D_800E7828);
    if ((D_800EFD64[20] & 2) != 0) {
        func_800A5840();
    }
    func_8002E190();
    func_80028278();
    if (D_800E7714 != 0) {
        func_800BCB04(D_800E7714);
    }
}
