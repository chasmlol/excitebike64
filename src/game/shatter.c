#include "game.h"

/* The midpoint of the piece's longest edge (relative to the object's box centre). */
s32 shatter_tri_longest_edge(ShatterPiece *piece, s32 *outI, s32 *outN, s32 *ox, s32 *oy, s32 *oz)
{
    s32 i, next, lenSq, bestLenSq, longest;
    s32 dx, dy, dz;

    for (i = 0; i < 3; i++) {
        next = (i + 1) % 3;
        dx = piece->cx[next] - piece->cx[i];
        dy = piece->cy[next] - piece->cy[i];
        dz = piece->cz[next] - piece->cz[i];
        lenSq = dx * dx + dy * dy + dz * dz;
        if (lenSq >= bestLenSq || i == 0) {
            bestLenSq = lenSq;
            longest = i;
        }
    }
    next = (longest + 1) % 3;
    *ox = (piece->cx[longest] + piece->cx[next]) / 2 + piece->x;
    *oy = (piece->cy[longest] + piece->cy[next]) / 2 + piece->y;
    *oz = (piece->cz[longest] + piece->cz[next]) / 2 + piece->z;
    *outI = longest;
    *outN = next;
    return 0;
}

/* Splits every breakable object of the list into pieces, one per triangle of its model (whose vertices are three
   per triangle), and groups the triangles that share their longest edge's midpoint so they fly off together. */
