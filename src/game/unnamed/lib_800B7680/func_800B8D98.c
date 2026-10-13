#include "ctx.h"

typedef struct {
    s32 f0, f4, f8, fc, f10;
    s32 cnt;
} Ent;

extern s32 D_800D33F0;
extern Ent D_800FA860[];

void func_800B78F0(s32);

void func_800B8D98(void)
{
    s32 i;

    if (D_800D33F0 != 0) {
        i = 0;
        do {
            if (0 < D_800FA860[i].cnt) {
                D_800FA860[i].cnt = 1;
                func_800B78F0(i);
            }
            i++;
        } while (i < 0x100);
    }
}
