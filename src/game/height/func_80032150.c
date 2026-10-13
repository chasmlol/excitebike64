#include "ctx.h"

typedef struct {
    s32 flags;
    u8 pad[0x20];
} Ent;

typedef struct {
    s32 f0;
    s32 n;
    Ent e[1];
} Hdr;

typedef struct {
    u8 pad[4];
    s16 v;
    u8 pad2[0xA];
} Cell;

typedef struct {
    u8 w, h;
    u8 pad[2];
    u8 *buf;
} Item;

typedef struct {
    u32 n;
    u32 pad;
    Item it[1];
} Tab;

typedef struct {
    s32 x, d, y;
} Pt;

typedef struct {
    u8 pad[0x2C];
    s32 mode;
    u8 pad2[0x20];
    s32 flags;
} Cfg;

extern Pt *D_800C9840;
extern Hdr *D_800C9860;
extern Cell *D_800E8E88[2];
extern Tab *D_800E8E80;
extern void *D_800E8E98;
extern s32 D_800E8E9C;
extern s32 D_800E7728;
extern s32 D_800D0F78;
extern s32 D_800D14EC;
extern s32 D_800D0E00;
extern char D_800D6C80[];
extern Cfg *D_800EFD64;

s32 func_8005B58C(s32 key);
s32 func_8000EFE8(void);
s32 func_8000EB18(s32 r);
void *func_800BF79C(s32 a, s32 b);
void *func_800BC908(s32 a, s32 b, char *c, s32 d);

void func_80032150(void)
{
    s32 v;
    s32 i, j, k, n, a, b, c;
    s32 x, y;

    D_800C9840 = 0;
    D_800E8E88[0] = 0;
    D_800E8E88[1] = 0;
    D_800C9860 = 0;
    if (D_800EFD64->mode == 2) {
        v = D_800D0F78;
    } else {
        v = func_8005B58C(D_800D14EC);
    }
    if (v < 0) {
        D_800C9860 = 0;
        return;
    }
    a = 0;
    D_800C9860 = func_800BF79C(D_800E7728, v);
    for (; a < D_800C9860->n; a++) {
        D_800C9860->e[a].flags &= ~0x18;
    }
    for (j = 0; j < 2; j++) {
        D_800E8E88[j] = func_800BC908(0x800, 0, D_800D6C80, 0xCB);
        for (n = 0; n < 0x80; n++) {
            D_800E8E88[j][n].v = 1000;
        }
    }
    D_800E8E80 = func_800BF79C(D_800E7728, D_800C9860->f0);
    for (a = 0; a < D_800E8E80->n; a++) {
        n = D_800E8E80->it[a].w * D_800E8E80->it[a].h * 8;
        for (k = 0; k < n; k++) {
            D_800E8E80->it[a].buf[k] |= 0xF0;
        }
    }
    if (D_800EFD64->flags & 0x20) {
        D_800E8E98 = func_800BF79C(D_800E7728, D_800D0E00);
        D_800C9840 = func_800BC908(0x300, 0, D_800D6C80, 0xE3);
        for (i = 0; i < 0x40; i++) {
            x = (func_8000EFE8() - 0x80) * 16;
            y = (func_8000EFE8() - 0x80) * 16;
            D_800C9840[i].d = func_8000EB18(0x400000 - (x * x + y * y));
            D_800C9840[i].x = x;
            D_800C9840[i].y = y;
        }
    }
    D_800E8E9C = 0;
}
