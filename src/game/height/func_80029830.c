#include "ctx.h"

typedef struct {
    s32 flags;
    u8 pad0[0x3C - 4];
    s32 mode;
    u8 pad1[0x2A8 - 0x40];
    s32 tab[16][2];
    u8 pad2[0x408 - 0x2A8 - 0x80];
    s16 f408;
    s16 pad3;
    u8 f40C;
} Ctl;

typedef struct {
    u8 pad[0x6C];
    f32 x, y, z;
} Obj;

extern Ctl D_800EFD70;
extern s32 D_800C9684;
extern s16 D_800C2C60[];
extern s32 D_800C2388;

void func_80012DC8(s32 *a, s32 *b, s32 *c);
void func_80011A58(s32 *a, s32 b, s32 c, s32 d);
void func_8002B5F8(s32 *a);

void func_80029830(Obj *p, s32 *out)
{
    s32 buf[10];
    s32 in[3];
    s32 res[3];
    s32 amp, yoff;
    s32 r, z;
    s32 *e;
    Ctl *g = &D_800EFD70;

    if (g->flags & (0x400 << D_800C9684)) {
        amp = 4000;
        yoff = 0x672;
    } else {
        amp = 6000;
        if (g->mode == 2 && D_800C2388 != 0) {
            amp = 7200;
        }
        yoff = 0x8FC;
    }
    e = D_800EFD70.tab[D_800C9684];
    out[0] = p->x * 1000.0f + (f32)((D_800C2C60[((e[0] >> 4) - 0x400) & 0xFFF] * amp) >> 14);
    out[1] = p->y * 1000.0f + (f32)yoff;
    r = 0;
    out[2] = p->z * 1000.0f + (f32)((D_800C2C60[(e[0] >> 4) & 0xFFF] * amp) >> 14);
    z = 0;
    out[3] = 0xE10;
    out[4] = 0;
    out[5] = (0x8000U - e[0]) & 0xFFFF;
    func_80011A58(buf, out[3], out[4], out[5]);
    switch (D_800EFD70.mode) {
    case 1:
        switch (D_800EFD70.f40C) {
        case 1:
            r = 0x3E - D_800EFD70.f408;
            break;
        case 2:
            r = 0x3E;
            break;
        case 3:
            r = D_800EFD70.f408;
            break;
        }
        break;
    case 2:
        r = 0x19;
        if (D_800C2388 != 0) {
            z = -0x226;
        } else {
            r = -0x1E;
            z = -0x64;
        }
        break;
    default:
        r = 0x1E;
        z = -0x1F4;
        break;
    }
    r = (r * -0x564) / 0x3E;
    if (D_800EFD70.flags & (0x400 << D_800C9684)) {
        in[0] = 0;
        in[1] = 0;
        in[2] = 0;
    } else {
        in[0] = r + 600;
        in[1] = z;
        in[2] = 0;
    }
    func_80012DC8(in, buf, res);
    out[0] += res[0];
    out[1] += res[1];
    out[2] += res[2];
    func_8002B5F8(out);
}
