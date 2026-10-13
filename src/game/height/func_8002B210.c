#include "ctx.h"

typedef struct {
    u8 pad00[0x2C];
    s32 f2C;
    u8 pad30[0x6C - 0x30];
    f32 x, y, z;
    u8 pad78[0x3FE - 0x78];
    s8 f3FE;
    u8 pad3FF[0x744 - 0x3FF];
    s32 f744;
    u8 pad748[0x7E0 - 0x748];
} Bike;

typedef struct {
    s32 x, y, z;
    s32 rx, ry, rz;
} Rec6;

extern s32 D_800E1B40;
void func_80011A58(s32 *param_1, s32 param_2, s32 param_3, s32 param_4);
void func_80012DC8(s32 *param_1, s32 *param_2, s32 *param_3);
void func_8002B5F8(Rec6 *src);

void func_8002B210(Bike *p, Rec6 *out)
{
    s32 a[3];
    s32 b[3];
    s32 m[10];

    if (p->f3FE != 0) {
        if (p->f744 <= 0) {
            p->f3FE--;
        }
        func_80011A58(m, 0x800, 0, p->f2C + D_800E1B40);
        D_800E1B40 -= 0x40;
        if (D_800E1B40 < 9000) {
            D_800E1B40 = 9000;
        }
        a[0] = 0;
        a[1] = 0;
        a[2] = -4000;
        func_80012DC8(a, m, b);
        out->x = (f32)b[0] + p->x * 1000.0f;
        out->y = (f32)b[1] + p->y * 1000.0f;
        out->z = (f32)b[2] + p->z * 1000.0f;
        out->rx = 0x800;
        out->rz = p->f2C + D_800E1B40;
        out->ry = 0;
        func_8002B5F8(out);
    }
}
