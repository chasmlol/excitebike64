#include "ctx.h"

typedef struct {
    u8 b[6];
} Blob6;

extern Blob6 D_800F0BD0;
extern Blob6 D_800F0CA0;

void func_80065048(void)
{
    D_800F0CA0 = D_800F0BD0;
}
