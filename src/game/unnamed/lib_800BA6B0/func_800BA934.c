#include "ctx.h"

typedef struct {
    s32 *data;
    s32 len;
    s32 b;
    s32 c;
    s32 d;
    s32 e;
    s32 f;
} Ent;

extern Ent D_800FE8F0[];
extern u8 D_800FE880[];
extern s32 D_800FE8DC;
extern char D_800DB170[];
extern char D_800DB1C0[];
extern char D_800DB1C8[];
extern char D_800DB1D0[];
void func_8000ED10(u8 *a0, u8 a1, s32 a2);
void func_800AEC60(u8 *a, char *b, s32 c);
void *func_800BE694(u8 *a, char *b);
s32 func_800BE848(u8 *param_1, s32 param_2, s32 param_3, s32 *param_4);
void func_800BE770(void *q);
s32 *func_800BC908(s32 a, s32 b, char *c, s32 d);

s32 func_800BA934(s32 param_1)
{
    s32 i;
    Ent *e;
    s32 *f;
    u8 buf[8];
    u8 *bp;

    for (i = 0; i < 0x100; i++) {
        if (D_800FE8F0[i].len == 0) break;
    }
    if (i == 0x100) return -1;
    func_8000ED10((u8 *)&D_800FE8F0[i], 0, 0x1c);
    if (D_800FE8DC & 2) {
        func_800AEC60(D_800FE880, D_800DB1C0, param_1);
    } else {
        func_800AEC60(D_800FE880, D_800DB1C8, param_1);
    }
    f = func_800BE694(D_800FE880, D_800DB1D0);
    do { do { do { do { do { do { bp = buf; } while (0); } while (0); } while (0); } while (0); } while (0); } while (0);
    if (f == 0) return -2;
    func_800BE848(bp, 1, 4, f);
    func_800BE848((u8 *)&D_800FE8F0[i].len, 1, 4, f);
    func_800BE848((u8 *)&D_800FE8F0[i].b, 1, 4, f);
    func_800BE848((u8 *)&D_800FE8F0[i].c, 1, 4, f);
    func_800BE848((u8 *)&D_800FE8F0[i].d, 1, 4, f);
    func_800BE848(bp, 1, 4, f);
    e = &D_800FE8F0[i];
    e->data = func_800BC908(e->len, 0, D_800DB170, 0x368);
    if (e->data == 0) {
        func_800BE770(f);
        return -4;
    }
    func_800BE848((u8 *)e->data, 1, e->len, f);
    func_800BE770(f);
    return i;
}
