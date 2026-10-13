#include "ctx.h"

typedef struct {
    f32 x, y, z;
    u8 padC[0x14 - 0xC];
    f32 f14;
    f32 f18;
    u8 pad1C[0x46 - 0x1C];
    s16 f46;
    u8 pad48[0x58 - 0x48];
    f32 f58;
    f32 f5C;
    u8 pad60[0x6C - 0x60];
} Node;

typedef struct { f32 x, z; } Vec2f;
typedef struct { s32 sx, sy; u8 data[0x1000]; } MapImg;
typedef struct { u16 a, b; } Pal;
typedef struct { u8 pad[0x4C]; s32 f4C; } Race;

extern u8 D_800F1478[];
extern u8 D_800CCCF8[];
extern MapImg *D_800F1458;
extern char D_800D8DCC[];
extern s32 D_800EE490;
extern s32 D_800EE494;
extern Node *D_800EE498;
extern Vec2f D_800F1468;
extern Vec2f D_800F1470;
extern u16 *D_800C29E0;
extern Race D_800EFD70;
extern Pal D_800CCD20[];

void func_8000ED10(u8 *a0, u8 a1, s32 a2);
void *func_800BC908(s32 size, u32 flags, s32 owner, s32 tag);

static inline void getpos(Node *n, f32 *x, f32 *z) {
    f32 h = (n->f58 - n->f5C) * 0.5f;
    *x = n->x - n->f14 * h;
    *z = n->z - n->f18 * h;
}

static inline f32 dist(f32 x, f32 y) {
    x -= 7.5f;
    y -= 7.5f;
    return __builtin_sqrtf(x * x + y * y);
}

static inline void stamp(u16 *brush, u16 *map, s32 cx, s32 cy) {
    s32 x, y, k;
    k = 0;
    for (y = cy - 8; y < cy + 8; y++) {
        for (x = cx - 8; x < cx + 8; x++) {
            if (y >= 0 && x >= 0 && y < 128 && x < 128) {
                if (brush[k] < map[y * 128 + x]) map[y * 128 + x] = brush[k];
            }
            k++;
        }
    }
}

void func_80069158(void) {
    f32 px, pz;
    f32 mx, my;
    f32 sx, sy, r, len, dx, dz, x0f;
    s32 x1, y1;
    s32 i, j, steps, x0, y0, ddx, ddy, ax, ay, t, cx, cy, x, y, k;
    u16 *brush;
    u16 *map;
    u16 *q;
    u16 *pp;
    f32 *pa, *pb;
    Node *n;
    Node *m;

    for (i = 0; i < 64; i++) {
        D_800F1478[i] = D_800CCCF8[i / 8];
    }
    D_800F1458 = func_800BC908(0x1003, 0, (s32)D_800D8DCC, 0x23E);
    for (i = 0; i < D_800EE490; i++) {
        if (D_800EE498[i].x < D_800F1468.x || i == 0) D_800F1468.x = D_800EE498[i].x;
        if (D_800EE498[i].z < D_800F1468.z || i == 0) D_800F1468.z = D_800EE498[i].z;
        if (D_800EE498[i].x > D_800F1470.x || i == 0) D_800F1470.x = D_800EE498[i].x;
        if (D_800EE498[i].z > D_800F1470.z || i == 0) D_800F1470.z = D_800EE498[i].z;
    }
    mx = (D_800F1468.x + D_800F1470.x) * 0.5f;
    my = (D_800F1468.z + D_800F1470.z) * 0.5f;
    sx = 118.0f / (D_800F1470.x - D_800F1468.x);
    sy = 118.0f / (D_800F1470.z - D_800F1468.z);
    if (sx < sy) sy = sx; else sx = sy;
    r = sx / sy;
    if (r > 0.8f && r < 1.2f) {
        sx /= 1.3f;
        sy /= 1.3f;
    }
    D_800F1458->sx = sx * 16384.0f;
    D_800F1458->sy = sy * 16384.0f;
    steps = sx * 6.0f;
    if (steps < 2) steps = 2;

    brush = D_800C29E0;
    for (i = 0; i < 16; i++) {
        for (j = 0; j < 16; j++) {
            brush[i * 16 + j] = (u32)(dist(i, j) * 256.0f);
        }
    }
    map = D_800C29E0 + 0x100;
    func_8000ED10((u8 *)map, 0xFF, 0x8000);

    for (i = 0; i < D_800EE490; i++) {
        n = (Node *)(i * sizeof(Node) + (u32)D_800EE498);
        j = n->f46;
        if (j == 0 && D_800EFD70.f4C == 7) break;
        getpos(n, &px, &pz);
        x0 = (px - mx) * sx + 64.0f;
        y0 = 64.0f - (pz - my) * sy;
        getpos(&D_800EE498[j], &px, &pz);
        cx = (px - mx) * sx + 64.0f;
        y1 = 64.0f - (pz - my) * sy;
        for (t = 0; t < steps; t++) {
            stamp(brush, map, x0 + t * (cx - x0) / steps, y0 + t * (y1 - y0) / steps);
        }
    }

    j = D_800EE498[D_800EE494].f46;
    x0 = (D_800EE498[D_800EE494].x - mx) * sx + 64.0f;
    y0 = 64.0f - (D_800EE498[D_800EE494].z - my) * sy;
    px = D_800EE498[j].x - D_800EE498[D_800EE494].x;
    pz = D_800EE498[j].z - D_800EE498[D_800EE494].z;
    len = __builtin_sqrtf(px * px + pz * pz);
    if (len > 0.0f) {
        px /= len;
        pz /= len;
    }
    pz = -pz;
    dz = pz;
    dx = px;
    for (i = -8; i < 9; i++) {
        cx = x0 + (s32)(i * dz);
        cy = y0 - (s32)(i * dx);
        {
            s32 xx, yy;
            t = 0;
            for (yy = cy - 8; yy < cy + 8; yy++) {
                for (xx = cx - 8; xx < cx + 8; xx++) {
                    if (yy >= 0 && xx >= 0 && yy < 128 && xx < 128) {
                        if (brush[t] < map[yy * 128 + xx]) map[yy * 128 + xx] = brush[t];
                    }
                    t++;
                }
            }
        }
    }

    for (i = 0; i < 0x4000; i++) {
        t = map[i] >> 7;
        if (t >= 16) t = 15;
        map[i] = D_800CCD20[t].b;
    }
    k = x = 0;
    for (i = 0; i < 128; i += 2) {
        for (j = 0; j < 128; j += 2) {
            u16 a = map[x], b = map[x + 1], cc = map[x + 128], d = map[x + 129];
            s32 lo;
            x += 2;
            t = (a >> 4) + (b >> 4) + (cc >> 4) + (d >> 4);
            lo = (a & 15) + (b & 15) + (cc & 15) + (d & 15);
            t >>= 2;
            lo >>= 2;
            D_800F1458->data[k] = (t << 4) | lo;
            k++;
        }
        x += 128;
    }
}
