#include "ctx.h"

typedef struct {
    u32 nv;
    Vtx *v;
    u32 ntri;
    u32 *tri;
    s32 x10;
    s32 x14;
} Mesh;

typedef struct {
    Mesh *mesh;
    s32 a, b, c, d;
    s16 *tex;
    s32 e, f;
} Entry;

typedef struct {
    u32 n;
    Entry e[1];
} Set;

typedef struct {
    u8 pad00[0x02];
    s16 h02;
    u8 pad04[0x08];
    u16 flags;
    u8 pad0E[0x12];
    u32 *p20;
    u32 *p24;
    u32 *p28;
    u32 *p2C;
    Set *s30;
    Set *s34;
    s32 r38;
    s32 r3C;
} Ent;

typedef struct {
    u8 pad[8];
    u32 *p8;
    u8 pad2[4];
    s32 r10;
} Hdr;

typedef struct {
    u8 pad[0xC];
    u8 cnt[8];
    u8 rec[1][0x1E];
} Tab;

extern u32 D_800D0680;
extern u8 D_800DB04C[], D_800DB050[], D_800DB054[], D_800DB040[];
extern u8 D_800248EC[], D_80031D08[], D_800F5CC0[];
extern Hdr D_800CE26C;
extern Ent D_800CE280[];
extern Tab *D_800C9570;

s32 func_800BF79C(u32 a0, u32 a1);
s32 func_800BF530(u8 *path, u32 *out);
void func_800BFC40(u8 *a0, s32 a1);
void func_80031D24(void);
void func_8000ED10(u8 *a0, u8 a1, s32 a2);
void *func_800BC908(s32 size, u32 flags, u8 *owner, s32 tag);
void func_800A5484(Set *a0, Set *a1, s32 a2);
void func_800A4FF0(Set *a0, Set *a1, u8 *a2, s32 a3);

void func_800A4BEC(void)
{
    s32 i;
    u32 j, k;
    s32 c;
    s32 n;

    func_800BFC40(D_800DB04C, (s32)D_800248EC);
    func_800BFC40(D_800DB050, (s32)D_80031D08);
    func_800BF530(D_800DB054, &D_800D0680);
    D_800CE26C.r10 = func_800BF79C(D_800D0680, *D_800CE26C.p8);
    func_80031D24();
    func_8000ED10(D_800F5CC0, 0, 0x2C);

    for (i = 0; i < 29; i++) {
        D_800CE280[i].r38 = 0;
        D_800CE280[i].r3C = 0;
        if (!(D_800CE280[i].flags & 0x40)) {
        D_800CE280[i].s30 = 0;
        D_800CE280[i].s34 = 0;
        if (D_800CE280[i].p20 != 0) {
            D_800CE280[i].s30 = (Set *)func_800BF79C(D_800D0680, *D_800CE280[i].p20);
        }
        if (D_800CE280[i].p24 != 0) {
            D_800CE280[i].s34 = (Set *)func_800BF79C(D_800D0680, *D_800CE280[i].p24);
        }
        if (D_800CE280[i].p20 != 0 && (j = 0, j < D_800CE280[i].s30->n)) {
            do {
                Mesh *m = D_800CE280[i].s30->e[j].mesh;
                for (k = 0; k < m->nv; k++) {
                    s32 l;
                    for (l = 0; l < 3; l += 2) {
                        s32 v = (s16)m->v[k].v.ob[l];
                        if (v < 0) {
                            c = -v;
                            v = c / 25;
                            v += (c % 25 >= 13);
                            v *= 25;
                            v = -v;
                        } else {
                            v = v / 25 + (v % 25 >= 13);
                            v *= 25;
                        }
                        m->v[k].v.ob[l] = v;
                    }
                }
                j++;
            } while (j < D_800CE280[i].s30->n);
        }
        if (D_800CE280[i].p28 != 0) {
            D_800CE280[i].r38 = func_800BF79C(D_800D0680, *D_800CE280[i].p28) + 4;
        }
        if (D_800CE280[i].p2C != 0) {
            D_800CE280[i].r3C = func_800BF79C(D_800D0680, *D_800CE280[i].p2C);
            func_80031D24();
        }
        }
    }

    for (i = 18, n = 0; i < 30; i++, n++) {
        if (!(D_800CE280[i].flags & 0x40)) continue;
        D_800CE280[i].s30 = func_800BC908(D_800C9570->cnt[n] << 6 | 4, 0, D_800DB040, 0x27F);
        D_800CE280[i].s34 = func_800BC908(D_800C9570->cnt[n] << 6 | 4, 0, D_800DB040, 0x280);
        D_800CE280[i].h02 = D_800C9570->cnt[n];
        if (D_800CE280[i].p28 != 0) {
            D_800CE280[i].r38 = func_800BF79C(D_800D0680, *D_800CE280[i].p28) + 4;
        }
        if (D_800CE280[i].p2C != 0) {
            D_800CE280[i].r3C = func_800BF79C(D_800D0680, *D_800CE280[i].p2C);
            func_80031D24();
        }
        func_800A5484(D_800CE280[i].s30, D_800CE280[i].s34, D_800C9570->cnt[n]);
        func_800A4FF0(D_800CE280[i].s30, D_800CE280[i].s34, D_800C9570->rec[n], D_800C9570->cnt[n]);
        D_800CE280[i].h02 = D_800C9570->cnt[n];
    }
}
