#include "ctx.h"

void func_80011C1C(s32 *a, s32 *b, s32 *c);
void func_80012DC8(s32 *a, s32 *b, s32 *c);
void func_8003C4A4(s32 *a, s32 b);
void func_8003C8E0(s32 *a, s32 b, s32 *c, s32 *d);
void func_8003CD80(s32 *a, s32 b, s32 *c, s32 *d);

typedef struct { s32 w[9]; } M9;

extern s32 D_800EA690[];
extern s32 D_800EA6CC[];
extern s32 D_800EA6FC[];
extern s32 D_800EA72C[];
extern s32 D_800EA750[];

void func_8003D180(s32 *param_1)
{
    s32 *p;
    s32 *q;
    s32 *o;
    volatile s32 pad[10];

    p = param_1 + 0x10;
    q = param_1 + 0x30;
    func_80011C1C(param_1 + 0x24, p, q);
    func_80012DC8(param_1 + 0x21, p, param_1 + 0x2d);
    param_1[0x2d] = param_1[0x2d] + param_1[0xd];
    param_1[0x2e] = param_1[0x2e] + param_1[0xe];
    param_1[0x2f] = param_1[0x2f] + param_1[0xf];
    func_80012DC8(param_1 + 0x61, q, param_1 + 0x6d);
    param_1[0x6d] = param_1[0x6d] + param_1[0x2d];
    param_1[0x6e] = param_1[0x6e] + param_1[0x2e];
    param_1[0x6f] = param_1[0x6f] + param_1[0x2f];
    func_80012DC8(param_1 + 0xc1, q, param_1 + 0xcd);
    param_1[0xcd] = param_1[0xcd] + param_1[0x2d];
    param_1[0xce] = param_1[0xce] + param_1[0x2e];
    param_1[0xcf] = param_1[0xcf] + param_1[0x2f];
    func_80012DC8(param_1 + 0x121, p, param_1 + 0x12d);
    param_1[0x12d] = param_1[0x12d] + param_1[0xd];
    param_1[0x12e] = param_1[0x12e] + param_1[0xe];
    param_1[0x12f] = param_1[0x12f] + param_1[0xf];
    func_80012DC8(param_1 + 0x181, p, param_1 + 0x18d);
    param_1[0x18d] = param_1[0x18d] + param_1[0xd];
    param_1[0x18e] = param_1[0x18e] + param_1[0xe];
    param_1[0x18f] = param_1[0x18f] + param_1[0xf];

    func_8003C8E0(param_1, 3, D_800EA690, D_800EA690 + 3);
    func_8003C8E0(param_1, 6, D_800EA690 + 12, D_800EA690 + 15);
    func_8003CD80(param_1, 9, D_800EA690 + 24, D_800EA690 + 27);
    func_8003CD80(param_1, 12, D_800EA690 + 36, D_800EA690 + 39);
    func_8003C4A4(param_1, 15);

    *(M9 *)(param_1 + 0xb0) = *(M9 *)(D_800EA690 + 3);
    *(M9 *)(param_1 + 0x110) = *(M9 *)D_800EA6CC;
    *(M9 *)(param_1 + 0x170) = *(M9 *)D_800EA6FC;
    *(M9 *)(param_1 + 0x1d0) = *(M9 *)D_800EA72C;
    o = param_1 + 0x50;
    *(M9 *)o = *(M9 *)D_800EA750;
}