void shatter_init(TrackModelList *objects)
{
    s32 x, y, z;
    s32 a, b;
    s32 i;
    s32 j, k, m, groupSize;
    s32 vy;
    ShatterObj *obj;
    ShatterObj *objs;
    PrimModel *model;
    TrackModel *trackObj;
    ShatterPiece *piece;

    j = 0;
    sShatterModelList = objects;
    for (i = 0; i < objects->count; i++) {
        if (gObjectFlags[i] & OBJ_BREAKABLE) {
            j++;
        }
    }
    sShatterCount = j;
    if (j == 0) {
        return;
    }
    objs = sShatterObjects = mem_alloc_dbg(j * sizeof(ShatterObj), 0, (s32)sStrShatterC, 0x74);
    j = 0;
    for (i = 0; i < objects->count; i++) {
        if (gObjectFlags[i] & OBJ_BREAKABLE) {
            objs[j++].obj = &objects->e[i];
        }
    }
    for (i = 0; i < sShatterCount; i++) {
        obj = &sShatterObjects[i];
        obj->x0 = obj->obj->x0;
        trackObj = obj->obj;
        obj->x1 = trackObj->x1;
        obj->z0 = trackObj->z0;
        obj->z1 = trackObj->z1;
        /* the normal of the box's diagonal */
        x = obj->z1 - obj->z0;
        z = obj->x0 - obj->x1;
        y = isqrt_u32_2(x * x + z * z) + 1;
        obj->minY = 999999;
        obj->maxY = -999999;
        obj->nx = (x << 14) / y;
        obj->nz = (z << 14) / y;
    }
    for (i = 0; i < sShatterCount; i++) {
        obj = &sShatterObjects[i];
        model = obj->obj->model;
        obj->numPieces = model->numVtx / 3;
        obj->pieces = mem_alloc_dbg(obj->numPieces * sizeof(ShatterPiece), 0, (s32)sStrShatterC, 0x96);
        for (j = 0; j < obj->numPieces; j++) {
            x = y = z = 0;
            for (a = 0; a < 3; a++) {
                vy = model->vtx[j * 3 + a].v.ob[1];
                x += model->vtx[j * 3 + a].v.ob[0];
                y += vy;
                z += model->vtx[j * 3 + a].v.ob[2];
                if (obj->maxY < vy) {
                    obj->maxY = vy;
                }
                if (vy < obj->minY) {
                    obj->minY = vy;
                }
            }
            /* the triangle's centre, in 16 mm units (the vertices' are 16 times coarser) */
            x = (x * 16) / 3;
            y = (y * 16) / 3;
            z = (z * 16) / 3;
            obj->pieces[j].x = x;
            obj->pieces[j].y = y;
            obj->pieces[j].z = z;
            obj->pieces[j].homeX = obj->pieces[j].x;
            obj->pieces[j].homeY = obj->pieces[j].y;
            obj->pieces[j].homeZ = obj->pieces[j].z;
            for (a = 0; a < 3; a++) {
                obj->pieces[j].cx[a] = model->vtx[j * 3 + a].v.ob[0] * 16 - x;
                obj->pieces[j].cy[a] = model->vtx[j * 3 + a].v.ob[1] * 16 - y;
                obj->pieces[j].cz[a] = model->vtx[j * 3 + a].v.ob[2] * 16 - z;
            }
        }
        obj->timer = -1;
    }
    for (i = 0; i < sShatterCount; i++) {
        obj = &sShatterObjects[i];
        for (j = 0; j < obj->numPieces; j += groupSize) {
            piece = &obj->pieces[j];
            if (shatter_tri_longest_edge(piece, &a, &b, &x, &y, &z) == 0) {
                k = j + 1;
                for (m = k; m < obj->numPieces; m++) {
                    s32 x2, y2, z2;
                    ShatterPiece *u = &obj->pieces[m];
                    if (shatter_tri_longest_edge(u, &a, &b, &x2, &y2, &z2) == 0 &&
                        x == x2 && y == y2 && z == z2) {
                        /* move it next to the group, swapping pieces and vertices through the Z buffer */
                        __builtin_memcpy(gZBuffer, u, sizeof(ShatterPiece));
                        __builtin_memcpy(u, &obj->pieces[k], sizeof(ShatterPiece));
                        __builtin_memcpy(&obj->pieces[k], gZBuffer, sizeof(ShatterPiece));
                        __builtin_memcpy(gZBuffer, &obj->obj->model->vtx[m * 3], 0x30);
                        __builtin_memcpy(&obj->obj->model->vtx[m * 3], &obj->obj->model->vtx[k * 3], 0x30);
                        __builtin_memcpy(&obj->obj->model->vtx[k * 3], gZBuffer, 0x30);
                        k++;
                    }
                }
                groupSize = k - j;
                if (groupSize != 1) {
                    /* the group turns about its common centre */
                    x = 0;
                    y = 0;
                    z = 0;
                    for (a = 0; a < groupSize; a++) {
                        x += piece[a].x;
                        y += piece[a].y;
                        z += piece[a].z;
                    }
                    x /= groupSize;
                    y /= groupSize;
                    z /= groupSize;
                    for (a = 0; a < groupSize; a++) {
                        for (b = 0; b < 3; b++) {
                            piece[a].cx[b] += piece[a].x;
                            piece[a].cy[b] += piece[a].y;
                            piece[a].cz[b] += piece[a].z;
                        }
                        piece[a].homeX = piece[a].x = x;
                        piece[a].homeY = piece[a].y = y;
                        piece[a].homeZ = piece[a].z = z;
                        for (b = 0; b < 3; b++) {
                            piece[a].cx[b] -= piece[a].x;
                            piece[a].cy[b] -= piece[a].y;
                            piece[a].cz[b] -= piece[a].z;
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
    ShatterObj *obj;
    TrackModel *trackObj;
    ShatterPiece *piece;
    Vtx *vtx;
    s32 centerX, centerZ, addr;
    s16 z;

    for (i = 0; i < sShatterCount; i++) {
        obj = &sShatterObjects[i];
        if (obj->timer < 0) continue;
        if (obj->timer >= SHATTER_FRAMES) continue;
        trackObj = obj->obj;
        piece = obj->pieces;
        /* the box's centre in mm */
        centerX = (trackObj->x0 + trackObj->x1) * 8;
        vtx = trackObj->model->vtx;
        centerZ = (trackObj->z0 + trackObj->z1) * 8;
        for (j = 0; j < obj->numPieces; j++, piece++, vtx += 3) {
            if (piece->bounces == 0) continue;
            piece->x += piece->vx;
            piece->y += piece->vy;
            piece->z += piece->vz;
            if (obj->timer > 0) {
                k = height_get_ground(piece->x + centerX, piece->y + 500, piece->z + centerZ);
                if (piece->y < k) {
                    piece->y = k;
                    piece->vy = -(piece->vy * 3) / 4;
                    if (--piece->bounces == 0) piece->y = k - 5000; /* sinks out of sight */
                }
            }
            piece->vy -= 2;
            mtx3i_from_angles(mtx, piece->rx * obj->timer, piece->ry * obj->timer, piece->rz * obj->timer);
            for (k = 0; k < 3; k++) {
                in[0] = piece->cx[k];
                in[1] = piece->cy[k];
                in[2] = piece->cz[k];
                vec3i_mul_mtx3i(in, mtx, out);
                vtx[k].v.ob[0] = (out[0] + piece->x) >> 4;
                vtx[k].v.ob[1] = (out[1] + piece->y) >> 4;
                vtx[k].v.ob[2] = (out[2] + piece->z) >> 4;
            }
        }
        if (++obj->timer >= SHATTER_FRAMES) {
            /* hide the object: its index in the list */
            addr = (s32)obj->obj - 4;
            j = (addr - (s32)sShatterModelList) >> 5;
            gObjectFlags[j] |= OBJ_HIDDEN;
        }
    }
    osWritebackDCache(0, 0x800000); /* all of RDRAM: the vertices the RSP reads */
}

/* Breaks a whole object (not one removed with walls) when a rider crosses its box's diagonal within its height. */
void shatter_check_riders(void)
{
    s32 addr, i, j, d, near, x, y, z, dx, dy;
    Rider *rider;
    ShatterObj *obj;
    ShatterPiece *piece;

    for (i = 0; i < 8; i++) {
        rider = &gRiders[i];
        if (rider->inactive != 0) continue;
        for (j = 0; j < sShatterCount; j++) {
            obj = &sShatterObjects[j];
            if (obj->timer < 0 &&
                (addr = (s32)obj->obj - 4, !(gObjectFlags[(addr - (s32)gTrackObjects) >> 5] & OBJ_WALL_GROUP))) {
                /* the bike in 16 mm units, and its distance d from the diagonal */
                x = (s32)(rider->bike.pos.x * 1000.0f) >> 4;
                y = (s32)(rider->bike.pos.z * 1000.0f) >> 4;
                d = (x - obj->x0) * obj->nx + (y - obj->z0) * obj->nz;
                d >>= 14;
                z = (s32)(rider->bike.pos.y * 1000.0f) >> 4;
                /* within 30 units of a small object's diagonal, 100 of a large one's */
                if (obj->x1 - obj->x0 < 30 && obj->z1 - obj->z0 < 30) {
                    near = (u32)(d + 30) < 61;
                } else {
                    near = (u32)(d + 100) < 201;
                }
                if (near) {
                    d = obj->x1 - obj->x0;
                    if (d < 0) d = -d;
                    dy = obj->z1 - obj->z0;
                    if (dy < 0) dy = -dy;
                    if (dy < d) {
                        if (x < obj->x1 && x < obj->x0) continue;
                        if (obj->x1 < x && obj->x0 < x) continue;
                    } else {
                        if (y < obj->z1 && y < obj->z0) continue;
                        if (obj->z1 < y && obj->z0 < y) continue;
                    }
                    if (z < obj->minY - 50) continue;
                    if (obj->maxY + 50 < z) continue;
                    snd_play_rider_sfx(4, i, 0xFF); /* the impact sound */
                    obj->timer = 0;
                    for (d = 0; d < obj->numPieces; d++) {
                        piece = &obj->pieces[d];
                        if (d & 1) {
                            piece->bounces = 3;
                            piece->vx = piece[-1].vx;
                            piece->vy = piece[-1].vy;
                            piece->vz = piece[-1].vz;
                            piece->rx = piece[-1].rx;
                            piece->ry = piece[-1].ry;
                            piece->rz = piece[-1].rz;
                        } else {
                            /* the bike's velocity per frame, a random spin */
                            piece->vx = (s32)((rider->bike.vel.x * 1000.0f) / 60.0f);
                            piece->vy = (s32)((rider->bike.vel.y * 1000.0f) / 60.0f);
                            piece->vz = (s32)((rider->bike.vel.z * 1000.0f) / 60.0f);
                            piece->rx = (random_byte() - 0x80) * 4;
                            piece->ry = (random_byte() - 0x80) * 5;
                            piece->rz = (random_byte() - 0x80) * 2;
                            piece->bounces = 3;
                        }
                    }
                }
            }
        }
    }
}

/* Breaks object index of gTrackObjects (remove.c): its pieces fly off at random. */
void shatter_break_object(s32 index) {
    ShatterObj *obj;
    ShatterObj *cur;
    s32 count;
    s32 target;
    s32 i;
    s32 j;
    s32 offset;
    u8 *piece; /* a ShatterPiece; its fields written as u16 to match */
    s32 total;
    s32 numPieces;

    total = sShatterCount;
    i = 0;
    if (total > 0) {
        count = total;
        cur = sShatterObjects;
        target = (s32)&gTrackObjects->e[index];
    loop:
        obj = cur;
        if ((s32)obj->obj == target) {
            goto chk;
        }
        i++;
        cur = obj + 1;
        if (i < count) {
            goto loop;
        }
    }
chk:
    j = 0;
    if (i != sShatterCount) {
        obj->timer = 0;
        if (obj->numPieces > 0) {
            offset = 0;
            do {
                piece = (u8 *)obj->pieces + offset;
                ((ShatterPiece *)piece)->bounces = 3;
                if ((j & 1) != 0) {
                    /* the same motion as the previous piece */
                    *(u16 *)&((ShatterPiece *)piece)->vx = *(u16 *)&((ShatterPiece *)piece)[-1].vx;
                    *(u16 *)&((ShatterPiece *)piece)->vy = *(u16 *)&((ShatterPiece *)piece)[-1].vy;
                    *(u16 *)&((ShatterPiece *)piece)->vz = *(u16 *)&((ShatterPiece *)piece)[-1].vz;
                    *(u16 *)&((ShatterPiece *)piece)->rx = *(u16 *)&((ShatterPiece *)piece)[-1].rx;
                    *(u16 *)&((ShatterPiece *)piece)->ry = *(u16 *)&((ShatterPiece *)piece)[-1].ry;
                    *(u16 *)&((ShatterPiece *)piece)->rz = *(u16 *)&((ShatterPiece *)piece)[-1].rz;
                } else {
                    *(u16 *)&((ShatterPiece *)piece)->vx = 0;
                    *(u16 *)&((ShatterPiece *)piece)->vy = 0;
                    *(u16 *)&((ShatterPiece *)piece)->vz = 0;
                    *(u16 *)&((ShatterPiece *)piece)->vx = *(u16 *)&((ShatterPiece *)piece)->vx + ((s32)random_byte() - 0x80) / 2;
                    *(u16 *)&((ShatterPiece *)piece)->vy = *(u16 *)&((ShatterPiece *)piece)->vy + (((s32)random_byte() - 0x80) >> 2) / 2;
                    *(u16 *)&((ShatterPiece *)piece)->vz = *(u16 *)&((ShatterPiece *)piece)->vz + ((s32)random_byte() - 0x80) / 2;
                    *(u16 *)&((ShatterPiece *)piece)->rx = ((s32)random_byte() - 0x80) * 4;
                    *(u16 *)&((ShatterPiece *)piece)->ry = ((s32)random_byte() - 0x80) * 5;
                    *(u16 *)&((ShatterPiece *)piece)->rz = ((s32)random_byte() - 0x80) * 2;
                }
                numPieces = obj->numPieces;
                offset += sizeof(ShatterPiece);
            } while ((s32)(++j) < numPieces);
        }
    }
}

/* Puts the broken objects back together. */
void shatter_reset(void)
{
    s32 count;
    s32 total;
    s32 whole;
    ShatterObj *objs;
    s32 i, j, k;
    ShatterObj *obj;
    ShatterPiece *piece;
    Vtx *vtx;

    total = sShatterCount;
    i = 0;
    if (total > 0) {
    whole = -1;
    count = total;
    objs = sShatterObjects;
    do {
        obj = &objs[i];
        piece = obj->pieces;
        obj->timer = whole;
        vtx = obj->obj->model->vtx;
        for (j = 0; j < obj->numPieces; j++, piece++, vtx += 3) {
            piece->x = piece->homeX;
            piece->y = piece->homeY;
            piece->z = piece->homeZ;
            for (k = 0; k < 3; k++) {
                vtx[k].v.ob[0] = (piece->cx[k] + piece->x) >> 4;
                vtx[k].v.ob[1] = (piece->cy[k] + piece->y) >> 4;
                vtx[k].v.ob[2] = (piece->cz[k] + piece->z) >> 4;
            }
        }
        i++;
    } while (i < count);
    }
    osWritebackDCache(0, 0x800000);
}

/* Frees the pieces and the objects. */
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

/* A frame: riders break objects, the pieces move. */
void shatter_update(void)
{
    shatter_check_riders();
    shatter_update_pieces();
}
