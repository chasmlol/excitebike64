#include "ctx.h"

extern u8 *D_800EE498;
extern s32 D_800EE488;
typedef struct {
    u8 p0[0x626];
    s16 f626;
    u8 p1[0x660 - 0x628];
    s16 f660;
    u8 p2[0x6ED - 0x662];
    u8 f6ED;
    u8 p3[0x7A5 - 0x6EE];
    u8 f7A5;
    u8 p4[0x7E0 - 0x7A6];
} Ent;
extern Ent D_800E37C0[];
typedef struct { s32 a, b, c; } S12;
void func_80046CC0(void *p);

void func_80063B60(u8 *p1, s16 *p2, f32 p3, s32 *p4)
{
    s32 v4;
    s32 idx;
    s32 cnt;
    s32 i;
    s32 lim;
    u8 *r;

    v4 = *(s8 *)(p1 + 0x6ED) + *(s16 *)(p1 + 0x660);
    if (0.0f < p3) {
        idx = *(s16 *)(p1 + 0x628);
        while (0.0f < p3) {
            {
                s32 off = idx * 0x6c;
                r = (u8 *)(off + (s32)D_800EE498);
            }
            idx = *(s16 *)(r + 0x46);
            p3 = p3 - *(f32 *)(r + 0xc);
            if (idx == D_800EE488) {
                v4 = v4 + 1;
            }
        }
    } else {
        idx = *(s16 *)(p1 + 0x628);
        while (p3 < 0.0f) {
            {
                s32 off = idx * 0x6c;
                r = (u8 *)(off + (s32)D_800EE498);
            }
            idx = *(s16 *)(r + 0x4a);
            p3 = p3 + *(f32 *)(r + 0xc);
            if (idx == D_800EE488) {
                v4 = v4 - 1;
            }
        }
    }
    D_800E37C0[*p2].f660 = v4;
    D_800E37C0[*p2].f6ED = 0;
    D_800E37C0[*p2].f7A5 = 0;
    D_800E37C0[*p2].f626 = idx;
    func_80046CC0(&D_800E37C0[*p2]);
    cnt = *p4 - 1;
    *p4 = cnt;
    for (i = 0; i < *p4; i++) {
        ((S12 *)p2)[i] = ((S12 *)p2)[i + 1];
    }
}
