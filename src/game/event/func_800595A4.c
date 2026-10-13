#include "ctx.h"

typedef struct {
    u8 *hdr;
    u8 pad[0x60];
    s16 idx;
} Ctl;

extern Ctl *D_800CA930;
extern u8 D_800CADE4[];

s32 func_800595A4(void) {
    Ctl *c = D_800CA930;
    s32 a1 = c->idx;
    s32 n = a1 + 1;
    u8 v1, v0;

    if (n > 16) {
        return 0;
    }
    v1 = D_800CADE4[a1 * 0x28 + 0xD];
    v0 = D_800CADE4[n * 0x28 + 0xD];
    if (v1 == v0) {
        return 1;
    }
    return v1 < c->hdr[1];
}
