#include "game.h"

typedef struct {
    s16 x, y, z;
    s16 pad6;
    s32 f8, fC;
} MVtx; /* 0x10 */

typedef struct {
    u32 n;
    MVtx *v;
} Mesh_6;

typedef struct {
    Mesh_6 *mesh;
    s32 f4, f8, fC, f10;
    u8 pad14[0xC];
} Ent_68; /* 0x20 */

typedef struct {
    s16 qx, qy, qz;
    s16 px, py, pz;
    s16 padC[3];
    s16 x[3];
    s16 y[3];
    s16 z[3];
    s16 pad24[4];
} Tri3_3; /* 0x2C */

typedef struct {
    Ent_68 *ent;
    s32 n;
    s32 f8;
    s32 fC, f10, f14, f18;
    s32 min, max;
    s32 dx, dz;
    Tri3_3 *tri;
} Seg_15; /* 0x30 */

extern Seg_15 *sShatterObjects;

struct Tri3_2 {
    u8 pad0[6];
    s16 px, py, pz;
    u8 padC[6];
    s16 x[3];
    s16 y[3];
    s16 z[3];
};

s32 shatter_tri_longest_edge(Tri3_2 *t, s32 *outI, s32 *outN, s32 *ox, s32 *oy, s32 *oz)
{
    s32 i, n, d, best, bi;
    s32 dx, dy, dz;

    for (i = 0; i < 3; i++) {
        n = (i + 1) % 3;
        dx = t->x[n] - t->x[i];
        dy = t->y[n] - t->y[i];
        dz = t->z[n] - t->z[i];
        d = dx * dx + dy * dy + dz * dz;
        if (d >= best || i == 0) {
            best = d;
            bi = i;
        }
    }
    n = (bi + 1) % 3;
    *ox = (t->x[bi] + t->x[n]) / 2 + t->px;
    *oy = (t->y[bi] + t->y[n]) / 2 + t->py;
    *oz = (t->z[bi] + t->z[n]) / 2 + t->pz;
    *outI = bi;
    *outN = n;
    return 0;
}

struct Mdl_2 {
    u32 n;
    Ent_68 e[1];
};

