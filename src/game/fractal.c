#include "game.h"

typedef struct {
    s32 x;
    s32 z;
    s16 vis;
    s16 pad;
    u8 pad2[0x310];
    s32 tex;
    u8 rest[0xE4];
} Tile_2;

extern Tile_2 *gDesertTiles_3;

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
            if (gDesertTiles_3[idx].x != x || gDesertTiles_3[idx].z != z) {
                gDesertTiles_3[idx].tex = sFractalMap + ((idx / 4) * 0x10000 + (idx % 4) * 0x80);
                fractal_tile_init(&gDesertTiles_3[idx], x, z);
            }
            gDesertTiles_3[idx].vis = 1;
            tx = gDesertTiles_3[idx].x * 0x80;
            tz = gDesertTiles_3[idx].z << 7;
            wa = tx + 0x7f;
            xa = tz + 0x7f;
            if (px + 0x7f < tx) gDesertTiles_3[idx].vis = 0;
            if (pz + 0x7f < tz) gDesertTiles_3[idx].vis = 0;
            if (wa < px - 0x80) gDesertTiles_3[idx].vis = 0;
            if (xa < pz - 0x80) gDesertTiles_3[idx].vis = 0;
        }
    }
    for (i = 0; i < 16; i++) {
        if (gDesertTiles_3[i].vis != 0) fractal_generate(&gDesertTiles_3[i], -1);
    }
}

#define RND() (sFractalRand = (sFractalRand * 0x35DF + 0x3038) % 65531)

void fractal_generate(Field *f, s32 n)
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
        if ((e >> 24) == 2) {
            fractal_place_objects(f);
            continue;
        }
        if ((e >> 24) == 1) {
            e &= 0xFFFFFF;
            row = e >> 7;
            col = e & 0x7F;
            if (row == 0 && col == 0) {
                __builtin_memcpy(f->buf[0], f->map, 128);
                f->toggle = 1;
            }
            t = f->toggle;
            prev = f->buf[1 - t];
            cur = f->buf[t];
            p = f->map + (row << 9);
            if (row == 127) {
                next = f->rowE;
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
                    v += f->colE[row - 1];
                } else {
                    v += f->colE[row];
                }
                v += cur[-1] * 2;
                v += cur[0] * 4;
                v += f->colE[row] * 2;
                v += next[-1];
                v += next[0] * 2;
                v += f->colE[row + 1];
                v >>= 4;
                v = (v * v) >> 8;
                if (v < 4) {
                    v = 4;
                }
                v ^= 0x80;
                *p = v;
                e++;
                f->toggle ^= 1;
            }
            if (e < 0x4000) {
                f->stack[f->sp++] = e | 0x1000000;
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
                    c01 = f->colE[y / 512];
                    c10 = f->rowE[x];
                    c11 = f->colE[128];
                } else {
                    c01 = f->colE[y / 512];
                    c10 = f->map[y2 | x];
                    c11 = f->colE[y2 / 512];
                }
            } else {
                if (y2 == 0x10000) {
                    c01 = f->map[y | x2];
                    c10 = f->rowE[x];
                    c11 = f->rowE[x2];
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

struct Gen {
    s32 x;
    s32 y;
    s32 pad8;
    s32 seed;
    volatile s32 ctl;
    s32 f14;
    s32 f18;
    s32 f1C;
    u8 pad20[0x114 - 0x20];
    u8 a[0x81];
    u8 b[0x81];
    u8 pad216[0x31C - 0x216];
    u8 *buf;
};

static inline s32 Seed(s32 x, s32 y)
{
    return ((x * (y + 123) * 456) << (x % 12634)) + y;
}

void fractal_tile_init(Gen *p, s32 x, s32 y)
{
    s32 i;
    s32 t;

    for (i = 0; i < 0x10000; i += 0x200) {
        mem_fill(p->buf + i, 0, 0x80);
    }
    p->f14 = 0x2000000;
    p->ctl = 0;
    p->ctl = 1;
    p->f18 = 0x1000000;
    p->ctl = 2;
    p->x = x;
    p->y = y;
    p->f1C = 7;
    t = Seed(x, y);
    p->ctl = 3;
    p->seed = t;
    p->buf[0] = fractal_corner_height(x, y);
    p->b[0] = fractal_corner_height(x + 1, y);
    p->b[0x80] = fractal_corner_height(x + 1, y + 1);
    p->a[0] = fractal_corner_height(x, y + 1);
    p->a[0x80] = p->b[0x80];
    sFractalRand = Seed(x, y + 1) + 1;
    fractal_subdivide_edge(p->a);
    sFractalRand = Seed(x + 1, y) + 2;
    fractal_subdivide_edge(p->b);
    sFractalRand = p->seed + 1;
    sFractalEdgeBuf[0] = p->buf[0];
    sFractalEdgeBuf[0x80] = p->b[0];
    fractal_subdivide_edge(sFractalEdgeBuf);
    for (i = 0; i < 0x80; i++) {
        p->buf[i] = sFractalEdgeBuf[i];
    }
    sFractalRand = p->seed + 2;
    sFractalEdgeBuf[0] = p->buf[0];
    sFractalEdgeBuf[0x80] = p->a[0];
    fractal_subdivide_edge(sFractalEdgeBuf);
    for (i = 0; i < 0x80; i++) {
        ((u8 (*)[0x200])p->buf)[i][0] = sFractalEdgeBuf[i];
    }
}

typedef struct Ent_41 {
    s32 a, b, c;
} Ent_41;

extern Ent_41 gDesertFires[];

void fractal_place_objects(volatile Field_2 *f)
{
    s32 t0, t1, t2, t3;
    s32 i, n, k, v, b, w;
    s32 *q;

    desert_export_checkpoints();
    n = 0;
    for (i = 0; i < gDesertFireCount; i++) {
        divmod_floor(gDesertFires[i].a, 0x3E800, &t0, &t1);
        if (t0 == f->x) {
            divmod_floor(gDesertFires[i].b, 0x3E800, &t2, &t3);
            if (t2 == f->z) {
                f->obj[n].x = gDesertFires[i].a;
                t1 = t1 / 2000;
                t3 = t3 / 2000;
                f->obj[n].val = f->map[t3 * 512 + t1] << 6;
                b = gDesertFires[i].b;
                f->obj[n].type = 2;
                w = f->obj[n].val;
                f->obj[n].idx = i;
                f->obj[n].f330 = 0;
                f->obj[n].f334 = 0;
                f->obj[n].z = b;
                n++;
                gDesertFires[i].c = w;
            }
        }
    }
    i = n;
    if (i < 8) {
        do {
            q = (s32 *)f + i * 7;
            do {
                k = 0;
                sFractalRand_2[0] = (sFractalRand_2[0] * 0x35DF + 0x3038) % 0xFFFB;
                t0 = (sFractalRand_2[0] & 0xFF) % 120 + 4;
                sFractalRand_2[0] = (sFractalRand_2[0] * 0x35DF + 0x3038) % 0xFFFB;
                t2 = (sFractalRand_2[0] & 0xFF) % 120 + 4;
                v = f->map[t2 * 512 + t0];
                t0 = t0 * 2000;
                t0 = t0 + f->x * 0x3E800;
                t2 = t2 * 2000;
                t2 = t2 + f->z * 0x3E800;
                v <<= 6;
                for (k = 0; k < i; k++) {
                    if (q[0xC9] == t0 && q[0xCB] == t2) {
                        break;
                    }
                }
            } while (k != i);
            q[0xCA] = v;
            q[0xC9] = t0;
            q[0xCB] = t2;
            q[0xCE] = (s32)random_byte() % 2;
            q[0xCC] = 400;
            q[0xCD] = 1000;
            if (i) k = 1; else k = 2;
            i++;
        } while (i < 8);
    }
    f->count = 8;
}

struct Elem_2 {
    s32 x, y, z, w, a, b, c;
};

typedef struct {
    u8 pad[0x10];
    s32 busy;
    u8 pad2[0x30C];
    s32 n;
    Elem_2 e[8];
} Group;

extern Group *gDesertTiles_4;

s32 fractal_get_visible_objects(s32 x, s32 z, s32 ang, s32 range, Elem_2 *out, s32 max)
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
        if (gDesertTiles_4[i].busy == 0) {
            for (j = 0; j < gDesertTiles_4[i].n; j++) {
                if (n >= max) {
                    break;
                }
                dx = (gDesertTiles_4[i].e[j].x - x) / 16;
                dz = (gDesertTiles_4[i].e[j].z - z) / 16;
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
                out[n++] = gDesertTiles_4[i].e[j];
            }
        }
    }
    return n;
}

