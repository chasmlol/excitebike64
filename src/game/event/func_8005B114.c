#include "ctx.h"

typedef struct {
    u8 pad0[0x48];
    s32 f48;
    u8 pad1[0xC];
    u32 f58;
} Blk;

typedef struct {
    u8 pad0[0x3C];
    s32 count;
} Ctr;

typedef struct {
    u8 pad0[0x69C];
    s32 f69C;
    u8 pad1[0x7E0 - 0x6A0];
} Ent;

extern Blk D_800F05A8;
extern Ctr D_800EFD70;
extern Ent D_800E37C0[];
void func_80064DB8(void);

void func_8005B114(void) {
    Ent *e;

    D_800F05A8.f48 = -1;
    D_800F05A8.f58 |= 0x10;
    func_80064DB8();
    for (e = D_800E37C0; e < &D_800E37C0[D_800EFD70.count]; e++) {
        e->f69C = 7200;
    }
}
