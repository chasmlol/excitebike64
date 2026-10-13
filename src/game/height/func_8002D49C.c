#include "ctx.h"

typedef struct {
    u8 pad[0x70];
    f32 f70;
    f32 f74;
} Obj;

extern f32 D_800C9794[];
extern void func_8002B5F8(void *);

void func_8002D49C(Obj *param_1, s32 *param_2)
{
    f32 k = 1000.0f;

    param_2[0] = (s32)-D_800C9794[0];
    param_2[1] = (s32)(param_1->f70 * k + D_800C9794[1]);
    param_2[2] = (s32)(param_1->f74 * k - D_800C9794[2]);
    param_2[3] = (s32)D_800C9794[4];
    param_2[5] = (s32)-D_800C9794[3];
    param_2[4] = 0;
    func_8002B5F8(param_2);
}
