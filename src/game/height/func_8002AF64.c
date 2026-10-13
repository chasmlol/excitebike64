#include "ctx.h"

typedef struct { s32 x, y, z, a, b, c; } Loc;

typedef struct Ent {
    s32 f0;
    s32 f4;
    s32 f8;
    u8 pad0C[0x34 - 0xC];
    u16 f34;
    u8 pad36[0x48 - 0x36];
    f32 x, y, z;
    u8 pad54[0x5AA - 0x54];
    s16 f5AA;
    u8 pad5AC[0x638 - 0x5AC];
    f32 f638;
    u8 pad63C[0x7E0 - 0x63C];
} Ent;

typedef struct { f32 v[16]; } Row;

extern s32 D_800C966C[];
extern Ent D_800E37C0[];
extern s32 *D_800EFD64;
extern s32 D_800CB5D0[];
extern Loc D_800C9764;
extern f32 D_800EE480;
extern f32 D_800EE484;
extern s32 D_800E84F8;
extern Row D_800C9080[];
extern s32 D_800C9684;

void func_8001254C(s32 a, s32 b, s16 *ang, s32 *dist);
void func_8004AC98(Ent *p);
void func_80011A58(s32 *m, s32 a, s32 b, s32 c);
void func_80012DC8(s32 *a, s32 *m, s32 *b);
void func_8002B5F8(Loc *l);
void func_8004F064(s32 idx);

void func_8002AF64(Ent *p)
{
    Loc loc;
    s32 m[9];
    s32 v[3];
    s32 o[3];
    s16 ang;
    s32 dist;
    f32 f;
    s32 idx;

    if (p->f5AA != 9) {
        D_800C966C[p - D_800E37C0] = 9;
        p->f5AA = 9;
        if (D_800EFD64 == D_800CB5D0) {
            loc = D_800C9764;
            func_8001254C(loc.x - p->f0, p->f8 - loc.z, &ang, &dist);
            loc.c = ang;
        } else {
            func_8004AC98(p);
            func_8001254C(-(D_800EE480 * 1000.0f), D_800EE484 * 1000.0f, &ang, &dist);
            if (0.0f < p->f638) {
                ang += 0x4000;
            } else {
                ang -= 0x4000;
            }
            p->f34 = ang;
            func_80011A58(m, 0x1000, 0, ang);
            f = D_800C9080[(D_800E84F8 >> 4) & 0xF].v[7];
            if (-0.1f < f) {
                f = -0.1f;
            }
            v[0] = 0;
            v[1] = 0;
            f *= 100000.0f;
            v[2] = f;
            func_80012DC8(v, m, o);
            loc.x = (s32)(p->x * 1000.0f) + o[0];
            loc.y = (s32)(p->y * 1000.0f) + o[1] - 1000;
            loc.z = (s32)(p->z * 1000.0f) + o[2];
            loc.a = 0x1000;
            loc.b = 0;
            loc.c = ang;
        }
        D_800C9684 = idx = p - D_800E37C0;
        func_8002B5F8(&loc);
        func_8004F064(idx);
    }
}
