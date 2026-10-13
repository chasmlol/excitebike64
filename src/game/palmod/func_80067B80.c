#include "ctx.h"

extern s32 D_800EFDBC;
extern s32 D_800F1458;
extern s32 D_800EFD70;
extern s32 D_800EE490;
typedef struct { f32 x; f32 y; f32 z; u8 pad[0x60]; } Obj;
typedef struct { f32 x; f32 z; } V2;
extern Obj *D_800EE498;
extern V2 D_800F1468;
extern V2 D_800F1470;
extern volatile s32 D_800F13EC;
extern volatile s32 D_800F13F0;
extern s32 D_800C2594;
extern s32 D_800CCD00[];
extern s32 D_800CCCA8;
extern s32 D_800F14B8;
extern s32 D_800F14BC;
extern s32 D_800F145C;
extern s32 D_800F1460;
extern s32 D_800F1450;
extern s32 D_800F1454;

void func_80069158(void);

void func_80067B80(void)
{
    s32 i;
    Obj *p;
    s32 n;
    s32 wA;
    s32 hA;
    s32 wB;
    s32 hB;
    s32 k;
    s32 x, y;

    D_800F1458 = 0;
    switch (D_800EFDBC) {
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 9:
    case 10:
    case 13:
    case 14:
        break;
    default:
        if ((D_800EFD70 & 0x8000) == 0) {
            for (i = 0; i < D_800EE490; i++) {
                if (D_800EE498[i].x < D_800F1468.x || i == 0) {
                    D_800F1468.x = D_800EE498[i].x;
                }
                if (D_800EE498[i].z < D_800F1468.z || i == 0) {
                    D_800F1468.z = D_800EE498[i].z;
                }
                if (D_800EE498[i].x > D_800F1470.x || i == 0) {
                    D_800F1470.x = D_800EE498[i].x;
                }
                if (D_800EE498[i].z > D_800F1470.z || i == 0) {
                    D_800F1470.z = D_800EE498[i].z;
                }
            }
            x = D_800CCD00[(D_800C2594 - 1) * 2];
            wA = D_800F13EC;
            hA = D_800F13F0;
            wB = D_800F13EC;
            hB = D_800F13F0;
            y = D_800CCD00[(D_800C2594 - 1) * 2 + 1];
            D_800F14B8 = wB / 5;
            D_800F145C = (x * wA) / 0x140;
            D_800F14BC = (hB * 0x40) / 0xF0;
            D_800F1460 = (y * hA) / 0xF0;
            if (D_800EFDBC == 7) {
                D_800F14B8 = D_800F14B8 + D_800F14B8 / 2;
                D_800F14BC = D_800F14BC + D_800F14BC / 2;
            }
            if (D_800CCCA8 == 2) {
                D_800F1460 = D_800F1460 + 0x34;
            }
            D_800F1450 = 0x10000 / D_800F14B8;
            D_800F1454 = 0x10000 / D_800F14BC;
            func_80069158();
        }
        break;
    }
}
