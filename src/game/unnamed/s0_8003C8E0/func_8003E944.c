#include "ctx.h"

extern s32 D_800C2358;
typedef struct { u8 pad[0x10]; u8 *anim; u8 rest[0x28]; } Slot;
extern Slot D_800C9A40[];
void func_80013438(f32 *v, f32 x, f32 y, f32 z);

void func_8003E944(u8 *param_1)
{
    s8 idx;
    u8 *ent;
    u8 *st;
    s16 *ps;
    s8 *cp;
    s32 n;
    s32 iv;
    s32 a2;
    s32 six;
    f32 f;

    if (D_800C2358 != 0) {
        return;
    }
    *(s32 *)(param_1 + 0x3A8) = 0;
    if (*(s8 *)(param_1 + 0x3D0) == -1) {
        return;
    }
    st = D_800C9A40[*(s8 *)(param_1 + 0x3D0)].anim;
    if (st == 0) {
        return;
    }
    a2 = *(s32 *)(param_1 + 0x3AC);
    iv = a2 >> 8;
    if (iv < 0) {
        iv = 0;
    }
    n = *(s32 *)(st + 4);
    if (iv >= n) {
        iv = n - 1;
    }
    six = 6;
    ps = (s16 *)*(u8 **)(st + 0x10) + iv * six;
    f = (f32)a2 * 0.0009765625f;
    cp = (s8 *)((u8 *)ps + 6);
    *(f32 *)(param_1 + 0x3A8) = f;
    if (1.0f < f) {
        *(f32 *)(param_1 + 0x3A8) = 1.0f;
    }
    func_80013438((f32 *)(param_1 + 0x38C),
                  (f32)(*(s8 *)((u8 *)ps + 6)) * 3.1415925f * 0.0078125f,
                  (f32)(cp[1]) * 3.1415925f * 0.0078125f,
                  (f32)(cp[2]) * 3.1415925f * 0.0078125f);
    *(f32 *)(param_1 + 0x39C) = (f32)(ps[0]) / 1000.0f;
    *(f32 *)(param_1 + 0x3A0) = (f32)(ps[1]) / 1000.0f;
    *(f32 *)(param_1 + 0x3A4) = (f32)(ps[2]) / 1000.0f;
}