typedef struct { s32 x, y, z, a, b, kind, ref; } Ent_42;
typedef struct { u8 pad[0x320]; s32 count; Ent_42 e[8]; } Tile_3;

extern Tile_3 *gDesertTiles_5;

void fractal_emit_fire_smoke(void)
{
    f32 pos[3];
    f32 vel[3];
    u8 col[4];
    s32 i, j;
    Ent_42 *p;

    if ((gRaceFrame & 7) != 0) {
        return;
    }
    col[0] = 0;
    col[1] = 0;
    col[2] = 0;
    col[3] = 0;
    for (i = 0; i < 16; i++) {
        for (j = 0; j < gDesertTiles_5[i].count; j++) {
            p = &gDesertTiles_5[i].e[j];
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

void fractal_init(s32 param_1) {
    s32 *dst = &gDesertTiles;
    sFractalMap = param_1;
    *dst = mem_alloc_dbg(0x4040, 0, (s32)&D_800D6FE0, 0x266);
}

void fractal_reset(void)
{
    s32 i;
    s32 off;
    u8 *p;
    u32 hi;

    mem_fill(gDesertTiles, 0, 4);
    i = 0xF;
    p = gDesertTiles + 0x3C3C;
    do {
        *(s32 *)p = -0x3E7;
        i = i - 1;
        p = p - 0x404;
    } while (i >= 0);
    fractal_update_tiles(0, 0);
    i = 0;
    hi = 0x800F0000;
    off = i;
    for (; i < 0x10; i++) {
        fractal_generate(*(u8 **)(hi - 0x1EC8) + off, -1);
        off = off + 0x404;
    }
}

typedef struct {
    u8 pad[0x10];
    s32 f10;
    u8 pad2[0x3F0];
} Ent_43;

extern Ent_43 *gDesertTiles_6;

void fractal_generate_step(s32 n)
{
    s32 i, cnt;

    cnt = 0;
    for (i = 0; i < 16; i++) {
        if (gDesertTiles_6[i].f10 != 0) {
            cnt++;
        }
    }
    if (cnt != 0) {
        n = (n + cnt - 1) / cnt;
        for (i = 0; i < 16; i++) {
            if (gDesertTiles_6[i].f10 != 0) {
                fractal_generate(&gDesertTiles_6[i], n);
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
