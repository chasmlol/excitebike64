#include "game.h"

/* Keeps the 4x4 tiles around the camera (mm) in the window, starting the tiles that came into it, and finishes the
   work of the tiles near the camera at once. */
void fractal_update_tiles(s32 camX, s32 camZ)
{
    s32 i, j, x, z, idx, originX, originZ, rowBase, endX, endZ;

    camX /= DESERT_CELL_MM;
    camZ /= DESERT_CELL_MM;
    sFractalCenterX = (camX + 0x40) >> 7;
    sFractalCenterZ = (camZ + 0x40) >> 7;
    for (i = -2; i < 2; i++) {
        for (j = -2; j < 2; j++) {
            z = sFractalCenterZ + i;
            rowBase = (z & 3) * 4;
            x = sFractalCenterX + j;
            idx = rowBase + (x & 3);
            if (gDesertTiles[idx].x != x || gDesertTiles[idx].z != z) {
                gDesertTiles[idx].map = sFractalMap + ((idx / 4) * 0x10000 + (idx % 4) * DESERT_TILE_CELLS);
                fractal_tile_init(&gDesertTiles[idx], x, z);
            }
            /* near: within a tile's width of the camera */
            gDesertTiles[idx].visible = 1;
            originX = gDesertTiles[idx].x * 0x80;
            originZ = gDesertTiles[idx].z << 7;
            endX = originX + 0x7f;
            endZ = originZ + 0x7f;
            if (camX + 0x7f < originX) gDesertTiles[idx].visible = 0;
            if (camZ + 0x7f < originZ) gDesertTiles[idx].visible = 0;
            if (endX < camX - 0x80) gDesertTiles[idx].visible = 0;
            if (endZ < camZ - 0x80) gDesertTiles[idx].visible = 0;
        }
    }
    for (i = 0; i < DESERT_NUM_TILES; i++) {
        if (gDesertTiles[i].visible != 0) fractal_generate(&gDesertTiles[i], -1);
    }
}

/* The next value of the terrain's random generator (a linear congruential one modulo 65531) */
#define RND() (sFractalRand = (sFractalRand * 0x35DF + 0x3038) % 65531)

/* Runs up to `steps` work items of a tile (all of them with -1): smoothing a row, a diamond-square step, or placing
   the objects. */

