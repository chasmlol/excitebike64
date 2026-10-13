#include "ctx.h"

typedef struct {
    u8 pad0[0xA0];
    u8 *p0;
    u8 *p1;
    s32 fA8;
    s32 fAC;
    u8 pad1[0xC4 - 0xB0];
    s32 fC4;
    s32 fC8;
    s32 fCC;
    s32 fD0;
} Obj;

extern Obj *D_800EFD24;
extern u8 D_800EECE0[];
extern u8 *D_800EECC4;
extern s32 D_800CA858;

void func_8000ED10(u8 *a0, u8 a1, s32 a2);

void func_8004F534(void)
{
    Obj *o;
    u8 *p;

    o = D_800EFD24;
    p = o->p0;
    o->fA8 = -1;
    o->fAC = -1;
    o->fC8 = -1;
    o->fC4 = -1;
    o->fD0 = -1;
    o->fCC = -1;
    func_8000ED10(p, 0, 0x220);
    func_8000ED10(D_800EFD24->p1, 0, 0x800);
    D_800CA858 = 0;
    func_8000ED10(D_800EECE0, 0, 0x1000);
    func_8000ED10(D_800EECC4, 0, 0x2000);
}
