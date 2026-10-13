#include "ctx.h"

typedef struct {
    s32 x, y, z, w, a, b, c;
} Elem;

typedef struct {
    u8 pad[0x10];
    s32 busy;
    u8 pad2[0x30C];
    s32 n;
    Elem e[8];
} Group;

extern s16 D_800C2C60[];
extern Group *D_800EE138;

s32 func_80043F24(s32 x, s32 z, s32 ang, s32 range, Elem *out, s32 max)
{
    s32 n;
    s32 i, j;
    s16 c, s;
    s32 dx, dz, a, b;

    range /= 16;
    n = 0;
    c = D_800C2C60[(ang >> 4) & 0xFFF];
    s = D_800C2C60[((ang >> 4) - 0x400) & 0xFFF];
    for (i = 0; i < 16; i++) {
        if (n >= max) {
            break;
        }
        if (D_800EE138[i].busy == 0) {
            for (j = 0; j < D_800EE138[i].n; j++) {
                if (n >= max) {
                    break;
                }
                dx = (D_800EE138[i].e[j].x - x) / 16;
                dz = (D_800EE138[i].e[j].z - z) / 16;
                a = (c * dx + s * dz) >> 14;
                b = (c * dz - s * dx) >> 14;
                if (b > range) {
                    continue;
                }
                if (b < -359 || a < -359 || a > 359) {
                    if (a < 0) {
                        a -= 360;
                    }
                    if (a > 0) {
                        a += 360;
                    }
                    if (b < a || a < -b || b < 0) {
                        continue;
                    }
                }
                out[n++] = D_800EE138[i].e[j];
            }
        }
    }
    return n;
}