void fractal_generate(DesertTile *tile, s32 steps)
{
    u32 item;
    s32 row, col, i, v, t;
    u8 *prev, *cur, *p, *next;
    s32 level, x, y, size, x2, y2, xm, ym, amp, idx;
    s32 c00, c01, c10, c11;
    s32 left;
    u32 itemX;

    left = steps;
    sFractalRand = tile->seed;
    while (left != 0) {
        if (tile->sp == 0) {
            return;
        }
        tile->sp--;
        item = tile->stack[tile->sp];
        if ((item >> 24) == DESERT_WORK_OBJECTS) {
            fractal_place_objects(tile);
            continue;
        }
        if ((item >> 24) == DESERT_WORK_SMOOTH) {
            item &= 0xFFFFFF;
            row = item >> 7;
            col = item & 0x7F;
            if (row == 0 && col == 0) {
                __builtin_memcpy(tile->smooth[0], tile->map, 128);
                tile->smoothCur = 1;
            }
            t = tile->smoothCur;
            prev = tile->smooth[1 - t];
            cur = tile->smooth[t];
            p = tile->map + (row << 9);
            /* a 3x3 weighted average of the original heights, squared to sharpen the dunes */
            if (row == 127) {
                next = tile->nextRow;
            } else {
                next = p + DESERT_MAP_SIZE;
            }
            prev += col;
            cur += col;
            p += col;
            next += col;
            if (col == 0) {
                __builtin_memcpy(cur, p, 128);
                v = prev[0] * 3;
                v += prev[1];
                v += cur[0] * 6;
                v += cur[1] * 2;
                v += next[0] * 3;
                v += next[1];
                v >>= 4;
                v = (v * v) >> 8;
                if (v < 4) {
                    v = 4;
                }
                v ^= 0x80;
                *p = v;
                p++;
                prev++;
                cur++;
                next++;
                item++;
            }
            for (i = col; i < col + 64; i++) {
                if (i == 127) {
                    break;
                }
                v = prev[-1];
                v += prev[0] * 2;
                v += prev[1];
                v += cur[-1] * 2;
                v += cur[0] * 4;
                v += cur[1] * 2;
                v += next[-1];
                v += next[0] * 2;
                v += next[1];
                v >>= 4;
                v = (v * v) >> 8;
                if (v < 4) {
                    v = 4;
                }
                v ^= 0x80;
                *p = v;
                p++;
                prev++;
                cur++;
                next++;
                item++;
            }
            if (i == 127) {
                v = prev[-1];
                v += prev[0] * 2;
                if (row != 0) {
                    v += tile->nextCol[row - 1];
                } else {
                    v += tile->nextCol[row];
                }
                v += cur[-1] * 2;
                v += cur[0] * 4;
                v += tile->nextCol[row] * 2;
                v += next[-1];
                v += next[0] * 2;
                v += tile->nextCol[row + 1];
                v >>= 4;
                v = (v * v) >> 8;
                if (v < 4) {
                    v = 4;
                }
                v ^= 0x80;
                *p = v;
                item++;
                tile->smoothCur ^= 1;
            }
            if (item < DESERT_TILE_CELLS * DESERT_TILE_CELLS) {
                tile->stack[tile->sp++] = item | (DESERT_WORK_SMOOTH << 24);
            }
            left--;
        } else {
            /* diamond-square: the midpoints of the square's edges and its centre, from its corners plus noise */
            row = (item >> 8) & 0xFF;
            itemX = item >> 16;
            level = item & 0xFF;
            size = 1 << level;
            x = itemX & 0xFF;
            y = row << 9;
            x2 = x + size;
            y2 = y + (size << 9);
            xm = x + (size >> 1);
            ym = y + (size << 8);
            c00 = tile->map[y | x];
            if (x2 == 128) {
                if (y2 == 0x10000) {
                    c01 = tile->nextCol[y / 512];
                    c10 = tile->nextRow[x];
                    c11 = tile->nextCol[128];
                } else {
                    c01 = tile->nextCol[y / 512];
                    c10 = tile->map[y2 | x];
                    c11 = tile->nextCol[y2 / 512];
                }
            } else {
                if (y2 == 0x10000) {
                    c01 = tile->map[y | x2];
                    c10 = tile->nextRow[x];
                    c11 = tile->nextRow[x2];
                } else {
                    c01 = tile->map[y | x2];
                    c10 = tile->map[y2 | x];
                    c11 = tile->map[y2 | x2];
                }
            }
            x2 %= 128;
            y2 %= 0x10000;
            amp = sFractalAmplitudes[level];
            if (tile->map[y | xm] == 0) {
                RND();
                v = ((c00 + c01) >> 1) + ((sFractalRand & 0xFF) - 0x80) * amp / 32;
                if (v <= 0) v = 1;
                if (v >= 0x100) v = 0xFF;
                tile->map[y | xm] = v;
            }
            if (tile->map[ym | x] == 0) {
                RND();
                v = ((c00 + c10) >> 1) + ((sFractalRand & 0xFF) - 0x80) * amp / 32;
                if (v <= 0) v = 1;
                if (v >= 0x100) v = 0xFF;
                tile->map[ym | x] = v;
            }
            if (tile->map[ym | x2] == 0) {
                RND();
                v = ((c01 + c11) >> 1) + ((sFractalRand & 0xFF) - 0x80) * amp / 32;
                if (v <= 0) v = 1;
                if (v >= 0x100) v = 0xFF;
                tile->map[ym | x2] = v;
            }
            if (tile->map[y2 | xm] == 0) {
                RND();
                v = ((c10 + c11) >> 1) + ((sFractalRand & 0xFF) - 0x80) * amp / 32;
                if (v <= 0) v = 1;
                if (v >= 0x100) v = 0xFF;
                tile->map[y2 | xm] = v;
            }
            if (tile->map[ym | xm] == 0) {
                RND();
                v = ((c00 + c01) >> 1) + ((sFractalRand & 0xFF) - 0x80) * amp / 32;
                RND();
                v += ((c00 + c10) >> 1) + ((sFractalRand & 0xFF) - 0x80) * amp / 32;
                RND();
                v += ((c01 + c11) >> 1) + ((sFractalRand & 0xFF) - 0x80) * amp / 32;
                RND();
                v += ((c10 + c11) >> 1) + ((sFractalRand & 0xFF) - 0x80) * amp / 32;
                v >>= 2;
                if (v <= 0) v = 1;
                if (v >= 0x100) v = 0xFF;
                tile->map[ym | xm] = v;
            }
            if (size != 2) {
                tile->stack[tile->sp++] = (xm << 16) | ((ym / 512) << 8) | (level - 1);
                tile->stack[tile->sp++] = (x << 16) | ((ym / 512) << 8) | (level - 1);
                tile->stack[tile->sp++] = (xm << 16) | ((y / 512) << 8) | (level - 1);
                tile->stack[tile->sp++] = (x << 16) | ((y / 512) << 8) | (level - 1);
            }
            left--;
        }

    }
    tile->seed = sFractalRand;
}

