#include "game.h"

void fractal_update_tiles(s32 px, s32 pz)
{
    s32 i, j, x, z, idx, tx, tz, q, wa, xa;

    px /= 2000;
    pz /= 2000;
    sFractalCenterX = (px + 0x40) >> 7;
    sFractalCenterZ = (pz + 0x40) >> 7;
    for (i = -2; i < 2; i++) {
        for (j = -2; j < 2; j++) {
            z = sFractalCenterZ + i;
            q = (z & 3) * 4;
            x = sFractalCenterX + j;
            idx = q + (x & 3);
            if (gDesertTiles[idx].x != x || gDesertTiles[idx].z != z) {
                gDesertTiles[idx].map = sFractalMap + ((idx / 4) * 0x10000 + (idx % 4) * 0x80);
                fractal_tile_init(&gDesertTiles[idx], x, z);
            }
            gDesertTiles[idx].visible = 1;
            tx = gDesertTiles[idx].x * 0x80;
            tz = gDesertTiles[idx].z << 7;
            wa = tx + 0x7f;
            xa = tz + 0x7f;
            if (px + 0x7f < tx) gDesertTiles[idx].visible = 0;
            if (pz + 0x7f < tz) gDesertTiles[idx].visible = 0;
            if (wa < px - 0x80) gDesertTiles[idx].visible = 0;
            if (xa < pz - 0x80) gDesertTiles[idx].visible = 0;
        }
    }
    for (i = 0; i < 16; i++) {
        if (gDesertTiles[i].visible != 0) fractal_generate(&gDesertTiles[i], -1);
    }
}

#define RND() (sFractalRand = (sFractalRand * 0x35DF + 0x3038) % 65531)

void fractal_generate(DesertTile *f, s32 n)
{
    u32 e;
    s32 row, col, i, v, t;
    u8 *prev, *cur, *p, *next;
    s32 lvl, x, y, size, x2, y2, xm, ym, amp, idx;
    s32 c00, c01, c10, c11;
    s32 cnt;
    u32 u;

    cnt = n;
    sFractalRand = f->seed;
    while (cnt != 0) {
        if (f->sp == 0) {
            return;
        }
        f->sp--;
        e = f->stack[f->sp];
        if ((e >> 24) == DESERT_WORK_OBJECTS) {
            fractal_place_objects(f);
            continue;
        }
        if ((e >> 24) == DESERT_WORK_SMOOTH) {
            e &= 0xFFFFFF;
            row = e >> 7;
            col = e & 0x7F;
            if (row == 0 && col == 0) {
                __builtin_memcpy(f->smooth[0], f->map, 128);
                f->smoothCur = 1;
            }
            t = f->smoothCur;
            prev = f->smooth[1 - t];
            cur = f->smooth[t];
            p = f->map + (row << 9);
            if (row == 127) {
                next = f->nextRow;
            } else {
                next = p + 0x200;
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
                e++;
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
                e++;
            }
            if (i == 127) {
                v = prev[-1];
                v += prev[0] * 2;
                if (row != 0) {
                    v += f->nextCol[row - 1];
                } else {
                    v += f->nextCol[row];
                }
                v += cur[-1] * 2;
                v += cur[0] * 4;
                v += f->nextCol[row] * 2;
                v += next[-1];
                v += next[0] * 2;
                v += f->nextCol[row + 1];
                v >>= 4;
                v = (v * v) >> 8;
                if (v < 4) {
                    v = 4;
                }
                v ^= 0x80;
                *p = v;
                e++;
                f->smoothCur ^= 1;
            }
            if (e < 0x4000) {
                f->stack[f->sp++] = e | (DESERT_WORK_SMOOTH << 24);
            }
            cnt--;
        } else {
            row = (e >> 8) & 0xFF;
            u = e >> 16;
            lvl = e & 0xFF;
            size = 1 << lvl;
            x = u & 0xFF;
            y = row << 9;
            x2 = x + size;
            y2 = y + (size << 9);
            xm = x + (size >> 1);
            ym = y + (size << 8);
            c00 = f->map[y | x];
            if (x2 == 128) {
                if (y2 == 0x10000) {
                    c01 = f->nextCol[y / 512];
                    c10 = f->nextRow[x];
                    c11 = f->nextCol[128];
                } else {
                    c01 = f->nextCol[y / 512];
                    c10 = f->map[y2 | x];
                    c11 = f->nextCol[y2 / 512];
                }
            } else {
                if (y2 == 0x10000) {
                    c01 = f->map[y | x2];
                    c10 = f->nextRow[x];
                    c11 = f->nextRow[x2];
                } else {
                    c01 = f->map[y | x2];
                    c10 = f->map[y2 | x];
                    c11 = f->map[y2 | x2];
                }
            }
            x2 %= 128;
            y2 %= 0x10000;
            amp = sFractalAmplitudes[lvl];
            if (f->map[y | xm] == 0) {
                RND();
                v = ((c00 + c01) >> 1) + ((sFractalRand & 0xFF) - 0x80) * amp / 32;
                if (v <= 0) v = 1;
                if (v >= 0x100) v = 0xFF;
                f->map[y | xm] = v;
            }
            if (f->map[ym | x] == 0) {
                RND();
                v = ((c00 + c10) >> 1) + ((sFractalRand & 0xFF) - 0x80) * amp / 32;
                if (v <= 0) v = 1;
                if (v >= 0x100) v = 0xFF;
                f->map[ym | x] = v;
            }
            if (f->map[ym | x2] == 0) {
                RND();
                v = ((c01 + c11) >> 1) + ((sFractalRand & 0xFF) - 0x80) * amp / 32;
                if (v <= 0) v = 1;
                if (v >= 0x100) v = 0xFF;
                f->map[ym | x2] = v;
            }
            if (f->map[y2 | xm] == 0) {
                RND();
                v = ((c10 + c11) >> 1) + ((sFractalRand & 0xFF) - 0x80) * amp / 32;
                if (v <= 0) v = 1;
                if (v >= 0x100) v = 0xFF;
                f->map[y2 | xm] = v;
            }
            if (f->map[ym | xm] == 0) {
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
                f->map[ym | xm] = v;
            }
            if (size != 2) {
                f->stack[f->sp++] = (xm << 16) | ((ym / 512) << 8) | (lvl - 1);
                f->stack[f->sp++] = (x << 16) | ((ym / 512) << 8) | (lvl - 1);
                f->stack[f->sp++] = (xm << 16) | ((y / 512) << 8) | (lvl - 1);
                f->stack[f->sp++] = (x << 16) | ((y / 512) << 8) | (lvl - 1);
            }
            cnt--;
        }

    }
    f->seed = sFractalRand;
}