void shatter_init(Mdl_2 *p)
{
    s32 x, y, z;
    s32 a, b;
    s32 i;
    s32 j, k, m, cnt;
    s32 vy;
    Seg_15 *s;
    Seg_15 *sp;
    Mesh_6 *me;
    Ent_68 *e;
    Tri3_3 *t;

    j = 0;
    sShatterModelList = p;
    for (i = 0; i < p->n; i++) {
        if (gObjectFlags[i] & 1) {
            j++;
        }
    }
    sShatterCount = j;
    if (j == 0) {
        return;
    }
    sp = sShatterObjects = mem_alloc_dbg(j * 0x30, 0, (s32)D_800D8D60, 0x74);
    j = 0;
    for (i = 0; i < p->n; i++) {
        if (gObjectFlags[i] & 1) {
            sp[j++].ent = &p->e[i];
        }
    }
    for (i = 0; i < sShatterCount; i++) {
        s = &sShatterObjects[i];
        s->fC = s->ent->f4;
        e = s->ent;
        s->f10 = e->fC;
        s->f14 = e->f8;
        s->f18 = e->f10;
        x = s->f18 - s->f14;
        z = s->fC - s->f10;
        y = isqrt_u32_2(x * x + z * z) + 1;
        s->min = 999999;
        s->max = -999999;
        s->dx = (x << 14) / y;
        s->dz = (z << 14) / y;
    }
    for (i = 0; i < sShatterCount; i++) {
        s = &sShatterObjects[i];
        me = s->ent->mesh;
        s->n = me->n / 3;
        s->tri = mem_alloc_dbg(s->n * 0x2C, 0, (s32)D_800D8D60, 0x96);
        for (j = 0; j < s->n; j++) {
            x = y = z = 0;
            for (a = 0; a < 3; a++) {
                vy = me->v[j * 3 + a].y;
                x += me->v[j * 3 + a].x;
                y += vy;
                z += me->v[j * 3 + a].z;
                if (s->max < vy) {
                    s->max = vy;
                }
                if (vy < s->min) {
                    s->min = vy;
                }
            }
            x = (x * 16) / 3;
            y = (y * 16) / 3;
            z = (z * 16) / 3;
            s->tri[j].px = x;
            s->tri[j].py = y;
            s->tri[j].pz = z;
            s->tri[j].qx = s->tri[j].px;
            s->tri[j].qy = s->tri[j].py;
            s->tri[j].qz = s->tri[j].pz;
            for (a = 0; a < 3; a++) {
                s->tri[j].x[a] = me->v[j * 3 + a].x * 16 - x;
                s->tri[j].y[a] = me->v[j * 3 + a].y * 16 - y;
                s->tri[j].z[a] = me->v[j * 3 + a].z * 16 - z;
            }
        }
        s->f8 = -1;
    }
    for (i = 0; i < sShatterCount; i++) {
        s = &sShatterObjects[i];
        for (j = 0; j < s->n; j += cnt) {
            t = &s->tri[j];
            if (shatter_tri_longest_edge(t, &a, &b, &x, &y, &z) == 0) {
                k = j + 1;
                for (m = k; m < s->n; m++) {
                    s32 x2, y2, z2;
                    Tri3_3 *u = &s->tri[m];
                    if (shatter_tri_longest_edge(u, &a, &b, &x2, &y2, &z2) == 0 &&
                        x == x2 && y == y2 && z == z2) {
                        __builtin_memcpy(gZBuffer, u, sizeof(Tri3_3));
                        __builtin_memcpy(u, &s->tri[k], sizeof(Tri3_3));
                        __builtin_memcpy(&s->tri[k], gZBuffer, sizeof(Tri3_3));
                        __builtin_memcpy(gZBuffer, &s->ent->mesh->v[m * 3], 0x30);
                        __builtin_memcpy(&s->ent->mesh->v[m * 3], &s->ent->mesh->v[k * 3], 0x30);
                        __builtin_memcpy(&s->ent->mesh->v[k * 3], gZBuffer, 0x30);
                        k++;
                    }
                }
                cnt = k - j;
                if (cnt != 1) {
                    x = 0;
                    y = 0;
                    z = 0;
                    for (a = 0; a < cnt; a++) {
                        x += t[a].px;
                        y += t[a].py;
                        z += t[a].pz;
                    }
                    x /= cnt;
                    y /= cnt;
                    z /= cnt;
                    for (a = 0; a < cnt; a++) {
                        for (b = 0; b < 3; b++) {
                            t[a].x[b] += t[a].px;
                            t[a].y[b] += t[a].py;
                            t[a].z[b] += t[a].pz;
                        }
                        t[a].qx = t[a].px = x;
                        t[a].qy = t[a].py = y;
                        t[a].qz = t[a].pz = z;
                        for (b = 0; b < 3; b++) {
                            t[a].x[b] -= t[a].px;
                            t[a].y[b] -= t[a].py;
                            t[a].z[b] -= t[a].pz;
                        }
                    }
                }
            }
        }
    }
}

typedef struct {
    s16 pad0[3];
    s16 x, y, z;
    s16 vx, vy, vz;
    s16 m[3][3];
    s16 rx, ry, rz;
    s16 cnt;
} Part_5;

typedef struct {
    s32 pad0;
    Vtx *vtx;
} Mesh_7;

typedef struct {
    Mesh_7 *mesh;
    s32 a, b, c, d;
} Model_6;

typedef struct {
    Model_6 *model;
    s32 n;
    s32 t;
    u8 pad[0x2C - 0xC];
    Part_5 *parts;
} Rec_38;

void shatter_update_pieces(void)
{
    s32 mtx[10];
    s32 in[4];
    s32 out[4];
    s32 i, j, k;
    Rec_38 *rec;
    Model_6 *mdl;
    Part_5 *p;
    Vtx *v;
    s32 ox, oz, h;
    s16 z;

    for (i = 0; i < sShatterCount; i++) {
        rec = &sShatterObjects[i];
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
                k = height_get_ground(p->x + ox, p->y + 500, p->z + oz);
                if (p->y < k) {
                    p->y = k;
                    p->vy = -(p->vy * 3) / 4;
                    if (--p->cnt == 0) p->y = k - 5000;
                }
            }
            p->vy -= 2;
            mtx3i_from_angles(mtx, p->rx * rec->t, p->ry * rec->t, p->rz * rec->t);
            for (k = 0; k < 3; k++) {
                in[0] = p->m[0][k];
                in[1] = p->m[1][k];
                in[2] = p->m[2][k];
                vec3i_mul_mtx3i(in, mtx, out);
                v[k].v.ob[0] = (out[0] + p->x) >> 4;
                v[k].v.ob[1] = (out[1] + p->y) >> 4;
                v[k].v.ob[2] = (out[2] + p->z) >> 4;
            }
        }
        if (++rec->t >= 0xB5) {
            h = (s32)rec->model - 4;
            j = (h - (s32)sShatterModelList) >> 5;
            gObjectFlags[j] |= 8;
        }
    }
    osWritebackDCache(0, 0x800000);
}

