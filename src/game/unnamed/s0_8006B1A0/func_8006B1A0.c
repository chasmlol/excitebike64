#include "ctx.h"

typedef struct {
    u8 p0[0x48];
    f32 x;
    f32 y;
    f32 z;
    u8 p1[0x78 - 0x54];
    u8 f78[0x44C - 0x78];
    f32 f44C;
    u8 p2[0x6E8 - 0x450];
    u8 f6E8;
    u8 p3[0x7A4 - 0x6E9];
    u8 f7A4;
    u8 f7A5;
    u8 p4[0x7E0 - 0x7A6];
} Ent;

extern Ent D_800E37C0[];
extern f32 D_800CCDC0[];
extern s32 D_800F1504;
extern s32 D_800E76D0;
void func_8001AEB0(Ent *p, s32 x);
void func_80013438(void *p, f32 a, f32 b, f32 c);

void func_8006B1A0(void)
{
    s32 i;

    D_800F1504 = 600;
    D_800E76D0 = 0;
    for (i = 6; i < 8; i++) {
        D_800E37C0[i].f7A4 = 1;
        D_800E37C0[i].f6E8 = 1;
        D_800E37C0[i].f7A5 = 0;
        D_800E37C0[i].x = -1000.0f;
        D_800E37C0[i].y = -50.0f;
        D_800E37C0[i].z = -1000.0f;
        func_8001AEB0(&D_800E37C0[i], 0);
        func_80013438(&D_800E37C0[i].x + 12, 0.0f, 0.0f, 0.0f);
    }
    for (i = 0; i < 8; i++) {
        D_800E37C0[i].f44C = D_800CCDC0[i];
    }
}
