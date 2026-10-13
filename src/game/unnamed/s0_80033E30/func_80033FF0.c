#include "ctx.h"

extern u8 *D_800DC144;
extern s32 D_800C2594;
extern s32 D_800C2364;
extern char *D_800EFD64;
extern char D_800CC070[];
extern s32 D_800E8EB0[];
extern s32 D_800E7978[];
extern s32 D_800E8EA0[];
extern s32 D_800E8000[];
extern s32 D_800E8ED0[];
extern s32 D_800E8EC0[];

void func_80033FF0(void)
{
    s32 i;
    s32 t;
    s32 v;
    s32 n;

    if (D_800DC144 != 0) {
        t = D_800DC144[9];
        if (t == 0) {
            t = 0x100;
        }
        i = 0;
        if (i < D_800C2594) {
        n = D_800C2594;
        do {
            D_800E8EC0[i] = 0x80;
            D_800E8ED0[i] = 0x68;
            v = D_800E8000[i] & 0x1FFF;
            if (D_800C2364 != 0) {
                v = 0x2000 - v;
            }
            D_800E8EA0[i] = (v / 32 - t) & 0xFF;
            v = D_800E7978[i];
            v = v / 32;
            v += 0x80;
            if (D_800EFD64 == D_800CC070) {
                v -= 0x50;
            }
            D_800E8EB0[i] = v;
            i++;
        } while (i < n);
        }
    }
}
