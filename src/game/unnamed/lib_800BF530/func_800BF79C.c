#include "ctx.h"

typedef struct {
    u8 key[4];
    void (*fn)(u32);
} Ent8;

extern void (*D_800D5730)(void);
extern s32 D_800D5734;
extern s32 *D_80100AB0;
extern Ent8 D_80100AB8[16];
extern u8 D_800DB440[];
extern u8 D_800DB43C[];
extern u8 D_800DB464[];

extern s32 *func_800BE694(u8 *p, u8 *q);
extern void func_800BE7F4(s32 *o, s32 v, s32 mode);
extern s32 func_800BE848(u8 *p1, s32 p2, s32 p3, s32 *p4);
extern void func_800BE770(s32 *q);
extern s32 *func_800BC908(s32 a, s32 b, u8 *c, s32 d);
extern void func_800BCB04(s32 a);
extern s32 func_8000ECE0(s32 a0);
extern void func_8000EB00(u8 *a, s32 *b, ...);

#define SWAP_BYTES(x) { if (D_800D5734 != 0) { u32 _i; for (_i = 0; _i < 2; _i++) { u8 _t = ((u8 *)&(x))[_i]; ((u8 *)&(x))[_i] = ((u8 *)&(x))[3 - _i]; ((u8 *)&(x))[3 - _i] = _t; } } }

u32 func_800BF79C(u32 *pa0, s32 pa1)
{
    u32 *p1 = pa0;
    s32 p2 = pa1;
    s32 *piVar2;
    s32 *piVar3;
    u32 uVar5;
    u32 i;
    u32 *puVar7;
    s32 local_34;
    s32 local_30;
    s32 local_2c;
    u32 local_38;
    u8 local_40[4];
    u8 *bb;
    s32 iVar8;
    s32 iVar4;

    if (D_800D5730 != 0) {
        (*D_800D5730)();
    }
    uVar5 = ((u32 *)p1[1])[p2];
    bb = (u8 *)(p1 + 0x123);
    if (uVar5 & 1) {
        goto inc;
    }
    if (uVar5 != 0) {
        return uVar5;
    }
inc:
    p1[0x143] = p1[0x143] + 1;
    piVar2 = func_800BE694(bb, D_800DB43C);
    if (piVar2 == 0) {
        goto dec0;
    }
    func_800BE7F4(piVar2, ((s32 *)p1[0x102])[p2], 0);
    func_800BE848(local_40, 1, 4, piVar2);
    func_800BE848((u8 *)&local_38, 1, 4, piVar2);
    SWAP_BYTES(local_38);
    if (D_80100AB0 != 0) {
        piVar3 = D_80100AB0;
        local_2c = 1;
        D_80100AB0 = 0;
    } else {
        local_2c = 0;
        piVar3 = func_800BC908(local_38, 0, D_800DB440, 0x1cd);
    }
    if (((u32)piVar3 & 3) != 0) {
        func_8000EB00(D_800DB464, (s32 *)piVar3);
        func_800BE770(piVar2);
        return 0;
    }
    if (piVar3 == 0) {
        p1[0x143] = p1[0x143] - 1;
        func_800BE770(piVar2);
        return 0;
    }
    local_34 = func_800BE848((u8 *)piVar3, 1, local_38, piVar2);
    if (local_34 == (s32)local_38) {
        goto main_path;
    }
    func_800BCB04((s32)piVar3);
    func_800BE770(piVar2);
dec0:
    p1[0x143] = p1[0x143] - 1;
    return 0;

main_path:
    func_800BE848((u8 *)&local_38, 1, 4, piVar2);
    SWAP_BYTES(local_38);
    for (i = 0; i < local_38; i++) {
        func_800BE848((u8 *)&local_34, 1, 4, piVar2);
        SWAP_BYTES(local_34);
        local_30 = *(s32 *)((u8 *)piVar3 + local_34);
        SWAP_BYTES(local_30);
        *(u32 *)((u8 *)piVar3 + local_34) = p1[1] + local_30 * 4;
        puVar7 = (u32 *)(local_30 * 4 + p1[1]);
        if (*puVar7 == 0) {
            *puVar7 = (p1[0x143] << 2) | 1;
        }
    }
    if (local_2c != 0) {
        ((u32 *)p1[1])[p2] = (u32)piVar3 | 2;
    } else {
        ((s32 *)p1[1])[p2] = (s32)piVar3;
    }
    func_800BE770(piVar2);
    for (i = 0; i < p1[0]; i++) {
        if (((u32 *)p1[1])[i] == ((p1[0x143] << 2) | 1)) {
            func_800BF79C(p1, i);
            i = 0xffffffff;
        }
    }
    for (i = 0; i < 16; i++) {
        iVar8 = func_8000ECE0(D_80100AB8[i].key[0]);
        iVar4 = func_8000ECE0(local_40[0]);
        if (iVar8 == iVar4) {
            iVar8 = func_8000ECE0(D_80100AB8[i].key[1]);
            iVar4 = func_8000ECE0(local_40[1]);
            if (iVar8 == iVar4) {
                iVar8 = func_8000ECE0(D_80100AB8[i].key[2]);
                iVar4 = func_8000ECE0(local_40[2]);
                if (iVar8 == iVar4) {
                    D_80100AB8[i].fn(p1[1] + p2 * 4);
                    break;
                }
            }
        }
    }
    piVar3 = (s32 *)((u32 *)p1[1])[p2];
    p1[0x143] = p1[0x143] - 1;
    return (u32)piVar3;
}
