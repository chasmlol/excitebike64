#include "ctx.h"

typedef struct {
    u8 pad0[0xD];
    u8 id;
    u8 pad1[0x28 - 0xE];
} Track;

typedef struct {
    u32 flags;
    u8 pad0[0x18];
    s32 f1C;
    u8 pad1[0x1C];
    s32 f3C;
    u8 pad2[0xC];
    s32 f4C;
} Game;

typedef struct {
    u8 pad0[0x2C1];
    u8 track;
} Opts;

typedef struct {
    void *p0;
    u8 pad0[0x60];
    s16 f64;
} Obj;

extern Opts D_800F1900;
extern Track D_800CADE4[];
extern char D_800CC250[];
extern u8 D_800CAAA0[];
extern char *D_800EFD64;
extern Game D_800EFD70;
extern s32 D_800F0624;
extern Obj *D_800CA930;

void func_80087EF8(s32 arg)
{
    s32 i;

    if (arg != 0) {
        D_800EFD64 = D_800CC250;
        D_800EFD70.f3C = 1;
        D_800EFD70.f4C = 9;
        D_800F0624 = 0;
        D_800EFD70.f1C = 4;
        D_800EFD70.flags &= ~0x80;
        D_800CA930->p0 = D_800CAAA0;
        D_800CA930->f64 = 0;
        for (i = 0; i < 0x11; i++) {
            if (D_800CADE4[i].id == D_800F1900.track) {
                D_800CA930->f64 = i;
                return;
            }
        }
    }
}

