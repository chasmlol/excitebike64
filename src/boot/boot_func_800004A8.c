#include "ctx.h"

extern s32 D_800137B0;
extern s32 D_80015960;
extern void boot_func_800010D0(void);

void boot_func_800084E0(void *a, s32 b, void *c, s32 d, void *e, s32 f);
void boot_func_80008780(void *a);
void boot_func_800086B0(void *a, s32 b);

void boot_func_800004A8(s32 param_1) {
    boot_func_800084E0(&D_800137B0, 3, (void *)boot_func_800010D0, param_1, &D_80015960, 10);
    boot_func_80008780(&D_800137B0);
    boot_func_800086B0((void *)0, 0);
    while (1) {
    }
}
