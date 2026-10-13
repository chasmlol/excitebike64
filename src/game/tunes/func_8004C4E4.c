#include "ctx.h"

typedef struct { s32 idx; s32 cnt; s32 nidx; s32 ncnt; } Seq;
typedef struct { s32 f0, f4, f8, fC, f10; } Ent;
typedef struct {
    u8 pad[0xF4];
    s32 fF4; s32 fF8; s32 fFC; s32 f100; s32 f104; s32 f108;
    u8 pad2[0x120 - 0x10C];
    s32 f120; s32 f124; s32 f128; s32 f12C;
} Obj;

extern s32 D_800EFCF8;
extern Seq D_800EFCE0;
extern Obj *D_800EFD24;
extern Ent *D_800EFD4C;
extern s32 D_800EECD4;
extern s32 D_800CA854[];
extern s32 D_800E32A0[];

void func_8004C4E4(void)
{
    Obj *obj; Ent *e;
    if (D_800EFCF8 != 0) {
        if (--D_800EFCE0.cnt <= 0) {
            obj = D_800EFD24;
            e = &D_800EFD4C[D_800EFCE0.idx];
            obj->fF4 = D_800EFCE0.idx;
            obj->f120 = 0;
            obj->f124 = 0;
            obj->fF8 = 0;
            obj->f104 = e->f8 + D_800EECD4;
            obj->f108 = e->fC;
            obj->f100 = e->f10;
            obj->f12C = (D_800CA854[0] * D_800E32A0[0]) >> 8;
            D_800EFCF8--;
            D_800EFCE0.idx = D_800EFCE0.nidx;
            D_800EFCE0.cnt = D_800EFCE0.ncnt;
        }
    }
}
