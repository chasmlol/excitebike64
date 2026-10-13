#include "ctx.h"

typedef struct Obj {
    s32 x;      /* 0x324 */
    s32 val;    /* 0x328 */
    s32 z;      /* 0x32C */
    s32 f330;
    s32 f334;
    s32 type;   /* 0x338 */
    s32 idx;    /* 0x33C */
} Obj;

typedef struct Field {
    s32 x;
    s32 z;
    u8 pad[0x314];
    s8 *map;    /* 0x31C */
    s32 count;  /* 0x320 */
    Obj obj[8]; /* 0x324 */
} Field;

typedef struct Ent {
    s32 a, b, c;
} Ent;

extern s32 D_800CA410;
extern Ent D_800EE068[];
extern s32 D_800EE128[];

void func_80047A1C(void);
void func_8004262C(s32 a, s32 b, s32 *q, s32 *r);
u32 func_8000EFE8(void);

void func_80043B60(volatile Field *f)
{
    s32 t0, t1, t2, t3;
    s32 i, n, k, v, b, w;
    s32 *q;

    func_80047A1C();
    n = 0;
    for (i = 0; i < D_800CA410; i++) {
        func_8004262C(D_800EE068[i].a, 0x3E800, &t0, &t1);
        if (t0 == f->x) {
            func_8004262C(D_800EE068[i].b, 0x3E800, &t2, &t3);
            if (t2 == f->z) {
                f->obj[n].x = D_800EE068[i].a;
                t1 = t1 / 2000;
                t3 = t3 / 2000;
                f->obj[n].val = f->map[t3 * 512 + t1] << 6;
                b = D_800EE068[i].b;
                f->obj[n].type = 2;
                w = f->obj[n].val;
                f->obj[n].idx = i;
                f->obj[n].f330 = 0;
                f->obj[n].f334 = 0;
                f->obj[n].z = b;
                n++;
                D_800EE068[i].c = w;
            }
        }
    }
    i = n;
    if (i < 8) {
        do {
            q = (s32 *)f + i * 7;
            do {
                k = 0;
                D_800EE128[0] = (D_800EE128[0] * 0x35DF + 0x3038) % 0xFFFB;
                t0 = (D_800EE128[0] & 0xFF) % 120 + 4;
                D_800EE128[0] = (D_800EE128[0] * 0x35DF + 0x3038) % 0xFFFB;
                t2 = (D_800EE128[0] & 0xFF) % 120 + 4;
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
            q[0xCE] = (s32)func_8000EFE8() % 2;
            q[0xCC] = 400;
            q[0xCD] = 1000;
            if (i) k = 1; else k = 2;
            i++;
        } while (i < 8);
    }
    f->count = 8;
}
