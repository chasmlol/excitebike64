#include "ctx.h"

typedef struct {
    u8 pad0[0x54];
    s32 f54;
    u8 pad58[0x750 - 0x58];
    s32 timer;
} Glob;

typedef struct {
    u8 pad0[0x16];
    s16 type;
} Item;

typedef struct {
    s32 f0;
    s32 f4;
} Ctl;

void func_8001102C(s32 x);
void func_8007AF38(s32 a, s32 b, s32 c);
extern Glob D_800EFD70;
extern s32 D_800E30B0;
extern s32 D_800E25B0;

#define PRESSED(m) ((D_800E30B0 & (m)) != 0 && (D_800E25B0 & (m)) == 0)

void func_8007417C(Item *a, Ctl *b)
{
    s32 flag;
    s32 ok;
    s16 type;

    flag = 0;
    type = a->type;
    if (type == 0x15 || type == 0x17) {
        ok = D_800EFD70.timer < 0x78;
    } else if (type == 0x18) {
        ok = D_800EFD70.timer < 0x474;
    } else if (type == 0x19) {
        ok = D_800EFD70.timer < 0x21C;
    } else {
        ok = D_800EFD70.timer < 0xF0;
    }
    if (ok) {
        if (PRESSED(0x10) | PRESSED(0x40)) {
            flag = 1;
        }
    }
    if (--D_800EFD70.timer == 0 || flag) {
        if (flag && D_800EFD70.f54 == 3) {
            func_8001102C(1);
        }
        func_8007AF38(b->f4, 0, 1);
    }
}
