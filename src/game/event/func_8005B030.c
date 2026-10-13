#include "ctx.h"

typedef struct {
    u8 pad0[0x58];
    s32 w58;
} G;

extern G D_800F05A8;

void func_80064DB8(void);

void func_8005B030(void) {
    D_800F05A8.w58 |= 0x10;
    func_80064DB8();
}