typedef struct {
    u8 pad0[0x48];
    f32 f48, f4c, f50, f54, f58, f5c;
    u8 pad1[0x7A5 - 0x60];
    u8 flag;
    u8 pad2[0x7E0 - 0x7A6];
} Ent_69;

typedef struct {
    u8 pad0[0xC];
    s16 c0, c1, c2;
    u8 pad1[0x24 - 0x12];
    s16 d0, d1, d2;
    s16 st;
} Item_7;

typedef struct {
    s32 f0;
    s32 n;
    s32 flag;
    s32 f0c, f10, f14, f18, f1c, f20, f24, f28;
    Item_7 *list;
} Elem_3;

extern Ent_69 gRiders_85[8];

void shatter_check_riders(void)
{
    s32 h, i, j, u, sel, x, y, z, dx, dy;
    Ent_69 *s;
    Elem_3 *e;
    Item_7 *it;

    for (i = 0; i < 8; i++) {
        s = &gRiders_85[i];
        if (s->flag != 0) continue;
        for (j = 0; j < sShatterCount; j++) {
            e = &sShatterObjects[j];
            if (e->flag < 0 && (h = e->f0 - 4, !(gObjectFlags[(h - gTrackObjects_3) >> 5] & 6))) {
                x = (s32)(s->f48 * 1000.0f) >> 4;
                y = (s32)(s->f50 * 1000.0f) >> 4;
                u = (x - e->f0c) * e->f24 + (y - e->f14) * e->f28;
                u >>= 14;
                z = (s32)(s->f4c * 1000.0f) >> 4;
                if (e->f10 - e->f0c < 30 && e->f18 - e->f14 < 30) {
                    sel = (u32)(u + 30) < 61;
                } else {
                    sel = (u32)(u + 100) < 201;
                }
                if (sel) {
                    u = e->f10 - e->f0c;
                    if (u < 0) u = -u;
                    dy = e->f18 - e->f14;
                    if (dy < 0) dy = -dy;
                    if (dy < u) {
                        if (x < e->f10 && x < e->f0c) continue;
                        if (e->f10 < x && e->f0c < x) continue;
                    } else {
                        if (y < e->f18 && y < e->f14) continue;
                        if (e->f18 < y && e->f14 < y) continue;
                    }
                    if (z < e->f1c - 50) continue;
                    if (e->f20 + 50 < z) continue;
                    snd_play_rider_sfx(4, i, 0xFF);
                    e->flag = 0;
                    for (u = 0; u < e->n; u++) {
                        it = &e->list[u];
                        if (u & 1) {
                            it->st = 3;
                            it->c0 = it[-1].c0;
                            it->c1 = it[-1].c1;
                            it->c2 = it[-1].c2;
                            it->d0 = it[-1].d0;
                            it->d1 = it[-1].d1;
                            it->d2 = it[-1].d2;
                        } else {
                            it->c0 = (s32)((s->f54 * 1000.0f) / 60.0f);
                            it->c1 = (s32)((s->f58 * 1000.0f) / 60.0f);
                            it->c2 = (s32)((s->f5c * 1000.0f) / 60.0f);
                            it->d0 = (random_byte() - 0x80) * 4;
                            it->d1 = (random_byte() - 0x80) * 5;
                            it->d2 = (random_byte() - 0x80) * 2;
                            it->st = 3;
                        }
                    }
                }
            }
        }
    }
}

