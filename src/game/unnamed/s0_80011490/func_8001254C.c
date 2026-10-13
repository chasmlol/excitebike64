#include "ctx.h"

extern s16 D_800C4C60[];
extern s16 D_800C2C60[];

void func_8001254C(s32 x, s32 y, s16 *pAng, s32 *pRatio)
{
    s32 a;
    s32 t;
    s32 nx, ny;
    s32 r;

    if (x == 0 && y == 0) {
        *pRatio = 0;
        *pAng = 0;
        return;
    }
    if (x > 0) {
        if (y > 0) {
            if (x < y) {
                t = D_800C4C60[(x << 12) / y];
                *pRatio = (y << 14) / D_800C2C60[(t >> 4) & 0xFFF];
                a = t;
                *pAng = a;
                return;
            } else {
                a = 0x4000 - D_800C4C60[(y << 12) / x];
                *pRatio = (x << 14) / D_800C2C60[((a >> 4) - 0x400) & 0xFFF];
                *pAng = a;
                return;
            }
        } else {
            if (-y < x) {
                a = D_800C4C60[(-y << 12) / x] + 0x4000;
                *pRatio = (x << 14) / D_800C2C60[((a >> 4) - 0x400) & 0xFFF];
                *pAng = a;
                return;
            } else {
                a = 0x8000 - D_800C4C60[(x << 12) / -y];
                *pRatio = (y << 14) / D_800C2C60[(a >> 4) & 0xFFF];
                *pAng = a;
                return;
            }
        }
    } else {
        nx = -x;
        if (y < 0) {
            ny = -y;
            if (nx < ny) {
                a = D_800C4C60[(nx << 12) / ny] + 0x8000;
                *pRatio = (y << 14) / D_800C2C60[(a >> 4) & 0xFFF];
                *pAng = a;
                return;
            } else {
                a = 0xC000 - D_800C4C60[(ny << 12) / nx];
                *pRatio = (x << 14) / D_800C2C60[((a >> 4) - 0x400) & 0xFFF];
                *pAng = a;
                return;
            }
        } else {
            if (y < nx) {
                a = D_800C4C60[(y << 12) / nx] + 0xC000;
                *pRatio = (x << 14) / D_800C2C60[((a >> 4) - 0x400) & 0xFFF];
                *pAng = a;
                return;
            } else {
                a = -D_800C4C60[(nx << 12) / y];
                *pRatio = (y << 14) / D_800C2C60[(a >> 4) & 0xFFF];
                *pAng = a;
                return;
            }
        }
    }
}
