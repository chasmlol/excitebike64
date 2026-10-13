#include "ctx.h"

typedef struct {
    s32 x, y, z;
    s32 x2, y2, z2;
    u8 pad[0x7E0 - 0x18];
} Player;

typedef struct {
    s32 x, y, z;
} Pt3;

typedef struct {
    u8 pad[8];
    Pt3 v[3];
} Facet;

extern Player D_800E37C0[];
extern s32 D_800E8D68;
extern s32 D_800E30A8;
extern s32 D_800E92E0;
extern s32 D_800E9670;
extern Vtx *D_800E9628;
extern Facet D_800E80D8[];
extern s32 D_800E92D8;
extern Pt3 D_800E92E8[];
extern s32 D_800E93D8[];

void func_8002F62C(s32 a, s32 b, s32 c, s32 d, s32 e);
void func_8003BB1C(Facet *a, s32 b);

void func_8003B7E0(s32 n)
{
    s32 x, y, z;
    s32 i, j, k, cnt;
    Vtx *v;
    u32 vb;
    Pt3 *src;

    if (n < 8) {
        x = D_800E37C0[n].x;
        y = D_800E37C0[n].y;
        z = D_800E37C0[n].z;
    } else {
        x = D_800E37C0[n % 8].x2;
        y = D_800E37C0[n % 8].y2;
        z = D_800E37C0[n % 8].z2;
    }
    func_8002F62C(x - 1000, z - 1000, x + 1000, z + 1000, D_800E37C0[n].y + 1000);
    if (D_800E8D68 == 0) return;
    D_800E92E0 = 0;
    D_800E9670 = 0;
    k = D_800E30A8 * 0x480;
    k += n * 0x48;
    vb = (u32)D_800E9628 + k * 16;
    for (i = 0; i < D_800E8D68; i++) {
        for (j = 0; j < 3; j++) {
            D_800E80D8[i].v[j].x -= x;
            D_800E80D8[i].v[j].y -= y;
            D_800E80D8[i].v[j].z -= z;
        }
        func_8003BB1C(&D_800E80D8[i], 1000);
        cnt = D_800E9670 - 2;
        if (cnt + D_800E92D8 >= 0x48) return;
        if (D_800E92E0 + D_800E92D8 >= 0x20) return;
        j = D_800E92E0;
        for (k = 0; k < D_800E92D8; k++) {
            v = (Vtx *)(D_800E92E0 * 16 + vb);
            v->v.ob[0] = D_800E92E8[k].x;
            v->v.ob[1] = D_800E92E8[k].y;
            v->v.ob[2] = D_800E92E8[k].z;
            v->v.tc[0] = ((D_800E92E8[k].x + 1000) << 11) / 2000;
            D_800E92E0++;
            v->v.tc[1] = ((-D_800E92E8[k].z + 1000) << 11) / 2000;
        }
        for (k = 2; k < D_800E92D8; k++) {
            D_800E93D8[D_800E9670 * 3] = j;
            D_800E93D8[D_800E9670 * 3 + 1] = j + (k - 1);
            D_800E93D8[D_800E9670 * 3 + 2] = j + k;
            D_800E9670++;
        }
    }
}
