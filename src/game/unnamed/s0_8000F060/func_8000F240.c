#include "ctx.h"

typedef struct {
    u8 pad[0xA0];
} Trk;

typedef struct {
    s32 f0, f4, f8, fC, f10;
    s32 pad14[9];
    s32 f38, f3C;
    s32 pad40;
    s32 f44, f48, f4C, f50;
    s32 pad54[4];
} Ent;

typedef struct {
    s32 *p;
    s32 v;
} Pair;

typedef struct {
    u8 pad[0x38];
    s32 a;
    s32 r;
} Slot;

extern s32 D_800C2B5C;
extern s32 D_800C2358;
extern u32 D_800EFD70[];
extern Trk *D_800EFD64;
extern Trk D_800CB210[];
extern s32 D_800C2AD4;
extern s32 D_800C29E0;
extern s32 D_800C2B64;
extern s32 D_800C2B44;
extern s32 D_800C8CE8;
extern Ent D_800E32A8[];
extern Slot *D_800E35CC;
extern Pair D_800C2C10[];
extern u32 D_80000318;
extern s32 D_800D11E4;
extern s32 D_800D11D8;
extern s32 D_800D11EC;
extern s32 D_800D1214;
extern s32 D_800D1224;
extern s32 *D_800C2AD8[];
extern u8 D_800111A0[];

void func_800BBABC(s32 a, s32 b);
void func_800B99B0(s32 a0);
void func_8000ED10(u8 *a0, u8 a1, s32 a2);
s32 func_800B781C(s32 a0);
void func_8000F164(void);
s32 func_8004F180(void);
void func_8004C22C(s32 a0);

void func_8000F240(Slot *param_1)
{
    s32 i;
    u32 idx;
    u32 f;
    s32 a;
    volatile s32 pad[4];

    if (D_800C2B5C == 0) {
        return;
    }
    if (D_800C2358 != 0) {
        return;
    }
    if (!(D_800EFD70[0] & 0x80)) {
        func_8000F164();
    }
    i = D_800EFD64 - D_800CB210;
    if (i == 0xD || i == 0x13 || i == 0x11) {
        D_800C2AD4 = 1;
    } else {
        D_800C2AD4 = 0;
    }
    if (D_80000318 > 0x400000) {
        D_800C2AD4 = 0;
    }
    func_800BBABC(D_800C29E0, 0x25800);
    i = 0;
    D_800C2B64 = 1;
    func_8000ED10((u8 *)D_800E32A8, i, 800);
    for (i = 0; i < 8; i++) {
        D_800E32A8[i].f38 = -0x28;
        D_800E32A8[i].f3C = -0x14;
        D_800E32A8[i].f0 = 0x3C;
        D_800E32A8[i].fC = -1;
        D_800E32A8[i].f4 = -1;
        D_800E32A8[i].f8 = -1;
        D_800E32A8[i].f10 = -1;
        D_800E32A8[i].f44 = -1;
        D_800E32A8[i].f48 = -1;
        D_800E32A8[i].f4C = -1;
        D_800E32A8[i].f50 = -1;
    }
    for (i = 0; D_800C2C10[i].p != 0; i++) {
        if (D_800C2AD4 != 0 && i >= 8) {
            D_800C2C10[i].v = -1;
        } else {
            D_800C2C10[i].v = func_800B781C(*D_800C2C10[i].p);
        }
    }
    D_800E35CC = param_1;
    for (i = 0; i < 16; i++) {
        a = D_800E35CC[i].a;
        if (a == 0) {
            D_800E35CC[i].r = -1;
        } else {
            if (a & 0x80000000) {
                a = *(s32 *)a;
            }
            D_800E35CC[i].r = func_800B781C(a);
        }
    }
    if (func_8004F180() == 0) {
        D_800C2B44 = 1;
        if (D_800C8CE8 != 0) {
            func_8004C22C(D_800D11D8);
        } else {
            idx = D_800EFD64 - D_800CB210;
            f = D_800EFD70[0];
            if ((f & 0x91) == 1 || !(f & 1)) {
                if (f & 0x40000) {
                    func_8004C22C(D_800D11E4);
                } else if (f & 0x1000000) {
                    func_8004C22C(D_800D11D8);
                } else if (f & 0x80) {
                    func_8004C22C(D_800D11EC);
                } else if (f & 1) {
                    func_8004C22C(D_800D1214);
                } else if (D_800EFD70[0x13] != 9) {
                    if (idx < 0x1B) {
                        func_8004C22C(*D_800C2AD8[idx]);
                    } else {
                        func_8004C22C(D_800D1224);
                    }
                }
            }
        }
    } else {
        D_800C2B44 = 0;
    }
    func_800BBABC(0, 0);
    func_800B99B0((s32)D_800111A0);
}
