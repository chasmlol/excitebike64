#include "ctx.h"

void func_8002A8C0(u8 *a, s32 *b, s32 c);

void func_8002D5A0(u8 *p, s32 *p2) {
    s32 v = -0x9C4;

    if (p[0x44] != 0) {
        v = -5000;
    }
    if (*(f32 *)(p + 0x424) < 0.0f) {
        v = (s32)((f32)v + *(f32 *)(p + 0x424) * 250.0f);
    }
    func_8002A8C0(p, p2, v);
}