void shatter_break_object(s32 param_1) {
    s32 *ent;
    s32 *p;
    s32 cnt;
    s32 key;
    s32 i;
    s32 uVar5;
    s32 off;
    u8 *rec;
    s32 t;
    s32 n2;

    t = sShatterCount;
    i = 0;
    if (t > 0) {
        cnt = t;
        p = sShatterObjects;
        key = (s32)(gTrackObjects + (param_1 * 8 + 1));
    loop:
        ent = p;
        if (*ent == key) {
            goto chk;
        }
        i++;
        p = ent + 0xc;
        if (i < cnt) {
            goto loop;
        }
    }
chk:
    uVar5 = 0;
    if (i != sShatterCount) {
        ent[2] = 0;
        if (ent[1] > 0) {
            off = 0;
            do {
                rec = (u8 *)ent[11] + off;
                *(u16 *)(rec + 0x2A) = 3;
                if ((uVar5 & 1) != 0) {
                    *(u16 *)(rec + 0xC) = *(u16 *)(rec - 0x20);
                    *(u16 *)(rec + 0xE) = *(u16 *)(rec - 0x1E);
                    *(u16 *)(rec + 0x10) = *(u16 *)(rec - 0x1C);
                    *(u16 *)(rec + 0x24) = *(u16 *)(rec - 0x8);
                    *(u16 *)(rec + 0x26) = *(u16 *)(rec - 0x6);
                    *(u16 *)(rec + 0x28) = *(u16 *)(rec - 0x4);
                } else {
                    *(u16 *)(rec + 0xC) = 0;
                    *(u16 *)(rec + 0xE) = 0;
                    *(u16 *)(rec + 0x10) = 0;
*(u16 *)(rec + 0xC) = *(u16 *)(rec + 0xC) + ((s32)random_byte() - 0x80) / 2;
*(u16 *)(rec + 0xE) = *(u16 *)(rec + 0xE) + (((s32)random_byte() - 0x80) >> 2) / 2;
*(u16 *)(rec + 0x10) = *(u16 *)(rec + 0x10) + ((s32)random_byte() - 0x80) / 2;
*(u16 *)(rec + 0x24) = ((s32)random_byte() - 0x80) * 4;
*(u16 *)(rec + 0x26) = ((s32)random_byte() - 0x80) * 5;
*(u16 *)(rec + 0x28) = ((s32)random_byte() - 0x80) * 2;
                }
                n2 = ent[1];
                off += 0x2C;
            } while ((s32)(++uVar5) < n2);
        }
    }
}

typedef struct Vt {
    s16 ob[3];
    u16 flag;
    s16 tc[2];
    u8 cn[4];
} Vt;

typedef struct {
    s16 o[3];
    s16 p[3];
    s16 pad[3];
    s16 x[3];
    s16 y[3];
    s16 z[3];
    s16 pad2[4];
} Src_2;

typedef struct {
    s32 pad;
    Vt *v;
} Obj_49;

typedef struct {
    Obj_49 **obj;
    s32 count;
    s32 flag;
    s32 pad[8];
    Src_2 *src;
} Rec_39;

void shatter_reset(void)
{
    s32 n;
    s32 nt;
    s32 m;
    Rec_39 *base;
    s32 i, j, k;
    Rec_39 *r;
    Src_2 *s;
    Vt *v;

    nt = sShatterCount;
    i = 0;
    if (nt > 0) {
    m = -1;
    n = nt;
    base = sShatterObjects;
    do {
        r = &base[i];
        s = r->src;
        r->flag = m;
        v = (*r->obj)->v;
        for (j = 0; j < r->count; j++, s++, v += 3) {
            s->p[0] = s->o[0];
            s->p[1] = s->o[1];
            s->p[2] = s->o[2];
            for (k = 0; k < 3; k++) {
                v[k].ob[0] = (s->x[k] + s->p[0]) >> 4;
                v[k].ob[1] = (s->y[k] + s->p[1]) >> 4;
                v[k].ob[2] = (s->z[k] + s->p[2]) >> 4;
            }
        }
        i++;
    } while (i < n);
    }
    osWritebackDCache(0, 0x800000);
}

/* struct stride 0x30, field w at 0x2c */
typedef struct {
    s32 pad[11];
    s32 w;
} Ent30;

extern Ent30 *sShatterObjects_2;

void shatter_free(void)
{
    s32 i;

    if (sShatterCount != 0) {
        for (i = 0; i < sShatterCount; i++) {
            mem_free(sShatterObjects_2[i].w);
        }
        mem_free((s32)sShatterObjects_2);
        sShatterCount = 0;
    }
}

void shatter_update(void)
{
    shatter_check_riders();
    shatter_update_pieces();
}
