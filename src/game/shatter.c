#include "game.h"

/* The midpoint of the piece's longest edge (relative to the object's box centre). */
s32 shatter_tri_longest_edge(ShatterPiece *t, s32 *outI, s32 *outN, s32 *ox, s32 *oy, s32 *oz)
{
    s32 i, n, d, best, bi;
    s32 dx, dy, dz;

    for (i = 0; i < 3; i++) {
        n = (i + 1) % 3;
        dx = t->cx[n] - t->cx[i];
        dy = t->cy[n] - t->cy[i];
        dz = t->cz[n] - t->cz[i];
        d = dx * dx + dy * dy + dz * dz;
        if (d >= best || i == 0) {
            best = d;
            bi = i;
        }
    }
    n = (bi + 1) % 3;
    *ox = (t->cx[bi] + t->cx[n]) / 2 + t->x;
    *oy = (t->cy[bi] + t->cy[n]) / 2 + t->y;
    *oz = (t->cz[bi] + t->cz[n]) / 2 + t->z;
    *outI = bi;
    *outN = n;
    return 0;
}

/* Splits every breakable object of the list into pieces, one per triangle of its model (whose vertices are three
   per triangle), and groups the triangles that share their longest edge's midpoint so they fly off together. */
void shatter_init(TrackModelList *p)
{
    s32 x, y, z;
    s32 a, b;
    s32 i;
    s32 j, k, m, cnt;
    s32 vy;
    ShatterObj *s;
    ShatterObj *sp;
    PrimModel *me;
    TrackModel *e;
    ShatterPiece *t;

    j = 0;
    sShatterModelList = p;
    for (i = 0; i < p->count; i++) {
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
    for (i = 0; i < p->count; i++) {
        if (gObjectFlags[i] & 1) {
            sp[j++].obj = &p->e[i];
        }
    }
    for (i = 0; i < sShatterCount; i++) {
        s = &sShatterObjects[i];
        s->x0 = s->obj->x0;
        e = s->obj;
        s->x1 = e->x1;
        s->z0 = e->z0;
        s->z1 = e->z1;
        x = s->z1 - s->z0;
        z = s->x0 - s->x1;
        y = isqrt_u32_2(x * x + z * z) + 1;
        s->minY = 999999;
        s->maxY = -999999;
        s->nx = (x << 14) / y;
        s->nz = (z << 14) / y;
    }
    for (i = 0; i < sShatterCount; i++) {
        s = &sShatterObjects[i];
        me = s->obj->model;
        s->numPieces = me->numVtx / 3;
        s->pieces = mem_alloc_dbg(s->numPieces * 0x2C, 0, (s32)D_800D8D60, 0x96);
        for (j = 0; j < s->numPieces; j++) {
            x = y = z = 0;
            for (a = 0; a < 3; a++) {
                vy = me->vtx[j * 3 + a].v.ob[1];
                x += me->vtx[j * 3 + a].v.ob[0];
                y += vy;
                z += me->vtx[j * 3 + a].v.ob[2];
                if (s->maxY < vy) {
                    s->maxY = vy;
                }
                if (vy < s->minY) {
                    s->minY = vy;
                }
            }
            x = (x * 16) / 3;
            y = (y * 16) / 3;
            z = (z * 16) / 3;
            s->pieces[j].x = x;
            s->pieces[j].y = y;
            s->pieces[j].z = z;
            s->pieces[j].homeX = s->pieces[j].x;
            s->pieces[j].homeY = s->pieces[j].y;
            s->pieces[j].homeZ = s->pieces[j].z;
            for (a = 0; a < 3; a++) {
                s->pieces[j].cx[a] = me->vtx[j * 3 + a].v.ob[0] * 16 - x;
                s->pieces[j].cy[a] = me->vtx[j * 3 + a].v.ob[1] * 16 - y;
                s->pieces[j].cz[a] = me->vtx[j * 3 + a].v.ob[2] * 16 - z;
            }
        }
        s->timer = -1;
    }
    for (i = 0; i < sShatterCount; i++) {
        s = &sShatterObjects[i];
        for (j = 0; j < s->numPieces; j += cnt) {
            t = &s->pieces[j];
            if (shatter_tri_longest_edge(t, &a, &b, &x, &y, &z) == 0) {
                k = j + 1;
                for (m = k; m < s->numPieces; m++) {
                    s32 x2, y2, z2;
                    ShatterPiece *u = &s->pieces[m];
                    if (shatter_tri_longest_edge(u, &a, &b, &x2, &y2, &z2) == 0 &&
                        x == x2 && y == y2 && z == z2) {
                        __builtin_memcpy(gZBuffer, u, sizeof(ShatterPiece));
                        __builtin_memcpy(u, &s->pieces[k], sizeof(ShatterPiece));
                        __builtin_memcpy(&s->pieces[k], gZBuffer, sizeof(ShatterPiece));
                        __builtin_memcpy(gZBuffer, &s->obj->model->vtx[m * 3], 0x30);
                        __builtin_memcpy(&s->obj->model->vtx[m * 3], &s->obj->model->vtx[k * 3], 0x30);
                        __builtin_memcpy(&s->obj->model->vtx[k * 3], gZBuffer, 0x30);
                        k++;
                    }
                }
                cnt = k - j;
                if (cnt != 1) {
                    x = 0;
                    y = 0;
                    z = 0;
                    for (a = 0; a < cnt; a++) {
                        x += t[a].x;
                        y += t[a].y;
                        z += t[a].z;
                    }
                    x /= cnt;
                    y /= cnt;
                    z /= cnt;
                    for (a = 0; a < cnt; a++) {
                        for (b = 0; b < 3; b++) {
                            t[a].cx[b] += t[a].x;
                            t[a].cy[b] += t[a].y;
                            t[a].cz[b] += t[a].z;
                        }
                        t[a].homeX = t[a].x = x;
                        t[a].homeY = t[a].y = y;
                        t[a].homeZ = t[a].z = z;
                        for (b = 0; b < 3; b++) {
                            t[a].cx[b] -= t[a].x;
                            t[a].cy[b] -= t[a].y;
                            t[a].cz[b] -= t[a].z;
                        }
                    }
                }
            }
        }
    }
}

/* Moves the pieces of the broken objects: velocity, gravity, bounces on the ground, spin. */
void shatter_update_pieces(void)
{
    s32 mtx[10];
    s32 in[4];
    s32 out[4];
    s32 i, j, k;
    ShatterObj *rec;
    TrackModel *mdl;
    ShatterPiece *p;
    Vtx *v;
    s32 ox, oz, h;
    s16 z;

    for (i = 0; i < sShatterCount; i++) {
        rec = &sShatterObjects[i];
        if (rec->timer < 0) continue;
        if (rec->timer >= 0xB5) continue;
        mdl = rec->obj;
        p = rec->pieces;
        ox = (mdl->x0 + mdl->x1) * 8;
        v = mdl->model->vtx;
        oz = (mdl->z0 + mdl->z1) * 8;
        for (j = 0; j < rec->numPieces; j++, p++, v += 3) {
            if (p->bounces == 0) continue;
            p->x += p->vx;
            p->y += p->vy;
            p->z += p->vz;
            if (rec->timer > 0) {
                k = height_get_ground(p->x + ox, p->y + 500, p->z + oz);
                if (p->y < k) {
                    p->y = k;
                    p->vy = -(p->vy * 3) / 4;
                    if (--p->bounces == 0) p->y = k - 5000;
                }
            }
            p->vy -= 2;
            mtx3i_from_angles(mtx, p->rx * rec->timer, p->ry * rec->timer, p->rz * rec->timer);
            for (k = 0; k < 3; k++) {
                in[0] = p->cx[k];
                in[1] = p->cy[k];
                in[2] = p->cz[k];
                vec3i_mul_mtx3i(in, mtx, out);
                v[k].v.ob[0] = (out[0] + p->x) >> 4;
                v[k].v.ob[1] = (out[1] + p->y) >> 4;
                v[k].v.ob[2] = (out[2] + p->z) >> 4;
            }
        }
        if (++rec->timer >= 0xB5) {
            /* hide the object: its index in the list */
            h = (s32)rec->obj - 4;
            j = (h - (s32)sShatterModelList) >> 5;
            gObjectFlags[j] |= 8;
        }
    }
    osWritebackDCache(0, 0x800000);
}

/* Breaks a whole object (not one removed with walls) when a rider crosses its box's diagonal within its height. */
void shatter_check_riders(void)
{
    s32 h, i, j, u, sel, x, y, z, dx, dy;
    Rider *s;
    ShatterObj *e;
    ShatterPiece *it;

    for (i = 0; i < 8; i++) {
        s = &gRiders[i];
        if (s->inactive != 0) continue;
        for (j = 0; j < sShatterCount; j++) {
            e = &sShatterObjects[j];
            if (e->timer < 0 && (h = (s32)e->obj - 4, !(gObjectFlags[(h - (s32)gTrackObjects) >> 5] & 6))) {
                x = (s32)(s->bike.pos.x * 1000.0f) >> 4;
                y = (s32)(s->bike.pos.z * 1000.0f) >> 4;
                u = (x - e->x0) * e->nx + (y - e->z0) * e->nz;
                u >>= 14;
                z = (s32)(s->bike.pos.y * 1000.0f) >> 4;
                if (e->x1 - e->x0 < 30 && e->z1 - e->z0 < 30) {
                    sel = (u32)(u + 30) < 61;
                } else {
                    sel = (u32)(u + 100) < 201;
                }
                if (sel) {
                    u = e->x1 - e->x0;
                    if (u < 0) u = -u;
                    dy = e->z1 - e->z0;
                    if (dy < 0) dy = -dy;
                    if (dy < u) {
                        if (x < e->x1 && x < e->x0) continue;
                        if (e->x1 < x && e->x0 < x) continue;
                    } else {
                        if (y < e->z1 && y < e->z0) continue;
                        if (e->z1 < y && e->z0 < y) continue;
                    }
                    if (z < e->minY - 50) continue;
                    if (e->maxY + 50 < z) continue;
                    snd_play_rider_sfx(4, i, 0xFF);
                    e->timer = 0;
                    for (u = 0; u < e->numPieces; u++) {
                        it = &e->pieces[u];
                        if (u & 1) {
                            it->bounces = 3;
                            it->vx = it[-1].vx;
                            it->vy = it[-1].vy;
                            it->vz = it[-1].vz;
                            it->rx = it[-1].rx;
                            it->ry = it[-1].ry;
                            it->rz = it[-1].rz;
                        } else {
                            it->vx = (s32)((s->bike.vel.x * 1000.0f) / 60.0f);
                            it->vy = (s32)((s->bike.vel.y * 1000.0f) / 60.0f);
                            it->vz = (s32)((s->bike.vel.z * 1000.0f) / 60.0f);
                            it->rx = (random_byte() - 0x80) * 4;
                            it->ry = (random_byte() - 0x80) * 5;
                            it->rz = (random_byte() - 0x80) * 2;
                            it->bounces = 3;
                        }
                    }
                }
            }
        }
    }
}

/* Breaks object index of gTrackObjects (remove.c): its pieces fly off at random. */
void shatter_break_object(s32 index) {
    ShatterObj *ent;
    ShatterObj *p;
    s32 cnt;
    s32 key;
    s32 i;
    s32 uVar5;
    s32 off;
    u8 *rec; /* a ShatterPiece; written as u16 halves */
    s32 t;
    s32 n2;

    t = sShatterCount;
    i = 0;
    if (t > 0) {
        cnt = t;
        p = sShatterObjects;
        key = (s32)&gTrackObjects->e[index];
    loop:
        ent = p;
        if ((s32)ent->obj == key) {
            goto chk;
        }
        i++;
        p = ent + 1;
        if (i < cnt) {
            goto loop;
        }
    }
chk:
    uVar5 = 0;
    if (i != sShatterCount) {
        ent->timer = 0;
        if (ent->numPieces > 0) {
            off = 0;
            do {
                rec = (u8 *)ent->pieces + off;
                ((ShatterPiece *)rec)->bounces = 3;
                if ((uVar5 & 1) != 0) {
                    /* the same motion as the previous piece */
                    *(u16 *)&((ShatterPiece *)rec)->vx = *(u16 *)&((ShatterPiece *)rec)[-1].vx;
                    *(u16 *)&((ShatterPiece *)rec)->vy = *(u16 *)&((ShatterPiece *)rec)[-1].vy;
                    *(u16 *)&((ShatterPiece *)rec)->vz = *(u16 *)&((ShatterPiece *)rec)[-1].vz;
                    *(u16 *)&((ShatterPiece *)rec)->rx = *(u16 *)&((ShatterPiece *)rec)[-1].rx;
                    *(u16 *)&((ShatterPiece *)rec)->ry = *(u16 *)&((ShatterPiece *)rec)[-1].ry;
                    *(u16 *)&((ShatterPiece *)rec)->rz = *(u16 *)&((ShatterPiece *)rec)[-1].rz;
                } else {
                    *(u16 *)&((ShatterPiece *)rec)->vx = 0;
                    *(u16 *)&((ShatterPiece *)rec)->vy = 0;
                    *(u16 *)&((ShatterPiece *)rec)->vz = 0;
                    *(u16 *)&((ShatterPiece *)rec)->vx = *(u16 *)&((ShatterPiece *)rec)->vx + ((s32)random_byte() - 0x80) / 2;
                    *(u16 *)&((ShatterPiece *)rec)->vy = *(u16 *)&((ShatterPiece *)rec)->vy + (((s32)random_byte() - 0x80) >> 2) / 2;
                    *(u16 *)&((ShatterPiece *)rec)->vz = *(u16 *)&((ShatterPiece *)rec)->vz + ((s32)random_byte() - 0x80) / 2;
                    *(u16 *)&((ShatterPiece *)rec)->rx = ((s32)random_byte() - 0x80) * 4;
                    *(u16 *)&((ShatterPiece *)rec)->ry = ((s32)random_byte() - 0x80) * 5;
                    *(u16 *)&((ShatterPiece *)rec)->rz = ((s32)random_byte() - 0x80) * 2;
                }
                n2 = ent->numPieces;
                off += 0x2C;
            } while ((s32)(++uVar5) < n2);
        }
    }
}

/* Puts the broken objects back together. */
void shatter_reset(void)
{
    s32 n;
    s32 nt;
    s32 m;
    ShatterObj *base;
    s32 i, j, k;
    ShatterObj *r;
    ShatterPiece *s;
    Vtx *v;

    nt = sShatterCount;
    i = 0;
    if (nt > 0) {
    m = -1;
    n = nt;
    base = sShatterObjects;
    do {
        r = &base[i];
        s = r->pieces;
        r->timer = m;
        v = r->obj->model->vtx;
        for (j = 0; j < r->numPieces; j++, s++, v += 3) {
            s->x = s->homeX;
            s->y = s->homeY;
            s->z = s->homeZ;
            for (k = 0; k < 3; k++) {
                v[k].v.ob[0] = (s->cx[k] + s->x) >> 4;
                v[k].v.ob[1] = (s->cy[k] + s->y) >> 4;
                v[k].v.ob[2] = (s->cz[k] + s->z) >> 4;
            }
        }
        i++;
    } while (i < n);
    }
    osWritebackDCache(0, 0x800000);
}

void shatter_free(void)
{
    s32 i;

    if (sShatterCount != 0) {
        for (i = 0; i < sShatterCount; i++) {
            mem_free(sShatterObjects[i].pieces);
        }
        mem_free(sShatterObjects);
        sShatterCount = 0;
    }
}

void shatter_update(void)
{
    shatter_check_riders();
    shatter_update_pieces();
}
