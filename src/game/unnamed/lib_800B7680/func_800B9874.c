#include "ctx.h"

typedef struct {
    u8 pad0[0x4C];
    s32 (*fn)(void);
} Vt;

extern s32 D_800D33F0;
extern Vt *D_800FE860;

s32 func_800B9874(void) {
    if (D_800D33F0 == 0) {
        return -1;
    }
    return D_800FE860->fn();
}
