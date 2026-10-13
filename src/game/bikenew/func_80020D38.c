#include "ctx.h"

typedef struct { f32 x, y, z; } V3;
typedef struct { u8 pad[0x28]; s32 flags; } Sub;

typedef struct {
    u8 pad0[0x88];
    V3 pos;          /* 0x88 */
    u8 pad94[0xA0 - 0x94];
    V3 vel;          /* 0xA0 */
    V3 vAC;          /* 0xAC */
    u8 padB8[0x414 - 0xB8];
    s16 h414;
    s16 h416;
    u8 b418;
    u8 b419;
    u8 b41A;
    u8 b41B;
    u8 pad41C[4];
    f32 f420;
    f32 f424;
    u8 pad428[0x458 - 0x428];
    s32 w458;
    u8 pad45C[0x4C0 - 0x45C];
    V3 v4C0;
    u8 pad4CC[0x4E4 - 0x4CC];
    Sub *s4E4;
    u8 pad4E8[0x4FC - 0x4E8];
    s32 w4FC;
    u8 pad500[0x588 - 0x500];
    Sub *s588;
    u8 pad58C[0x5B0 - 0x58C];
    u32 btn;
    u32 prev;
    u8 pad5B8[0x5D0 - 0x5B8];
    s8 c5D0;
    s8 c5D1;
    s8 c5D2;
    u8 pad5D3[0x6E8 - 0x5D3];
    u8 b6E8;
    u8 pad6E9[0x744 - 0x6E9];
    s32 w744;
    u8 pad748[0x7A8 - 0x748];
    u8 b7A8;
} Ent;

void func_800445B0(Ent *e, s32 b);
f32 func_80013408(f32 *a, f32 *b);

void func_80020D38(Ent *e)
{
    s8 c;

    if (e->w744 > 0x3C && (*(s32 *)&e->b418 & 0xFFFF00) == 0) {
        e->h416 = 0;
    }
    if (e->h416 == 0) {
        if (e->w744 < 0) {
            if (e->w4FC < 0 && e->f420 > 7.5f && (e->s588->flags & 0x20)) {
                func_800445B0(e, 0x1C);
            }
            if ((e->btn & 0x20000) && !(e->prev & 0x20000) && abs(e->c5D0) < 3 &&
                e->f424 > -1.0f && e->c5D2 < -4) {
                e->h416 = 0x10;
                e->h414 = 0;
            }
        }
    } else {
        if (e->w458 <= 0) {
            if (e->s4E4->flags & 0x20) {
                e->h416 = 0xF;
            }
            if (e->w458 <= 0 && func_80013408(&e->vAC.x, &e->v4C0.x) < 0.5f) {
                func_800445B0(e, 0x19);
            }
        }
        if (e->h416 < 0x10) {
            e->h416--;
        } else {
            if (e->h416 >= 0x3D && e->w744 <= 0) {
                c = e->c5D2;
                if (c < -12) {
                    e->h414 += 0x20;
                } else if (c >= -3) {
                    e->h414 -= 0x20;
                }
                e->h414 -= e->h414 / 64;
                if (e->h414 > 0x400) {
                    func_800445B0(e, 0x19);
                    e->pos.x -= e->vel.x * 5.0f;
                    e->pos.y -= e->vel.y * 5.0f;
                    e->pos.z -= e->vel.z * 5.0f;
                } else if (e->h414 < -0x400) {
                    e->h416 = 0xF;
                }
                if ((e->c5D2 < -12 || e->c5D2 > -4) && (f32)e->w744 < 3.0f) {
                    if (e->b41A < 0x10) e->b41A++;
                    if (e->b419 < 0x10) e->b419++;
                } else {
                    if (e->b41A != 0) e->b41A--;
                    if (e->b419 != 0) e->b419--;
                }
            } else {
                e->b419 = 0;
                e->b41A = 0;
            }
            e->h416++;
            if (e->h416 == 0xF0) {
                e->b7A8 = 1;
            }
            if (e->c5D2 >= 0) {
                e->h416 = 0xF;
            }
        }
    }
    if (e->b6E8 && e->h416 != 0) {
        e->h416 = 0;
    }
}
