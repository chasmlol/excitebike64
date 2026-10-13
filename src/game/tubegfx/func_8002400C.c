#include "ctx.h"

extern s32 D_800C9550[];
extern s32 D_800C9560[];
extern s32 D_800E1898;
extern s32 D_800E8090[][3];
extern s32 D_800E8000[];
extern s16 D_800C2C60[];
extern s32 D_800C2384;
extern s32 D_800C2388;
extern s32 D_800E7978[];

typedef struct {
    u8 pad[0x4C];
    s32 mode;
} Game;
extern Game D_800EFD70;

typedef struct {
    s32 pad0;
    s32 x0;
    s32 z0;
    s32 x1;
    s32 z1;
    s32 pad14;
    s32 flag;
    u32 bits;
} Rec;

typedef struct {
    u32 count;
    Rec recs[1];
} List;

void func_8002400C(u32 *list, s32 p, s32 strict, s16 *slots, s32 nslots)
{
    s32 unused[10];
    s32 d[4];
    s32 r[4];
    s32 cam0;
    s32 cam1;
    s32 cam2;
    s32 far;
    s32 near;
    s32 j;
    s32 sn;
    s32 cs;
    s32 k17;
    s32 k5;
    s32 n;
    u32 i;
    u32 m2;
    u32 m1;
    s32 a;
    s32 h;
    s32 idx;
    s32 ex;
    s32 ey;
    Rec *rec;

    far = D_800C9550[D_800E1898 - 1];
    near = D_800C9560[D_800E1898 - 1];
    if (slots != NULL) {
        for (sn = 0; sn < nslots; sn++) {
            slots[sn] = -1;
        }
    }
    cam0 = D_800E8090[p][0] >> 4;
    sn = D_800C2C60[(D_800E8000[p] >> 4) & 0xFFF];
    cs = D_800C2C60[((D_800E8000[p] >> 4) - 0x400) & 0xFFF];
    cam1 = D_800E8090[p][1] >> 4;
    cam2 = D_800E8090[p][2] >> 4;
    for (i = 0; i < list[0]; i++) {
        rec = (Rec *)(list + 1) + i;
        ex = ((rec->x1 + rec->x0) >> 1) - cam0;
        ey = ((rec->z1 + rec->z0) >> 1) - cam2;
        rec->flag = 1;
        rec->bits &= ~(2 << (p * 2));
        d[0] = ex;
        d[1] = -cam1;
        d[2] = ey;
        r[0] = (sn * ex + cs * ey) >> 14;
        r[2] = (sn * ey - cs * ex) >> 14;
        if (far < r[2] && strict != 0) {
            rec->bits |= 2 << (p * 2);
        }
        if (D_800EFD70.mode == 9 || D_800EFD70.mode == 2 || D_800E1898 == 2) {
            if (r[2] >= -799 && r[2] < 1200 && r[0] >= -799 && r[0] < 800) goto visible;
        } else {
            if (r[2] >= -399 && r[2] < 800 && r[0] >= -399 && r[0] < 400) goto visible;
        }
        if (r[2] < 0) {
            goto reject;
        }
        a = r[0];
        if (a < -180) {
            a += 360;
        } else if (a > 180) {
            a -= 360;
        }
        h = r[2];
        h /= 2;
        if (D_800E1898 == 2 && D_800C2384 == 0 && D_800C2388 == 0) {
            h = h * 3 >> 1;
            if (D_800E7978[0] > 0x1500) {
                h = (h * 4) / 3;
            }
        }
        if (h < a) {
            goto reject;
        }
        if (a < -h) {
            goto reject;
        }
        if (r[2] > near && strict != 0) {
            goto hidden;
        }
    visible:
        rec->flag = 1;
        rec->bits |= 1 << (p * 2);
        if (slots != NULL) {
            idx = r[2] * nslots / D_800C9560[D_800E1898 - 1];
            if (idx < 0) {
                idx = 0;
            }
            if (idx >= nslots) {
                idx = nslots - 1;
            }
            while (slots[idx] != -1) {
                idx = (idx + 1) % nslots;
            }
            slots[idx] = i;
        }
        continue;
    reject:
        rec->flag = 0;
    hidden:
        rec->bits &= ~(1 << (p * 2));
    }
}
