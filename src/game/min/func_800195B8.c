#include "ctx.h"

typedef struct { u8 b[12]; } B12;

typedef struct {
    char name[0x10];
    s32 v[6];
    s32 f28;
    B12 b;
    s32 key;
} Src;

typedef struct {
    char name[0x10];
    f32 v[6];
    s32 f28;
    B12 b;
    s32 f38;
    s32 pad3C;
} Dst;

typedef struct {
    s32 n;
    Src e[1];
} File;

typedef struct {
    s32 key;
    s32 val;
} Pair;

typedef struct {
    s32 f0;
    u32 *arr;
} Res;

extern Res *D_800D5604;
extern s32 D_800D123C;
extern Dst D_800C9080[];

void *func_800BF79C(Res *r, s32 idx);
void func_8000EF8C(char *a0, char *a1);
void func_800BFEA4(u32 *p);

void func_800195B8(Res *r, s32 idx)
{
    Pair *tab;
    File *f;
    s32 i, j;

    tab = func_800BF79C(D_800D5604, D_800D123C);
    f = func_800BF79C(r, idx);
    for (i = 0; i < 16; i++) {
        func_8000EF8C(D_800C9080[i].name, f->e[i].name);
        for (j = 0; j < 6; j++) {
            ((f32 *)&D_800C9080[i])[j + 4] = (f32)f->e[i].v[j] / 1000.0f;
        }
        D_800C9080[i].f28 = f->e[i].f28;
        D_800C9080[i].b.b[0] = f->e[i].b.b[0];
        D_800C9080[i].b.b[1] = f->e[i].b.b[1];
        D_800C9080[i].b.b[2] = f->e[i].b.b[2];
        D_800C9080[i].b.b[3] = f->e[i].b.b[3];
        D_800C9080[i].b.b[4] = f->e[i].b.b[4];
        D_800C9080[i].b.b[5] = f->e[i].b.b[5];
        D_800C9080[i].b.b[6] = f->e[i].b.b[6];
        D_800C9080[i].b.b[7] = f->e[i].b.b[7];
        D_800C9080[i].b.b[8] = f->e[i].b.b[8];
        D_800C9080[i].b.b[9] = f->e[i].b.b[9];
        D_800C9080[i].b.b[10] = f->e[i].b.b[10];
        D_800C9080[i].b.b[11] = f->e[i].b.b[11];
        for (j = 0; tab[j].key != 0; j++) {
            if (tab[j].key == f->e[i].key) {
                break;
            }
        }
        D_800C9080[i].f38 = tab[j].val;
    }
    func_800BFEA4(&r->arr[idx]);
    func_800BFEA4(&D_800D5604->arr[D_800D123C]);
}
