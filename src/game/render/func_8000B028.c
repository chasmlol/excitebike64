#include "ctx.h"

typedef struct { s32 x, y, z, w; } V4;
typedef struct { s16 v[8]; } Q;

extern s32 D_800C2380;
extern s32 D_800C2384;
extern V4 D_800C2494[4][4];
extern s32 D_800C2594;
extern s32 D_800C2598;
extern s32 D_800CCCA8;
extern V4 D_800E1718[4][4];
extern V4 D_800E1758[];
extern s32 D_800E1898;
extern s32 D_800E1B40;
extern Q *D_800E19C0;
extern Q D_800E19D0[];
extern Q D_800E1AC0[];
extern s32 D_800F13EC[];
extern s32 D_800F13F0[];

void func_8000B028(void)
{
    s32 i;
    s32 j;
    s32 n;
    Q *pb;
    V4 *s;
    V4 *p;
    s32 t;
    s32 u;

    if (D_800C2384 != D_800C2380) {
        if (D_800C2380 != 0) {
            D_800C2594 = 2;
            D_800E19C0 = D_800E19D0;
            D_800E1B40 = 24000;
        } else {
            D_800E19C0 = D_800E1AC0;
            D_800C2594 = D_800E1898;
        }
        for (i = 0; i < 4; i++) {
            for (j = 0; j < 4; j++) {
                D_800E1718[i][j].x = D_800C2494[i][j].x * D_800F13EC[0] / 512;
                D_800E1718[i][j].z = D_800C2494[i][j].z * D_800F13EC[0] / 512;
                D_800E1718[i][j].y = D_800C2494[i][j].y * D_800F13F0[0] / 240;
                D_800E1718[i][j].w = D_800C2494[i][j].w * D_800F13F0[0] / 240;
            }
        }
        if (D_800C2380 != 0) {
            t = *D_800F13EC;
            D_800E1758[0] = D_800E1758[-4];
            p = D_800E1758 - 4;
            p[5].x = t / 16;
            p[5].z = *D_800F13EC * 130 / 512;
            p[5].w = D_800F13F0[0] * 70 / 240;
            if (D_800CCCA8 == 2) {
                p[5].y = 100;
            } else {
                p[5].y = 30;
            }
        }
        pb = D_800E19C0;
        n = D_800C2594 - 1;
        for (D_800C2598 = 0; D_800C2598 < 4; D_800C2598++) {
            s = (V4 *)((u8 *)D_800E1718 + (D_800C2598 * 16 + n * 64));
            pb[D_800C2598].v[0] = s->z * 2;
            pb[D_800C2598].v[1] = s->w * 2;
            pb[D_800C2598].v[4] = (s->x + s->z / 2) * 4;
            pb[D_800C2598].v[5] = (s->y + s->w / 2) * 4;
            pb[D_800C2598 + 4].v[0] = s->z * 2;
            pb[D_800C2598 + 4].v[1] = s->w * 2;
            pb[D_800C2598 + 4].v[4] = (s->x + s->z / 2) * 4;
            pb[D_800C2598 + 4].v[5] = (s->y + s->w / 2) * 4 + 2;
            pb[D_800C2598].v[2] = 0x1FF;
            pb[D_800C2598].v[6] = 0x1FF;
            pb[D_800C2598 + 4].v[2] = 0x1FF;
            pb[D_800C2598 + 4].v[6] = 0x1FF;
        }
        if (D_800C2380 != 0) {
            D_800E19D0[0].v[2] = 0xFF;
            D_800E19D0[0].v[6] = 0x2FF;
            D_800E19D0[4].v[2] = 0xFF;
            D_800E19D0[4].v[6] = 0x2FF;
            D_800E19D0[1].v[2] = 0xFF;
            D_800E19D0[1].v[6] = 0xFF;
            D_800E19D0[5].v[2] = 0xFF;
            D_800E19D0[5].v[6] = 0xFF;
        }
        D_800C2384 = D_800C2380;
    }
}
