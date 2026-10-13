#include "ctx.h"

typedef struct {
    void *hdr;
    s32 x0, z0, x1, z1;
    void *grid;
    s32 f18;
    s32 f1C;
} Sub;

typedef struct {
    u32 count;
    Sub subs[1];
} Area;

typedef struct {
    s16 x, z;
} Pt;

typedef struct {
    u16 f0, f2, f4;
    u16 n;
    Pt pts[1];
} Poly;

typedef struct {
    u32 count;
} PolyHdr;

extern Area *D_800E8948;
extern s32 D_800E8D6C;
extern s32 D_800E8958;
extern s32 D_800E8940[2];
extern s32 D_800E8950[2];
extern u16 *D_800C29E0;
extern char D_800D6C20[];
extern u16 *D_800E8510;
typedef struct { u16 n; u16 ids[1]; } List;
extern List **D_800E8D60;
extern Poly **D_800E8E6C;
extern s16 **D_800E8500;

void func_8000ED10(u8 *a0, u8 a1, s32 a2);
void *func_800BC908(s32 size, u32 flags, s32 owner, s32 tag);
s32 func_8002E29C(s32 id, s32 x, s32 y, s32 z);
void func_800AEC34(void *dst, void *src, s32 n);

#define SX(v) (((s32)(v) << 4) >> 4)

void func_8002DA60(Area *area, s32 b, s32 c, u32 *polys)
{
    u32 i;
    s32 n;
    s32 cell;
    s32 x, z;
    s32 j, k, total;
    u16 *out;
    Sub *s;
    Area *r;
    Poly *p;
    Poly **tbl;
    s16 *buf;

    D_800E8948 = area;
    D_800E8D6C = c;
    D_800E8958 = -1;
    if (area != NULL) {
        for (i = 0; i < D_800E8948->count; i++) {
            s = (Sub *)((u8 *)D_800E8948 + (i * 32 + 4));
            if (s->x0 < D_800E8940[0] || i == 0) D_800E8940[0] = s->x0;
            if (s->z0 < D_800E8940[1] || i == 0) D_800E8940[1] = s->z0;
            if (s->x1 > D_800E8950[0] || i == 0) D_800E8950[0] = s->x1;
            if (s->z1 > D_800E8950[1] || i == 0) D_800E8950[1] = s->z1;
        }
        func_8000ED10((u8 *)D_800C29E0, 0, 0x200);
        i = 0;
        total = 0;
        out = D_800C29E0 + 0x100;
        for (k = 0; k < 16; k++) {
            for (j = 0; j < 16; j++) {
                for (n = 0; n < D_800E8948->count; n++) {
                    r = (Area *)((u8 *)D_800E8948 + n * 32);
                    x = r->subs[0].x0 << 4;
                    x >>= 4;
                    x = ((x - D_800E8940[0]) << 4) / (D_800E8950[0] - D_800E8940[0]);
                    z = r->subs[0].z0 << 4;
                    z >>= 4;
                    if (x < 0) x = 0;
                    if (x >= 16) x = 15;
                    z = ((z - D_800E8940[0]) << 4) / (D_800E8950[1] - D_800E8940[1]);
                    if (z < 0) z = 0;
                    if (z >= 16) z = 15;
                    cell = z * 16 + x;
                    z = cell >> 4;
                    if ((cell & 0xF) > j) continue;
                    if (z > k) continue;
                    z = r->subs[0].x1 << 4;
                    z >>= 4;
                    z = ((z - D_800E8940[0]) << 4) / (D_800E8950[0] - D_800E8940[0]);
                    x = r->subs[0].z1 << 4;
                    x >>= 4;
                    if (z < 0) z = 0;
                    if (z >= 16) z = 15;
                    x = ((x - D_800E8940[0]) << 4) / (D_800E8950[1] - D_800E8940[1]);
                    if (x < 0) x = 0;
                    if (x >= 16) x = 15;
                    cell = x * 16 + z;
                    z = cell >> 4;
                    if ((cell & 0xF) < j) continue;
                    if (z < k) continue;
                    D_800C29E0[i]++;
                    total++;
                    *out++ = n;
                }
                i++;
            }
        }
        D_800E8510 = func_800BC908((total + 0x100) * 2, 0, (s32)D_800D6C20, 0xDA);
        D_800E8D60 = func_800BC908(0x400, 0, (s32)D_800D6C20, 0xDB);
        n = 0;
        out = D_800C29E0 + 0x100;
        for (i = 0; (s32)i < 0x100; i++) {
            D_800E8D60[i] = (List *)(D_800E8510 + n);
            D_800E8D60[i]->n = D_800C29E0[i];
            func_800AEC34(D_800E8D60[i]->ids, out, D_800E8D60[i]->n * 2);
            n++;
            n += D_800E8D60[i]->n;
            out += D_800E8D60[i]->n;
        }
    } else {
        D_800E8510 = NULL;
        D_800E8D60 = NULL;
    }
    D_800E8E6C = NULL;
    D_800E8500 = NULL;
    if (polys != NULL) {
        D_800E8E6C = func_800BC908(*polys * 4, 0, (s32)D_800D6C20, 0xF6);
        i = 0;
        p = (Poly *)(polys + 1);
        z = *polys;
        if (z != 0) {
            do {
                D_800E8E6C[i] = p;
                p = (Poly *)((u8 *)p + (p->n * 4 + 8));
            } while (++i < *polys);
        }
        func_8000ED10((u8 *)D_800C29E0, 0, D_800E8948->count * 2);
        i = 0;
        out = D_800C29E0 + D_800E8948->count;
        for (; i < D_800E8948->count; i++) {
            for (cell = 0; cell < *polys; cell++) {
                p = D_800E8E6C[cell];
                for (n = 0; n < p->n; n++) {
                    if (func_8002E29C(i, p->pts[n].x << 6, 0, p->pts[n].z << 6)) break;
                }
                if (n != p->n) {
                    *out++ = cell;
                    D_800C29E0[i]++;
                }
            }
        }
        i = 0;
        D_800E8500 = func_800BC908(D_800E8948->count * 4, 0, (s32)D_800D6C20, 0x11C);
        func_8000ED10((u8 *)D_800E8500, 0, D_800E8948->count * 4);
        out = D_800C29E0 + D_800E8948->count;
        for (; i < D_800E8948->count; i++) {
            if (D_800C29E0[i] != 0) {
                buf = func_800BC908((D_800C29E0[i] + 1) * 2, 0, (s32)D_800D6C20, 0x123);
                D_800E8500[i] = buf;
                func_800AEC34(buf, out, D_800C29E0[i] * 2);
                out += D_800C29E0[i];
                D_800E8500[i][D_800C29E0[i]] = -1;
            }
        }
    }
}
