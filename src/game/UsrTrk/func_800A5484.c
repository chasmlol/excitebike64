#include "ctx.h"

typedef struct {
    u32 nv;
    Vtx *v;
    u32 ntri;
    u32 *tri;
    s32 x10;
    s32 x14;
} Mesh;

typedef struct {
    Mesh *mesh;
    s32 a, b, c, d;
    s16 *tex;
    s32 e, f;
} Entry;

typedef struct {
    u32 n;
    Entry e[1];
} Set;

typedef struct {
    u8 pad[0x730];
    Set *s1;
    Set *s2;
} Glob;

extern Glob D_800CE280;
extern u8 D_800DB040[];

void *func_800BC908(s32 size, u32 flags, s32 owner, s32 tag);
void func_800AEC34(void *dst, void *src, s32 n);

void func_800A5484(Set *out1, Set *out2, s32 count)
{
    Set *set1;
    Set *set2;
    s32 j;
    u32 i;
    u32 n;
    s32 off;
    Entry *o;
    Entry *o2;
    s32 size;
    s32 size2;

    set1 = D_800CE280.s1;
    set2 = D_800CE280.s2;
    for (j = 0; j < count; j++) {
        for (i = 0; i < (n = set1->n); i++) {
            size = (set1->e[i].tex[1] << 4) | 4;
            o = &out1->e[j * n + i];
            o2 = &out2->e[j * set2->n + i];
            o->mesh = func_800BC908(sizeof(Mesh) + set1->e[i].mesh->nv * 16 + set1->e[i].mesh->ntri * 4 + size, 0, (s32)D_800DB040, 0x2F8);
            *o->mesh = *set1->e[i].mesh;
            o->e = set1->e[i].e;
            o->f = set1->e[i].f;
            o->a = set1->e[i].a;
            o->b = set1->e[i].b;
            o->c = set1->e[i].c;
            o->d = set1->e[i].d;
            o->mesh->nv = set1->e[i].mesh->nv;
            o->mesh->v = (Vtx *)(o->mesh + 1);
            o->mesh->tri = (u32 *)(o->mesh->v + o->mesh->nv);
            o->mesh->ntri = set1->e[i].mesh->ntri;
            o->tex = (s16 *)(o->mesh->tri + o->mesh->ntri);
            func_800AEC34(o->mesh->v, set1->e[i].mesh->v, o->mesh->nv * 16);
            func_800AEC34(o->mesh->tri, set1->e[i].mesh->tri, o->mesh->ntri * 4);
            func_800AEC34(o->tex, set1->e[i].tex, size);

            size2 = (set2->e[i].tex[1] << 4) | 4;
            o2->mesh = func_800BC908(sizeof(Mesh) + set2->e[i].mesh->nv * 16 + set2->e[i].mesh->ntri * 4 + size2, 0, (s32)D_800DB040, 0x30C);
            *o2->mesh = *set2->e[i].mesh;
            o2->e = set2->e[i].e;
            o2->f = set2->e[i].f;
            o2->a = set2->e[i].a;
            o2->b = set2->e[i].b + j * 500;
            o2->c = set2->e[i].c;
            o2->d = set2->e[i].d + j * 500;
            o2->mesh->nv = set2->e[i].mesh->nv;
            o2->mesh->v = (Vtx *)(o2->mesh + 1);
            o2->mesh->tri = (u32 *)(o2->mesh->v + o2->mesh->nv);
            o2->mesh->ntri = set2->e[i].mesh->ntri;
            o2->tex = (s16 *)(o2->mesh->tri + o2->mesh->ntri);
            func_800AEC34(o2->mesh->v, set2->e[i].mesh->v, o2->mesh->nv * 16);
            func_800AEC34(o2->mesh->tri, set2->e[i].mesh->tri, o2->mesh->ntri * 4);
            func_800AEC34(o2->tex, set2->e[i].tex, size2);
        }
    }
}
