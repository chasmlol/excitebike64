#include "ctx.h"

typedef struct {
    u8 b[6];
} Blk6;

extern Blk6 D_800F0C98;
extern Blk6 D_800F0CA0;
extern Blk6 D_800F0BD0;

void func_80065080(void) {
    D_800F0CA0 = D_800F0C98;
    D_800F0BD0 = D_800F0CA0;
}