/* The height of a tile's corner, from a hash of its coordinates (so that the tiles meeting there agree). */
s32 fractal_corner_height(s32 x, s32 z)
{
    s32 y;

    y = ((x * (z + 0x7B) * 0x1C8) << (x % 0x315A)) + z * 0x55;
    sFractalRand = (y * 0x35DF + 0x3038) % 0xFFFB;
    return sFractalRand & 0xFF;
}

/* Fills a 129-entry edge between its two ends by midpoint displacement. */
void fractal_subdivide_edge(u8 *buf)
{
    s32 step;
    s32 i;
    s32 stride;
    s32 rnd;
    s32 v;
    s32 t;

    step = 0x40;
    do {
        i = step;
        if (step < 0x80) {
            stride = step * 2;
            do {
                sFractalRand = (sFractalRand * 0x35df + 0x3038) % 0xfffb;
                rnd = sFractalRand;
                t = ((s32)buf[i - step] + (s32)buf[i + step]) >> 1;
                v = ((rnd & 0xff) - 0x80) * step;
                if (v < 0) {
                    v = v + 0x1f;
                }
                t = t + (v >> 5);
                if (t <= 0) {
                    t = 1;
                }
                if (t > 0xff) {
                    t = 0xff;
                }
                buf[i] = (u8)t;
                i = i + stride;
            } while (i < 0x80);
        }
        step = step >> 1;
    } while (step != 0);
}

/* fractal_tile_seed written inline */
static inline s32 Seed(s32 x, s32 z)
{
    return ((x * (z + 123) * 456) << (x % 12634)) + z;
}

/* Starts tile (x, z): clears its cells, pushes its work (objects, smoothing, subdivision) and makes its edges from
   the hashes of its corners, so that the neighbouring tiles' edges match. */
void fractal_tile_init(DesertTile *t, s32 x, s32 z)
{
    s32 i;
    s32 seed;

    for (i = 0; i < DESERT_TILE_CELLS * DESERT_MAP_SIZE; i += DESERT_MAP_SIZE) {
        mem_fill(t->map + i, 0, DESERT_TILE_CELLS);
    }
    /* every store of the count stays: volatile */
    t->stack[0] = DESERT_WORK_OBJECTS << 24;
    *(volatile s32 *)&t->sp = 0;
    *(volatile s32 *)&t->sp = 1;
    t->stack[1] = DESERT_WORK_SMOOTH << 24;
    *(volatile s32 *)&t->sp = 2;
    t->x = x;
    t->z = z;
    t->stack[2] = 7; /* subdivide the whole tile */
    seed = Seed(x, z);
    *(volatile s32 *)&t->sp = 3;
    t->seed = seed;
    t->map[0] = fractal_corner_height(x, z);
    t->nextCol[0] = fractal_corner_height(x + 1, z);
    t->nextCol[0x80] = fractal_corner_height(x + 1, z + 1);
    t->nextRow[0] = fractal_corner_height(x, z + 1);
    t->nextRow[0x80] = t->nextCol[0x80];
    sFractalRand = Seed(x, z + 1) + 1;
    fractal_subdivide_edge(t->nextRow);
    sFractalRand = Seed(x + 1, z) + 2;
    fractal_subdivide_edge(t->nextCol);
    sFractalRand = t->seed + 1;
    sFractalEdgeBuf[0] = t->map[0];
    sFractalEdgeBuf[0x80] = t->nextCol[0];
    fractal_subdivide_edge(sFractalEdgeBuf);
    for (i = 0; i < 0x80; i++) {
        t->map[i] = sFractalEdgeBuf[i];
    }
    sFractalRand = t->seed + 2;
    sFractalEdgeBuf[0] = t->map[0];
    sFractalEdgeBuf[0x80] = t->nextRow[0];
    fractal_subdivide_edge(sFractalEdgeBuf);
    for (i = 0; i < 0x80; i++) {
        t->map[i * 0x200] = sFractalEdgeBuf[i];
    }
}

