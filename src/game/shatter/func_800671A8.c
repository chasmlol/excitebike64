#include "ctx.h"

typedef struct {
    s16 pad0[3];
    s16 x, y, z;
    s16 vx, vy, vz;
    s16 m[3][3];
    s16 rx, ry, rz;
    s16 cnt;
} Part;

typedef struct {
    s32 pad0;
    Vtx *vtx;
} Mesh;

typedef struct {
    Mesh *mesh;
    s32 a, b, c, d;
} Model;

typedef struct {
    s32 pad0;
    Model m;
    u8 pad[0x20 - 4 - sizeof(Model)];
} Obj;

typedef struct {
    Model *model;
    s32 n;
    s32 t;
    u8 pad[0x2C - 0xC];
    Part *parts;
} Rec;

extern s32 D_800CCCE0;
extern Rec *D_800F1444;
extern Obj *D_800F1440;
extern u32 *D_800E7834;

void func_80011A58(s32 *param_1, s32 param_2, s32 param_3, s32 param_4);
void func_80012DC8(s32 *param_1, s32 *param_2, s32 *param_3);
s32 func_8002E920(s32 x, s32 y, s32 z);
void func_800AD910(s32 a, s32 b);

void func_800671A8(void)
{
    s32 mtx[10];
    s32 in[4];
    s32 out[4];
    s32 i, j, k;
    Rec *rec;
    Model *mdl;
    Part *p;
    Vtx *v;
    s32 ox, oz, h;
    s16 z;

    for (i = 0; i < D_800CCCE0; i++) {
        rec = &D_800F1444[i];
        if (rec->t < 0) continue;
        if (rec->t >= 0xB5) continue;
        mdl = rec->model;
        p = rec->parts;
        ox = (mdl->a + mdl->c) * 8;
        v = mdl->mesh->vtx;
        oz = (mdl->b + mdl->d) * 8;
        for (j = 0; j < rec->n; j++, p++, v += 3) {
            if (p->cnt == 0) continue;
            p->x += p->vx;
            p->y += p->vy;
            p->z += p->vz;
            if (rec->t > 0) {
                k = func_8002E920(p->x + ox, p->y + 500, p->z + oz);
                if (p->y < k) {
                    p->y = k;
                    p->vy = -(p->vy * 3) / 4;
                    if (--p->cnt == 0) p->y = k - 5000;
                }
            }
            p->vy -= 2;
            func_80011A58(mtx, p->rx * rec->t, p->ry * rec->t, p->rz * rec->t);
            for (k = 0; k < 3; k++) {
                in[0] = p->m[0][k];
                in[1] = p->m[1][k];
                in[2] = p->m[2][k];
                func_80012DC8(in, mtx, out);
                v[k].v.ob[0] = (out[0] + p->x) >> 4;
                v[k].v.ob[1] = (out[1] + p->y) >> 4;
                v[k].v.ob[2] = (out[2] + p->z) >> 4;
            }
        }
        if (++rec->t >= 0xB5) {
            h = (s32)rec->model - 4;
            j = (h - (s32)D_800F1440) >> 5;
            D_800E7834[j] |= 8;
        }
    }
    func_800AD910(0, 0x800000);
}
