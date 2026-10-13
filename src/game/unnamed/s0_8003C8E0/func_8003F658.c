#include "ctx.h"

typedef struct {
    u32 f0;
    void *f4;
    u32 f8;
    u32 fc;
    u32 f10;
    u8 pad[0x3C - 0x14];
} Slot;

extern u32 D_800C2358;
extern Slot D_800C9A40[];

void func_8003F658(void) {
    s32 i;

    if (D_800C2358 == 0 && D_800C9A40[0].f4 != NULL) {
        i = 0;
        do {
            D_800C9A40[i].fc = 0;
            D_800C9A40[i].f10 = 0;
            i++;
        } while (D_800C9A40[i].f4 != NULL);
    }
}
