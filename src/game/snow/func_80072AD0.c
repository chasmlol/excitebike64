#include "ctx.h"

typedef struct {
    u32 pad0;
    s32 type;
    u32 flags;
    s32 index;
    s32 count;
    u16 *list;
} SubEnt;

typedef struct {
    u32 flags;
    u8 pad[0x1C];
} Rec;

typedef struct {
    u32 n;
    u8 pad[0x1C];
    Rec r[1];
} Mdl2;

typedef struct {
    u8 pad[0x18];
    s32 n;
} Obj2;

extern s32 D_800F21B0;
extern SubEnt D_800F21B8[];
extern u8 *D_800C9800;
extern u8 D_800D91A0[];
void *func_800BC908(s32 size, u32 flags, s32 owner, s32 tag);

void func_80072AD0(Mdl2 *m, Obj2 *o)
{
    s32 i, j, k;
    SubEnt *e;

    D_800F21B0 = 0;
    for (i = 0; i < m->n; i++) {
        if (m->r[i].flags & 6) {
            D_800F21B8[D_800F21B0].index = i;
            D_800F21B8[D_800F21B0].flags = m->r[i].flags;
            D_800F21B8[D_800F21B0++].type = (m->r[i].flags >> 1) & 3;
        }
    }
    if (D_800F21B0 == 0) return;
    for (i = 0; i < D_800F21B0; i++) {
        D_800F21B8[i].count = 0;
    }
    for (k = 0; k < o->n; k++) {
        i = (D_800C9800[k] >> 3) & 3;
        if (i) {
            for (j = 0; j < D_800F21B0; j++) {
                if (D_800F21B8[j].type == i) break;
            }
            D_800F21B8[j].count++;
        }
    }
    for (i = 0; i < D_800F21B0; i++) {
        if (D_800F21B8[i].count) {
            D_800F21B8[i].list = func_800BC908(D_800F21B8[i].count * 2, 0, (s32)D_800D91A0, 0x5D);
            D_800F21B8[i].count = 0;
        }
    }
    for (k = 0; k < o->n; k++) {
        i = (D_800C9800[k] >> 3) & 3;
        if (i) {
            for (j = 0; j < D_800F21B0; j++) {
                if (D_800F21B8[j].type == i) break;
            }
            D_800F21B8[j].list[D_800F21B8[j].count++] = k;
        }
    }
}