/* Places a tile's objects: the campfires of the checkpoints that lie on it, then scenery at random points up to
   eight. */
void fractal_place_objects(volatile DesertTile *t)
{
    s32 x, cellX, z, cellZ;
    s32 i, count, k, y, fireZ, fireY;
    DesertTile *tile;

    desert_export_checkpoints();
    count = 0;
    for (i = 0; i < gDesertFireCount; i++) {
        divmod_floor(gDesertFires[i].x, DESERT_TILE_MM, &x, &cellX);
        if (x == t->x) {
            divmod_floor(gDesertFires[i].z, DESERT_TILE_MM, &z, &cellZ);
            if (z == t->z) {
                t->objs[count].x = gDesertFires[i].x;
                cellX = cellX / DESERT_CELL_MM;
                cellZ = cellZ / DESERT_CELL_MM;
                /* the heights are read signed here */
                t->objs[count].y = ((s8 *)t->map)[cellZ * DESERT_MAP_SIZE + cellX] << 6;
                fireZ = gDesertFires[i].z;
                t->objs[count].kind = DESERT_OBJ_CAMPFIRE;
                fireY = t->objs[count].y;
                t->objs[count].ref = i;
                t->objs[count].radius = 0;
                t->objs[count].height = 0;
                t->objs[count].z = fireZ;
                count++;
                gDesertFires[i].y = fireY;
            }
        }
    }
    /* scenery in the rest */
    tile = (DesertTile *)t; /* stores that need not be volatile */
    i = count;
    if (i < DESERT_TILE_OBJS) {
        do {
            do {
                k = 0;
                RND();
                x = (sFractalRand & 0xFF) % 120 + 4;
                RND();
                z = (sFractalRand & 0xFF) % 120 + 4;
                y = ((s8 *)t->map)[z * DESERT_MAP_SIZE + x];
                x = x * DESERT_CELL_MM;
                x = x + t->x * DESERT_TILE_MM;
                z = z * DESERT_CELL_MM;
                z = z + t->z * DESERT_TILE_MM;
                y <<= 6;
                /* meant to keep it off the objects placed before; it tests the new object itself */
                for (k = 0; k < i; k++) {
                    if (tile->objs[i].x == x && tile->objs[i].z == z) {
                        break;
                    }
                }
            } while (k != i);
            tile->objs[i].y = y;
            tile->objs[i].x = x;
            tile->objs[i].z = z;
            tile->objs[i].kind = (s32)random_byte() % 2;
            tile->objs[i].radius = 400;
            tile->objs[i].height = 1000;
            if (i) k = 1; else k = 2; /* unused */
            i++;
        } while (i < DESERT_TILE_OBJS);
    }
    t->numObjs = DESERT_TILE_OBJS;
}

/* Copies to out (up to max) the objects of the finished tiles that lie ahead of the camera at (x, z) facing yaw,
   within range mm and a 90-degree field of view; returns their number. */
