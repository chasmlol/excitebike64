#include "ctx.h"

void func_8005D004(char *param_1, f32 param_2);
void func_8005CD30(char *p, f32 x, s32 n);

typedef struct {
    u8 pad0[0x44C];
    f32 f44C;
    f32 f450;
    u8 pad1[0x7E0 - 0x454];
} Ent;

typedef struct {
    u8 pad0[0x20];
    f32 f20[1];
} Ply;

typedef struct {
    u8 pad0[0x410];
    s32 f410;
} Gm;

extern s32 D_800E3770;
extern s32 D_800E36E0;
extern s32 D_800CB178;
extern s32 D_800C8D08;
extern s32 D_800C8D10;
extern s32 D_800C8D00;
extern s32 D_800C8D18;
extern f32 D_800E76E8;
extern f32 D_800E76EC;
extern f32 D_800E76F0;
extern f32 D_800E76F4;
extern f32 D_800E76F8;
extern f32 D_800E76FC;
extern Ply *D_800EFD64;
extern Gm D_800EFD70;
extern Ent D_800E37C0[];
extern char D_800D8B10[];
extern char D_800D8B18[];
extern char D_800D8B20[];
extern char D_800D8B24[];
extern char D_800D8B28[];
extern char D_800D8B2C[];
extern char D_800D8B30[];
extern char D_800D8B34[];

void func_8005C910(void)
{
    s32 i;
    char buf[0x28];

    if (D_800E3770 & 0x80) {
        if (D_800E36E0 == 0) {
            D_800CB178++;
            if (D_800CB178 == 4) {
                D_800CB178 = 0;
            }
        }
    }
    for (i = 0x13; i >= 0; i--) {
    }
    switch (D_800CB178) {
    case 1:
        if (*(s32 *)((u8 *)D_800EFD64 + 0x2C) == 2) {
            break;
        }
        if (D_800C8D08 != 0 || D_800C8D10 != 0 || D_800C8D00 != 0 || D_800C8D18 != 0) {
            break;
        }
        func_8005D004(D_800D8B10, D_800E76E8);
        func_8005D004(D_800D8B18, D_800E76EC);
        break;
    case 0:
        func_8005D004(D_800D8B20, D_800E76E8);
        func_8005D004(D_800D8B24, D_800E76EC);
        func_8005D004(D_800D8B28, D_800E76F0);
        func_8005D004(D_800D8B2C, D_800E76F4);
        func_8005D004(D_800D8B30, D_800E76F8);
        func_8005D004(D_800D8B34, D_800E76FC);
        break;
    case 2:
        for (i = 0; i < 6; i++) {
            func_8005CD30(buf, D_800E37C0[i].f44C / D_800EFD64->f20[D_800EFD70.f410 - 1], 4);
            func_8005D004(buf, D_800E37C0[i].f450);
        }
        break;
    }
}
