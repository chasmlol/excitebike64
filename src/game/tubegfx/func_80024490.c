#include "ctx.h"

typedef struct {
    s16 x, y, z;
    u8 pad[6];
    u8 r, g, b;
    u8 pad2;
} V16;

typedef struct {
    u32 count;
    V16 *v;
} Mesh;

typedef struct {
    Mesh *mesh;
    s32 minA, minB, maxA, maxB;
    u8 pad[0xC];
} Item;

typedef struct {
    u32 count;
    Item items[1];
} Model;

extern u8 *D_800C29E0;
s32 func_80065858(void);
u32 func_8000EFE8(void);

void func_80024490(Model *m)
{
    s32 minB = 9999999;
    s32 minA = 9999999;
    s32 maxB = -9999999;
    s32 maxA = -9999999;
    s32 i, off, off2;
    u32 k;
    Item *it;
    u8 *buf;
    u8 *out;
    s32 dx, dy, ix, iy;
    Mesh *mesh;
    V16 *vt;
    u8 c;

    for (i = 0; i < m->count; i++) {
        it = (Item *)((u8 *)m + (i * 0x20 + 4));
        if (it->minA < minA) minA = it->minA;
        if (it->minB < minB) minB = it->minB;
        if (maxA < it->maxA) maxA = it->maxA;
        if (maxB < it->maxB) maxB = it->maxB;
    }
    while (func_80065858() != 0) {
    }
    buf = D_800C29E0;
    out = buf + 0x10000;
    for (i = 0; i < 0x10000; i++) {
        buf[i] = func_8000EFE8();
    }
    for (i = 0; i < 0x10000; i++) {
        out[i] = (buf[i] + buf[i + 0x100] + buf[i - 0x100] + buf[i + 1] + buf[i - 1]) / 5;
    }
    i = 0;
    if (m->count != 0) {
        dx = maxA - minA;
        dy = maxB - minB;
        off2 = 4;
        do {
            it = (Item *)((u8 *)m + off2);
            mesh = it->mesh;
            for (k = 0; k < mesh->count; k++) {
                vt = &mesh->v[k];
                ix = vt->x;
                ix = (ix - minA) * 64 / dx + 5;
                iy = vt->z;
                iy -= minB;
                iy = iy * 64 / dy + 5;
                c = out[(ix << 8) + iy];
                vt->r = c;
                vt->g = c;
                vt->b = c;
            }
            off2 += 0x20;
            i++;
        } while (i < m->count);
    }
}
