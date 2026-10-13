#include "ctx.h"

typedef struct Sub {
    s32 pad0[5];
    s32 *f14;
    s32 pad18;
    s32 *f1c;
    s32 pad20;
    s32 *f24;
    s32 *f28;
    s32 *f2c;
} Sub;

typedef struct Blk {
    s32 *f00;
    s32 pad04;
    s32 *f08;
    s32 pad0c;
    s32 *f10;
    s32 *f14;
    s32 *f18;
    s32 *f1c;
    s32 *f20;
    s32 pad24;
    Sub *f28;
} Blk;

extern Blk D_800F5CC0;
extern s32 *D_800D0680;

void func_800BFDD4(s32 *p);
void func_800BCB04(s32 *p);
void func_800A5964(s32 a);
void func_80031D24(void);

void func_800A5840(void)
{
    func_800BFDD4(D_800D0680);
    D_800D0680 = 0;
    if (D_800F5CC0.f00 != 0) {
        func_800BCB04(D_800F5CC0.f00);
        func_800BCB04(D_800F5CC0.f08);
        func_800BCB04(D_800F5CC0.f10);
        func_800BCB04(D_800F5CC0.f14);
        func_800BCB04(D_800F5CC0.f18);
        func_800BCB04(D_800F5CC0.f1c);
        func_800BCB04(D_800F5CC0.f20);
        if (D_800F5CC0.f28->f14 != 0) {
            func_800BCB04(D_800F5CC0.f28->f14);
        }
        if (D_800F5CC0.f28->f24 != 0) {
            func_800BCB04(D_800F5CC0.f28->f24);
        }
        if (D_800F5CC0.f28->f1c != 0) {
            func_800BCB04(D_800F5CC0.f28->f1c);
        }
        if (D_800F5CC0.f28->f28 != 0) {
            func_800BCB04(D_800F5CC0.f28->f28);
        }
        if (D_800F5CC0.f28->f2c != 0) {
            func_800BCB04(D_800F5CC0.f28->f2c);
        }
        func_800BCB04(D_800F5CC0.f28);
    }
    func_800A5964(0);
    func_80031D24();
}
