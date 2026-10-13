#include "ctx.h"

typedef struct {
    Vec3f pos;      /* 0x00 */
    f32 f0C;        /* 0x0C */
    f32 f10;        /* 0x10 */
    u8 pad14[0x18];
    f32 c2C;        /* 0x2C */
    f32 c30;
    f32 c34;
    f32 c38;
    f32 c3C;
    f32 c40;        /* 0x40 */
    u8 pad44[2];
    s16 prev;       /* 0x46 */
    u8 pad48[2];
    s16 next;       /* 0x4A */
    u8 pad4C[0xC];
    f32 f58;
    f32 f5C;
    u8 pad60[8];
    f32 f68;
} Node;

extern Node *D_800EE498[];
#define ND D_800EE498[0]
#define ND2 (*(Node **)(u32)D_800EE498)

f32 func_800228A0(Vec3f *p);
s32 func_8004B5F0(Vec3f *pos, s32 start, s32 count, u8 mode);
f32 func_8004A944(Vec3f *pos, s32 idx);
void func_80049AA4(s32 end, s32 i);

void func_80049D24(void)
{
    Vec3f pts[20];
    Vec3f pos;
    s32 cur, prv, nx, j, idx, i;
    u8 flag;
    Vec3f *pa, *pb;
    Node *e;
    f32 w;
    f32 t, t2, d, s, x, h, v;

    cur = 0;
    while (cur != ND[0].next) {
        j = ND[cur].next;
        prv = ND[cur].prev;
        {
        f32 bq = (ND[cur].pos.y - ND[j].pos.y) / ND[j].f0C;
        if (7.3500004f < -(((ND[prv].pos.y - ND[cur].pos.y) / ND[cur].f0C - bq) * 2.0f / (ND[j].f0C + ND[cur].f0C)) * (ND[cur].f68 * ND[cur].f68)) {
            for (i = 0; i < 10; i++) {
                t = (f32)i / 10.0f;
                pts[i] = ND[j].pos;
                e = ND;
                pts[i].x += e[j].c34 * t + e[j].c30 * (t * t) + e[j].c2C * (t * t * t);
                pts[i].z += e[j].c40 * t + e[j].c3C * (t * t) + e[j].c38 * (t * t * t);
                pts[i].y = func_800228A0(&pts[i]);
            }
            for (i = 0; i < 10; i++) {
                t = (f32)i / 10.0f;
                pts[i + 10] = ND[cur].pos;
                e = ND;
                pts[i + 10].x += e[cur].c34 * t + e[cur].c30 * (t * t) + e[cur].c2C * (t * t * t);
                pts[i + 10].z += e[cur].c40 * t + e[cur].c3C * (t * t) + e[cur].c38 * (t * t * t);
                pts[i + 10].y = func_800228A0(&pts[i + 10]);
            }
            for (i = 0; i < 18; i++) {
                if (pts[i].y < pts[i + 1].y && pts[i + 2].y < pts[i + 1].y) {
                    d = __builtin_sqrtf((pts[i + 1].x - pts[i].x) * (pts[i + 1].x - pts[i].x)
                                        + (pts[i + 1].z - pts[i].z) * (pts[i + 1].z - pts[i].z));
                    s = (pts[i + 1].y - pts[i].y) / d;
                    if (s > 1.0f) {
                        s = 1.0f;
                    }
                    flag = 0;
                    pb = &pts[i + 1];
                    pa = &pts[i];
                    while (!((x = ND2[cur].f10 * ND2[cur].f10 * 2.0f * s / 14.700001f) < 10.0f)) {
                        pos.x = pb->x + x * (pb->x - pa->x) / d;
                        pos.z = pb->z + x * (pb->z - pa->z) / d;
                        idx = func_8004B5F0(&pos, cur, 20, 2);
                        h = func_8004A944(&pos, idx) * 1000.0f;
                        if (-h < ND2[idx].f5C && h < ND2[idx].f58 && ND2[cur].f10 <= ND2[idx].f10) {
                            break;
                        }
                        w = ND2[cur].f10 * 0.95f;
                        ND2[cur].f10 = w;
                        flag = 1;
                    }
                    if (flag) {
                        j = cur;
                        do {
                            s = ND2[ND2[j].next].f10;
                            func_80049AA4(ND2[j].next, j);
                            nx = ND2[j].next;
                            j = nx;
                        } while (ND2[nx].f10 < s);
                    }
                }
            }
        }
        }
        cur = prv;
    }
}
