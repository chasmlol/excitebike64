#include "ctx.h"

typedef struct { u32 w[7]; } Blk7;
typedef struct { u8 pad0[0x34]; Blk7 b; s32 flags; u8 pad54[0xA0 - 0x54]; } Rec;
typedef struct {
    u8 pad0[8];
    Blk7 blk[24];
    u8 a8[5];
    u8 tbl[5][3];
    u8 pad2BC[4];
    u8 c0, c1;
    u8 pad2C2[3];
    u8 c5, c6, c7, c8;
    u8 pad2C9[7];
    s32 n;
} Save;
typedef struct { u8 p0; u8 a[3]; u8 b[3]; u8 pad[0x24 - 7]; } Ent;
typedef struct {
    Ent e[5];
    u8 count;
    u8 padB5[7];
    s16 x[7];
    s16 y[5];
} Cfg;
typedef struct { u8 pad[0x50]; u8 b[4]; } Snd;

extern void func_8006EFB0(void);
extern void func_80010E5C(s32 mode, s32 v);
extern void func_8006F128(void);

extern Save D_800F1900;
extern Rec D_800CB210[];
extern s32 D_800CA9C4;
extern Cfg D_800CA9C8;
extern u8 D_800CAA7C;
extern Snd D_800EFD70;
extern u8 D_800CAAA1;
extern s32 D_800F0180;

void func_8006F3E4(void)
{
    s32 i, k;
    s32 j;
    u8 v;

    func_8006EFB0();
    do { D_800CA9C4 = 0; } while (0);
    j = 0;
    for (i = 0; i < 27; i++) {
        if (i < D_800F1900.n) {
            D_800CA9C4 |= 1 << i;
        }
        if (D_800CB210[i].flags & 0x200) {
            Blk7 *d = &D_800CB210[i].b;
            Blk7 *s = &D_800F1900.blk[j];
            *d = *s;
            j++;
        }
    }
    D_800CAA7C = 0;
    for (i = 0; i < 4; i++) {
        D_800EFD70.b[i] = (D_800F1900.c8 >> i) & 3;
    }
    func_80010E5C(0, D_800F1900.c6);
    func_80010E5C(1, D_800F1900.c5);
    func_80010E5C(2, D_800F1900.c7);
    for (i = 0; i < 5; i++) {
        for (k = 0; k < 3; k++) {
            D_800CA9C8.e[i].a[k] = D_800F1900.tbl[0][i * 3 + k] >> 5;
            D_800CA9C8.e[i].b[k] = D_800F1900.tbl[0][i * 3 + k] & 0x1F;
        }
    }
    for (i = 0; i < 5; i++) {
        v = D_800F1900.a8[i] & 0x1F;
        if (v == 0x1F) break;
        D_800CA9C8.x[i] = v;
        D_800CA9C8.y[i] = D_800F1900.a8[i] >> 5;
        D_800CA9C8.count++;
    }
    D_800CAAA1 = D_800F1900.c1;
    D_800F0180 = D_800F1900.c0;
    func_8006F128();
}