s32 fractal_corner_height(s32 a0, s32 a1)
{
    s32 y;

    y = ((a0 * (a1 + 0x7B) * 0x1C8) << (a0 % 0x315A)) + a1 * 0x55;
    sFractalRand = (y * 0x35DF + 0x3038) % 0xFFFB;
    return sFractalRand & 0xFF;
}

void fractal_subdivide_edge(u8 *buf)
{
    s32 step;
    s32 u;
    s32 k;
    s32 seed;
    s32 v;
    s32 t;

    step = 0x40;
    do {
        u = step;
        if (step < 0x80) {
            k = step * 2;
            do {
                sFractalRand = (sFractalRand * 0x35df + 0x3038) % 0xfffb;
                seed = sFractalRand;
                t = ((s32)buf[u - step] + (s32)buf[u + step]) >> 1;
                v = ((seed & 0xff) - 0x80) * step;
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
                buf[u] = (u8)t;
                u = u + k;
            } while (u < 0x80);
        }
        step = step >> 1;
    } while (step != 0);
}

static inline s32 Seed(s32 x, s32 z)
{
    return ((x * (z + 123) * 456) << (x % 12634)) + z;
}

void fractal_tile_init(DesertTile *t, s32 x, s32 z)
{
    s32 i;
    s32 seed;

    for (i = 0; i < 0x10000; i += 0x200) {
        mem_fill(t->map + i, 0, 0x80);
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

void fractal_place_objects(volatile DesertTile *t)
{
    s32 x, cellX, z, cellZ;
    s32 i, n, k, y, fireZ, fireY;
    DesertTile *p;

    desert_export_checkpoints();
    n = 0;
    for (i = 0; i < gDesertFireCount; i++) {
        divmod_floor(gDesertFires[i].x, 0x3E800, &x, &cellX);
        if (x == t->x) {
            divmod_floor(gDesertFires[i].z, 0x3E800, &z, &cellZ);
            if (z == t->z) {
                t->objs[n].x = gDesertFires[i].x;
                cellX = cellX / 2000;
                cellZ = cellZ / 2000;
                /* the heights are read signed here */
                t->objs[n].y = ((s8 *)t->map)[cellZ * 512 + cellX] << 6;
                fireZ = gDesertFires[i].z;
                t->objs[n].kind = 2;
                fireY = t->objs[n].y;
                t->objs[n].ref = i;
                t->objs[n].radius = 0;
                t->objs[n].height = 0;
                t->objs[n].z = fireZ;
                n++;
                gDesertFires[i].y = fireY;
            }
        }
    }
    /* scenery in the rest */
    p = (DesertTile *)t; /* stores that need not be volatile */
    i = n;
    if (i < 8) {
        do {
            do {
                k = 0;
                RND();
                x = (sFractalRand & 0xFF) % 120 + 4;
                RND();
                z = (sFractalRand & 0xFF) % 120 + 4;
                y = ((s8 *)t->map)[z * 512 + x];
                x = x * 2000;
                x = x + t->x * 0x3E800;
                z = z * 2000;
                z = z + t->z * 0x3E800;
                y <<= 6;
                /* meant to keep it off the objects placed before; it tests the new object itself */
                for (k = 0; k < i; k++) {
                    if (p->objs[i].x == x && p->objs[i].z == z) {
                        break;
                    }
                }
            } while (k != i);
            p->objs[i].y = y;
            p->objs[i].x = x;
            p->objs[i].z = z;
            p->objs[i].kind = (s32)random_byte() % 2;
            p->objs[i].radius = 400;
            p->objs[i].height = 1000;
            if (i) k = 1; else k = 2;
            i++;
        } while (i < 8);
    }
    t->numObjs = 8;
}

s32 fractal_get_visible_objects(s32 x, s32 z, s32 ang, s32 range, DesertObj *out, s32 max)
{
    s32 n;
    s32 i, j;
    s16 c, s;
    s32 dx, dz, a, b;

    range /= 16;
    n = 0;
    c = gCosTable[(ang >> 4) & 0xFFF];
    s = gCosTable[((ang >> 4) - 0x400) & 0xFFF];
    for (i = 0; i < 16; i++) {
        if (n >= max) {
            break;
        }
        if (gDesertTiles[i].sp == 0) {
            for (j = 0; j < gDesertTiles[i].numObjs; j++) {
                if (n >= max) {
                    break;
                }
                dx = (gDesertTiles[i].objs[j].x - x) / 16;
                dz = (gDesertTiles[i].objs[j].z - z) / 16;
                a = (c * dx + s * dz) >> 14;
                b = (c * dz - s * dx) >> 14;
                if (b > range) {
                    continue;
                }
                if (b < -359 || a < -359 || a > 359) {
                    if (a < 0) {
                        a -= 360;
                    }
                    if (a > 0) {
                        a += 360;
                    }
                    if (b < a || a < -b || b < 0) {
                        continue;
                    }
                }
                out[n++] = gDesertTiles[i].objs[j];
            }
        }
    }
    return n;
}

void fractal_emit_fire_smoke(void)
{
    f32 pos[3];
    f32 vel[3];
    u8 col[4];
    s32 i, j;
    DesertObj *p;

    if ((gRaceFrame & 7) != 0) {
        return;
    }
    col[0] = 0;
    col[1] = 0;
    col[2] = 0;
    col[3] = 0;
    for (i = 0; i < 16; i++) {
        for (j = 0; j < gDesertTiles[i].numObjs; j++) {
            p = &gDesertTiles[i].objs[j];
            if (p->kind == 2 && p->ref == gDesWaypoint) {
                pos[0] = (f32)p->x / 1000.0f;
                pos[1] = (f32)p->y / 1000.0f + 1.0f;
                pos[2] = (f32)p->z / 1000.0f;
                vel[0] = (f32)(s32)(random_byte() - 0x80) / 10000.0f;
                vel[1] = 0.1f;
                vel[2] = (f32)(s32)(random_byte() - 0x80) / 10000.0f;
                particle_spawn(6, 0, pos, vel, col);
            }
        }
    }
}

void fractal_init(u8 *map)
{
    DesertTile **dst = &gDesertTiles;

    sFractalMap = map;
    *dst = mem_alloc_dbg(16 * sizeof(DesertTile), 0, (s32)&sStrFractalC, 0x266);
}

void fractal_reset(void)
{
    s32 i;
    DesertTile *t;

    mem_fill((u8 *)gDesertTiles, 0, 4); /* only the first word */
    i = 15;
    t = &gDesertTiles[15];
    do {
        t->x = -999;
        i--;
        t--;
    } while (i >= 0);
    fractal_update_tiles(0, 0);
    for (i = 0; i < 16; i++) {
        fractal_generate(&gDesertTiles[i], -1);
    }
}

void fractal_generate_step(s32 n)
{
    s32 i, cnt;

    cnt = 0;
    for (i = 0; i < 16; i++) {
        if (gDesertTiles[i].sp != 0) {
            cnt++;
        }
    }
    if (cnt != 0) {
        n = (n + cnt - 1) / cnt;
        for (i = 0; i < 16; i++) {
            if (gDesertTiles[i].sp != 0) {
                fractal_generate(&gDesertTiles[i], n);
            }
        }
    }
}

u8 fractal_rand_byte(void)
{
    s32 x;
    sFractalRand = (sFractalRand * 13791 + 0x3038) % 65531;
    x = sFractalRand;
    return x & 0xff;
}

s32 fractal_tile_seed(s32 param_1, s32 param_2)
{
    return ((param_1 * (param_2 + 0x7B) * 0x1C8) << (param_1 % 0x315A)) + param_2;
}
