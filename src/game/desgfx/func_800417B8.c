#include "ctx.h"

extern s32 D_800ED698;
extern s32 D_800ED6A0;
extern s32 D_800ED690;
extern s32 D_800ED694;
typedef struct { u8 pad[0x190]; s32 x; s32 y; s8 life; u8 p199; u8 near; u8 snd; u8 p19c[5]; u8 vis; u8 p1a2[6]; } Rec;
extern Rec *D_800ED7AC;
extern s32 D_800EA998[];
extern s16 D_800EBE08[3][0x400];
extern u8 D_800EB9F0[0x20][0x20];
extern u8 D_800EDBD0[];
extern s32 D_800EB9E8;
extern s32 D_800ED7C8[];

void func_80041E90(s32 n, s32 x, s32 y, s32 mode);

#define REC(h) (&D_800ED7AC[h])

static inline void divmod(s32 v, s32 d, s32 *q, s32 *r)
{
    *q = v / d;
    *r = v - *q * d;
    if (*r < 0) {
        *r += d;
        *q -= 1;
    }
}

void func_800417B8(void)
{
    s32 qx0, rx, qy0, ry, qx, qy;
    s32 i, j, h, s3, wx, wy, b, dr, dc;

    divmod(D_800ED698 >> 4, 500, &qx0, &rx);
    divmod(D_800ED6A0 >> 4, 500, &qy0, &ry);

    for (i = 0; i < 3; i++) {
        j = 1 << i;
        D_800EA998[i] = ((qy0 / j) * 0x20 + qx0 / j) & 0x3ff;
    }
    D_800ED690 = qx0;
    D_800ED694 = qy0;

    for (i = 0; i < 250; i++) {
        if (REC(i)->life > 0)
            REC(i)->life--;
        REC(i)->vis = 0;
    }

    for (i = 0; i < 3; i++) {
        for (j = 0; j < 0x400; j++) {
            h = D_800EBE08[i][j];
            if (h != -1 && REC(h)->life <= 0)
                D_800EBE08[i][j] = -1;
        }
    }

    D_800EB9E8 = 0;
    for (i = 0; i < 250; i++) {
        if (REC(i)->life <= 0) {
            D_800ED7C8[D_800EB9E8++] = i;
        }
    }

    for (i = 0; i < 3; i++) {
        divmod(D_800ED698 >> 4, 500 << i, &qx, &rx);
        divmod(D_800ED6A0 >> 4, 500 << i, &qy, &ry);
        s3 = D_800EA998[i];
        wy = (qy - 16) * (500 << i);
        for (j = 0; j < 0x20; j++) {
            s32 k;
            wx = (qx - 16) * (500 << i);
            for (k = 0; k < 0x20; k++) {
                if ((D_800EB9F0[0][j * 32 + k] >> i) & 1) {
                    s32 t = D_800EBE08[i][s3];
                    s32 n;
                    if (t == -1) {
                        if (D_800EB9E8 == 0) goto out; h = D_800ED7C8[--D_800EB9E8]; D_800EBE08[i][s3] = h; func_80041E90(h, wx, wy, i);
                    } else {
                        h = t;
                        if (REC(h)->x != wx || REC(h)->y != wy) {
                            if (D_800EB9E8 == 0) goto out; h = D_800ED7C8[--D_800EB9E8]; D_800EBE08[i][s3] = h; func_80041E90(h, wx, wy, i);
                        } else {
                            REC(h)->life = 2;
                        }
                    }
                    REC(h)->vis = 1;
                    dr = j - 16;
                    if (dr < 0)
                        dr = -dr;
                    dc = k - 16;
                    if (dc < 0)
                        dc = -dc;
                    if (dc + dr < 7)
                        REC(h)->near = 1;
                    else
                        REC(h)->near = 0;
                } else {
                    D_800EBE08[i][s3] = -1;
                }
                s3 = (s3 + 1) & 0x3ff;
                wx += 500 << i;
            }
            wy += 500 << i;
        }
    }
out:
    for (i = 0; i < 250; i++)
        REC(i)->snd = 0;

    for (i = 0; i < 2; i++) {
        s3 = D_800EA998[i];
        for (j = 0; j < 0x400; j++) {
            h = D_800EBE08[i][s3];
            if (h != -1) {
                Rec *r = (Rec *)(h * 0x1a8 + (u32)D_800ED7AC);
                u8 x = D_800EDBD0[j];
                if (i == 0)
                    r->snd = x >> 4;
                else
                    r->snd = x & 0xf;
            }
            s3 = (s3 + 1) % 0x400;
        }
    }
}
