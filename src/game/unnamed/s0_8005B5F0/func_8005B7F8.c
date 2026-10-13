#include "ctx.h"

typedef struct {
    u8 unk0[0x420];
    f32 unk420;
    u8 unk424[0x5A0 - 0x424];
    f32 unk5A0;
    u8 unk5A4[0x5D0 - 0x5A4];
    u8 unk5D0;
} Bike;

extern f32 D_800C901C[];
extern f32 D_800C9050[];

void func_8005B7F8(Bike *b, f32 maxv) {
    f32 speed = b->unk420;
    s32 i = 0;
    f32 t;
    f32 r;

    while (D_800C9050[i + 1] < speed) {
        i++;
    }
    if (speed < 0.0f) {
        t = 0.0f;
    } else {
        t = (D_800C9050[i + 1] - speed) / (D_800C9050[i + 1] - D_800C9050[i]);
    }
    r = (D_800C901C[i] * t + D_800C901C[i + 1] * (1.0f - t)) * 0.18849557638168335f * 0.5f;
    if (r < maxv) {
        maxv = r;
    } else if (r < -maxv) {
        maxv = -r;
    }
    b->unk5A0 = maxv;
    b->unk5D0 = (s32)((maxv * 13.0f) / r);
}
