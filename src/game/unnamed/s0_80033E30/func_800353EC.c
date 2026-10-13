#include "ctx.h"

typedef struct { s32 f0, f4, f8, fc; } G1718;
typedef struct Rec { u8 pad[0xA0]; } Rec;
extern G1718 D_800E1718;
extern s32 D_800C98B0;
extern u16 D_800E8002;
extern s32 D_800E8EC0[];
extern s32 D_800E8ED0[];
extern s32 D_800E8EB0[];
extern s32 D_800E7978[];
extern Rec *D_800EFD64;
extern s32 D_800F13EC;
extern s32 D_800F13F0;
extern Rec D_800CB210[];
s32 func_80033AC8(s32 a, s32 b, s32 c, s32 *d, s32 *e, s32 *f, s32 *g, s32 h, s32 i);

s32 func_800353EC(s32 *outx, s32 *outy, s32 p3, s32 p4)
{
    s32 x, y, z, w;
    s32 t;
    s32 m;

    if (D_800C98B0 == 1) {
        t = (D_800E1718.f8 << 7) / D_800E8EC0[p4];
        x = (s16)(D_800E8002 - 0x7AEC);
        m = x * t;
        t = (D_800E1718.fc << 2) / D_800E8ED0[p4];
        x = m / 8192;
        y = D_800E7978[p4];
        y = 45 - y * t / 1024;
        y = 0xB9 - D_800E8EB0[p4];
    } else {
        if (D_800EFD64 - D_800CB210 == 1) {
            if (func_80033AC8(300, 0x14, 50, &x, &y, &z, &w, p3, p4)) {
                return 1;
            }
        } else {
            if (func_80033AC8(0, 0x14, -300, &x, &y, &z, &w, p3, p4)) {
                return 1;
            }
        }
    }
    if (x < -50 || y < -50 || D_800F13EC < x || D_800F13F0 + 50 < y) {
        return 1;
    }
    *outx = x;
    *outy = y;
    return 0;
}
