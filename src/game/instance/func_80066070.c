#include "ctx.h"

typedef struct {
    u8 pad[0x1C];
    u32 flags;
} ObjInfo;

typedef struct {
    s32 hdr;
    ObjInfo e[1];
} ObjTable;

typedef struct {
    s32 count;
    u32 n;
} ObjList;

typedef struct {
    u8 pad[0xF];
    u8 dl;
} ObjInst;

extern Mtx *D_800CCCD0;
extern Mtx **D_800E77D4;
extern Gfx **D_800F1424;
extern s32 *D_800F142C;
extern ObjList *D_800F1430;
extern ObjTable *D_800F1434;
extern ObjInst *D_800F143C;

Gfx *func_80066070(Gfx *gdl, Mtx *m, s32 unused, s32 view, s32 vis)
{
    u32 i;
    s32 key;
    ObjInfo *o;
    s32 off;

    if (D_800CCCD0 == 0) {
        return gdl;
    }
    i = 0;
    while (i < D_800F1430->n) {
        key = D_800F142C[i];
        o = &D_800F1434->e[key];
        if (((o->flags >> (view * 2)) & 3) != vis) {
            while (D_800F142C[i] == key && i < D_800F1430->n) {
                i++;
            }
        } else {
            gSPMatrix(gdl++, D_800E77D4[key * 4 + view], G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            while (D_800F142C[i] == key && i < D_800F1430->n) {
                gSPMatrix(gdl++, &D_800CCCD0[i], G_MTX_PUSH | G_MTX_MUL | G_MTX_MODELVIEW);
                gSPDisplayList(gdl++, D_800F1424[D_800F143C[i].dl]);
                gSPPopMatrix(gdl++, G_MTX_MODELVIEW);
                i++;
            }
        }
    }
    gSPMatrix(gdl++, &m[16], G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    return gdl;
}