s32 fractal_get_visible_objects(s32 x, s32 z, s32 yaw, s32 range, DesertObj *out, s32 max)
{
    s32 count;
    s32 i, j;
    s16 cosYaw, sinYaw;
    s32 dx, dz, side, ahead;

    range /= 16;
    count = 0;
    cosYaw = gCosTable[(yaw >> 4) & 0xFFF];
    sinYaw = gCosTable[((yaw >> 4) - 0x400) & 0xFFF];
    for (i = 0; i < DESERT_NUM_TILES; i++) {
        if (count >= max) {
            break;
        }
        if (gDesertTiles[i].sp == 0) {
            for (j = 0; j < gDesertTiles[i].numObjs; j++) {
                if (count >= max) {
                    break;
                }
                dx = (gDesertTiles[i].objs[j].x - x) / 16;
                dz = (gDesertTiles[i].objs[j].z - z) / 16;
                side = (cosYaw * dx + sinYaw * dz) >> 14;
                ahead = (cosYaw * dz - sinYaw * dx) >> 14;
                if (ahead > range) {
                    continue;
                }
                if (ahead < -359 || side < -359 || side > 359) {
                    if (side < 0) {
                        side -= 360;
                    }
                    if (side > 0) {
                        side += 360;
                    }
                    if (ahead < side || side < -ahead || ahead < 0) {
                        continue;
                    }
                }
                out[count++] = gDesertTiles[i].objs[j];
            }
        }
    }
    return count;
}

/* Every 8 frames, a puff of smoke from the campfire of the next checkpoint. */
void fractal_emit_fire_smoke(void)
{
    f32 pos[3];
    f32 vel[3];
    u8 col[4];
    s32 i, j;
    DesertObj *obj;

    if ((gRaceFrame & 7) != 0) {
        return;
    }
    col[0] = 0;
    col[1] = 0;
    col[2] = 0;
    col[3] = 0;
    for (i = 0; i < DESERT_NUM_TILES; i++) {
        for (j = 0; j < gDesertTiles[i].numObjs; j++) {
            obj = &gDesertTiles[i].objs[j];
            if (obj->kind == DESERT_OBJ_CAMPFIRE && obj->ref == gDesWaypoint) {
                pos[0] = (f32)obj->x / 1000.0f;
                pos[1] = (f32)obj->y / 1000.0f + 1.0f;
                pos[2] = (f32)obj->z / 1000.0f;
                vel[0] = (f32)(s32)(random_byte() - 0x80) / 10000.0f;
                vel[1] = 0.1f;
                vel[2] = (f32)(s32)(random_byte() - 0x80) / 10000.0f;
                particle_spawn(PARTICLE_SMOKE, 0, pos, vel, col);
            }
        }
    }
}

/* Allocates the 16 tiles over the height map. */
void fractal_init(u8 *map)
{
    DesertTile **dst = &gDesertTiles;

    sFractalMap = map;
    *dst = mem_alloc_dbg(DESERT_NUM_TILES * sizeof(DesertTile), 0, (s32)&sStrFractalC, 0x266);
}

/* Empties the tiles and generates the ones around the start. */
void fractal_reset(void)
{
    s32 i;
    DesertTile *t;

    mem_fill((u8 *)gDesertTiles, 0, 4); /* only the first word */
    i = DESERT_NUM_TILES - 1;
    t = &gDesertTiles[DESERT_NUM_TILES - 1];
    do {
        t->x = -999;
        i--;
        t--;
    } while (i >= 0);
    fractal_update_tiles(0, 0);
    for (i = 0; i < DESERT_NUM_TILES; i++) {
        fractal_generate(&gDesertTiles[i], -1);
    }
}

/* Shares about `steps` work items of this frame among the tiles still being generated. */
void fractal_generate_step(s32 steps)
{
    s32 i, busy;

    busy = 0;
    for (i = 0; i < DESERT_NUM_TILES; i++) {
        if (gDesertTiles[i].sp != 0) {
            busy++;
        }
    }
    if (busy != 0) {
        steps = (steps + busy - 1) / busy;
        for (i = 0; i < DESERT_NUM_TILES; i++) {
            if (gDesertTiles[i].sp != 0) {
                fractal_generate(&gDesertTiles[i], steps);
            }
        }
    }
}

/* The next byte of the terrain's random generator. */
u8 fractal_rand_byte(void)
{
    s32 x;
    sFractalRand = (sFractalRand * 13791 + 0x3038) % 65531;
    x = sFractalRand;
    return x & 0xff;
}

/* A hash of a tile's coordinates: the seed of its random generator. */
s32 fractal_tile_seed(s32 x, s32 z)
{
    return ((x * (z + 0x7B) * 0x1C8) << (x % 0x315A)) + z;
}
