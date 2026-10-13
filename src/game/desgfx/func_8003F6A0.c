#include "ctx.h"

extern s32 func_800BF79C(s32 a0, s32 a1);
extern void *func_800BC908(s32 a0, s32 a1, void *a2, s32 a3);
extern void func_80044314(void *a0);
extern void func_80044358(void);
extern s32 func_800422AC(s32 a0);
extern void func_8000ED10(void *a0, s32 a1, s32 a2);

extern s32 D_800C23F0;
extern s32 D_800E7728;
extern s32 D_800D0B68;
extern s32 D_800D1014;
extern s32 D_800D0DF0;
extern s32 D_800D0C50;
extern s32 D_800D07C4;
extern s32 D_800D0BC8;
extern s32 D_800D6FD0[];
typedef struct { s32 *name; s32 val; } NP;
extern NP D_800CA3BC[];
extern s32 D_800E783C;
extern s32 D_800E781C;
typedef struct { u8 pad[0x198]; u8 f198; u8 rest[0x1A8 - 0x199]; } B1A8;
extern B1A8 *D_800ED7AC;
extern void *D_800EA990;
extern s32 D_800EA9A8[];
extern u8 D_800EBE08[];
extern u8 D_800EA998[];
extern s32 D_800EB9E8;
extern s32 D_800ED7C8[];
extern void *D_800EBDF8[];
extern s32 D_800EBE04;
extern s32 D_800EA988;
extern void *D_800EDBB4;
extern s32 D_800EDFD0;
extern s32 D_800EA98C;

void func_8003F6A0(void)
{
    s32 i;

    D_800C23F0 = 0x4000;
    D_800E783C = func_800BF79C(D_800E7728, D_800D0B68);
    for (i = 0; D_800CA3BC[i].name != 0; i++) {
        D_800CA3BC[i].val = func_800BF79C(D_800E7728, *D_800CA3BC[i].name);
    }
    D_800E781C = func_800BF79C(D_800E7728, D_800D1014);
    D_800ED7AC = func_800BC908(0x19e10, 0, D_800D6FD0, 0xe9);
    for (i = 0; i < 0xfa; i++) {
        D_800ED7AC[i].f198 = 0;
    }
    D_800EA990 = func_800BC908(0x40000, 0, D_800D6FD0, 0xed);
    func_80044314(D_800EA990);
    func_80044358();
    for (i = 0; i < 0x10; i++) {
        D_800EA9A8[i] = func_800422AC(i);
    }
    func_8000ED10(D_800EBE08, -1, 0x1800);
    func_8000ED10(D_800EA998, 0, 0xc);
    D_800EB9E8 = 0xfa;
    for (i = 0xf9; i >= 0; i--) {
        D_800ED7C8[i] = i;
    }
    D_800EBDF8[0] = func_800BC908(0x400, 0, D_800D6FD0, 0xfe);
    D_800EBDF8[1] = func_800BC908(0x400, 0, D_800D6FD0, 0xff);
    D_800EBE04 = func_800BF79C(D_800E7728, D_800D0DF0);
    D_800EA988 = func_800BF79C(D_800E7728, D_800D0C50);
    D_800EDBB4 = func_800BC908(0x80, 0, D_800D6FD0, 0x104);
    D_800EDFD0 = func_800BF79C(D_800E7728, D_800D07C4);
    D_800EA98C = func_800BF79C(D_800E7728, D_800D0BC8);
}
