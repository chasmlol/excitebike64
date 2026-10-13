#include "ctx.h"

typedef struct {
    u8 pad0[0x48];
    f32 v48[3];
    f32 v54[3];
    u8 pad60[0x40];
    f32 vA0[3];
    u8 padAC[0x2A4];
    s32 cnt350;
    f32 v354[3];
} Obj;

extern s32 D_800E1898;
extern u8 D_800E37C0[];
extern u8 D_800C9994[];

u32 func_8000EFE8(void);
void func_8001327C(f32 *v, f32 *m, f32 *o);
void func_80036374(s32 a, s32 b, f32 *c, f32 *d, void *e);

void func_80036544(Obj *param_1)
{
    f32 o[3];
    f32 t[3];
    u32 r;
    s32 iv;

    if (D_800E1898 == 1 && param_1->cnt350 != 0) {
        param_1->cnt350 = param_1->cnt350 - 1;
        if ((void *)param_1 == (void *)D_800E37C0) {
            func_8001327C(param_1->v354, param_1->vA0, o);
            o[0] = param_1->v48[0] + o[0];
            o[1] = param_1->v48[1] + o[1];
            o[2] = param_1->v48[2] + o[2];
            t[0] = param_1->v54[0] * 0.016666668f;
            t[1] = param_1->v54[1] * 0.016666668f;
            t[2] = param_1->v54[2] * 0.016666668f;
            r = func_8000EFE8();
            if (0.0f < param_1->v354[0]) {
                iv = r + 0x80;
            } else {
                iv = r - 0x80;
            }
            t[0] = t[0] + param_1->vA0[0] * ((f32)iv * 0.00012207031f);
            t[2] = t[2] + param_1->vA0[2] * ((f32)iv * 0.00012207031f);
            r = func_8000EFE8();
            t[1] = t[1] + (f32)(s32)(r - 0x40) * 0.02f * 0.0078125f;
            func_80036374(2, 0, o, t, D_800C9994);
        }
    }
    return;
}
