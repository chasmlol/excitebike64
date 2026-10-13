#include "ctx.h"

typedef struct {
    u8 unk0[0x28];
    u32 flags;
    u8 unk2C[0x40 - 0x2C];
} Surface;

typedef struct {
    u8 unk0[0x45];
    u8 unk45;
    u8 unk46[2];
    Vec3f pos;
    Vec3f unk54;
    u8 unk60[0x43C - 0x60];
    Vec3f unk43C;
    u8 unk448[0x454 - 0x448];
    s32 unk454;
    u8 unk458[0x47C - 0x458];
    f32 unk47C;
    u8 unk480[0x4E4 - 0x480];
    Surface *unk4E4;
    u8 unk4E8[0x4F8 - 0x4E8];
    s32 unk4F8;
    u8 unk4FC[0x520 - 0x4FC];
    f32 unk520;
    u8 unk524[0x588 - 0x524];
    Surface *unk588;
    u8 unk58C[0x744 - 0x58C];
    s32 unk744;
    u8 unk748[0x780 - 0x748];
    s32 unk780;
} Bike;

extern s32 D_800E8958;
extern s32 D_800E8D64;
extern s32 D_800E84F8;
extern s32 D_800E84F4[];
extern s32 D_800EFDBC;
extern u8 *D_800EFD64;
extern u8 D_800CBB70[];
extern Surface D_800C9080[];

s32 func_8002E920(s32 x, s32 y, s32 z);
void func_800477F0(Bike *b, s32 arg1);
void func_8001B354(Bike *b, void *arg1, f32 arg2, s32 arg3);


void func_8001B808(Bike *b) {
    f32 diff = 1.0f;
    s32 idx;
    s32 t;
    s32 w;

    do {
        w = b->unk454;
        t = b->unk744;
    } while (0);
    D_800E8D64 = 0;
    D_800E84F8 = 0;
    D_800E8958 = w;
    if (t > 0) {
        s32 h;
        t = (s32)&b->pos;

        h = func_8002E920((s32)(((f32 *)t)[b->unk45 >> 8] * 1000.0f), (s32)(((f32 *)t)[1] * 1000.0f), (s32)(((f32 *)t)[2] * 1000.0f));

        diff = b->pos.y - (f32)h / 1000.0f;
        idx = (D_800E84F4[1] >> 4) & 0xF;
        *(volatile s32 *)&b->unk780 = idx;
        if ((D_800EFDBC == 7 || D_800EFD64 == D_800CBB70) && diff > 2.0f && (D_800C9080[idx].flags & 0x800)) {
            func_800477F0(b, -25);
            return;
        }
    } else {
        b->unk780 = b->unk4E4 - D_800C9080;
    }
    if (diff < 1.0f) {
        if (b->unk520 < -2.0f || b->unk47C < -2.0f) {
            b->unk520 = 0.0f;
            b->unk47C = 0.0f;
        }
    }
    func_8001B354(b, &b->unk4F8, 0.6f, diff > 1.5f);
    func_8001B354(b, &b->unk454, -0.65f, diff > 1.5f);
    if (b->unk588->flags & 0x800) {
        b->unk45 = 0x1C;
        b->unk43C = b->unk54;
    }
    if (b->unk4E4->flags & 0x800) {
        b->unk45 = 0x1C;
        b->unk43C = b->unk54;
    }
}
