#include "ctx.h"

typedef struct {
    s32 x, y, z;
    s32 rx, ry, rz;
} Rec6;

typedef struct {
    s32 x, y, z;
    s32 rx, ry, rz;
    s32 flags;
    s32 p7, p8;
} Obj;

typedef struct {
    u16 flags;
    u16 pad;
    Obj *obj;
} Slot;

typedef struct {
    s32 x, y, z;
} Vec3i;

typedef struct {
    u8 pad[0x6C];
    f32 fx, fy, fz;
} Actor;

extern s32 D_800C9684;
extern Slot D_800E7FC8[];
extern Vec3i D_800E8090[];
extern Vec3i D_800C97E8;

void func_8001254C(s32, s32, s16 *, s32 *);
void func_8002B5F8(Rec6 *);
void func_80011A58(s32 *, s32, s32, s32);
void func_80012DC8(s32 *, s32 *, Vec3i *);
f32 func_8002BBD4(Actor *, s32, s32);

void func_8002BFE0(Actor *act, Rec6 *out)
{
    s32 m[3];
    s32 mtx[10];
    s16 a1;
    s32 r1;
    s16 a2;
    s16 b1;
    s32 r2;
    s16 b2;
    Slot *sl;
    s32 idx;
    Obj *o;
    u16 sf;
    s32 f;
    s32 rx, rz;
    f32 t, u, v;

    sl = D_800E7FC8;
    sl += D_800C9684;
    sf = sl->flags;
    o = sl->obj;
    if (sf & 4) {
        sl->flags = sf & ~4;
        f = o->flags;
        if (f & 8) {
            if (f & 1) {
                if (f & 4) {
                    o->x = (f32)D_800E8090[D_800C9684].x - act->fx * 1000.0f;
                    o->y = (f32)D_800E8090[D_800C9684].y - act->fy * 1000.0f;
                    o->z = (f32)D_800E8090[D_800C9684].z - act->fz * 1000.0f;
                } else {
                    o->x = D_800E8090[D_800C9684].x;
                    o->y = D_800E8090[D_800C9684].y;
                    o->z = D_800E8090[D_800C9684].z;
                }
            } else {
                o->x = D_800E8090[D_800C9684].x;
                o->y = D_800E8090[D_800C9684].y;
                o->z = D_800E8090[D_800C9684].z;
            }
            f = o->flags;
        }
        if (f & 0x10) {
            func_80011A58(mtx, o->rx, 0, o->rz);
            m[0] = 0;
            m[1] = 0;
            m[2] = o->ry;
            func_80012DC8(m, mtx, &D_800C97E8);
        } else if (f & 4) {
            func_8001254C(-o->x, o->z, &a1, &r1);
            func_8001254C(r1, -o->y + 700, &a2, &r1);
            func_80011A58(mtx, a2 - 0x4000, 0, -a1);
            m[0] = 0;
            m[1] = 0;
            m[2] = o->ry;
            func_80012DC8(m, mtx, &D_800C97E8);
        }
    }
    t = func_8002BBD4(act, o->p7, o->p8);
    f = o->flags;
    if (f & 4) {
        out->x = (f32)o->x + act->fx * 1000.0f;
        out->y = (f32)o->y + act->fy * 1000.0f;
        out->z = (f32)o->z + act->fz * 1000.0f;
        if (o->flags & 0x20) {
            u = 1.0f - t;
            out->x = (f32)out->x + (f32)D_800C97E8.x * u;
            out->y = (f32)out->y + (f32)D_800C97E8.y * u;
            out->z = (f32)out->z + (f32)D_800C97E8.z * u;
        } else {
            out->x = (f32)out->x + (f32)D_800C97E8.x * t;
            out->y = (f32)out->y + (f32)D_800C97E8.y * t;
            out->z = (f32)out->z + (f32)D_800C97E8.z * t;
        }
    } else if (f & 1) {
        out->x = (f32)o->x + (f32)(o->rx - o->x) * t;
        out->y = (f32)o->y + (f32)(o->ry - o->y) * t;
        out->z = (f32)o->z + (f32)(o->rz - o->z) * t;
    } else if (f & 0x10) {
        if (f & 0x20) {
            out->x = (f32)(o->x + D_800C97E8.x) - (f32)D_800C97E8.x * t;
            out->y = (f32)(o->y + D_800C97E8.y) - (f32)D_800C97E8.y * t;
            out->z = (f32)(o->z + D_800C97E8.z) - (f32)D_800C97E8.z * t;
        } else {
            out->x = (f32)o->x + (f32)D_800C97E8.x * t;
            out->y = (f32)o->y + (f32)D_800C97E8.y * t;
            out->z = (f32)o->z + (f32)D_800C97E8.z * t;
        }
    } else {
        out->x = o->x;
        out->y = o->y;
        out->z = o->z;
    }
    if (o->flags & 0x10) {
        out->rx = o->rx;
        out->ry = 0;
        out->rz = o->rz;
    } else {
        func_8001254C(act->fx * 1000.0f - (f32)out->x, act->fz * 1000.0f - (f32)out->z, &b1, &r2);
        func_8001254C(r2, (act->fy + 0.7f) * 1000.0f - (f32)out->y, &b2, &r2);
        out->ry = 0;
        out->rx = b2 - 0x4000;
        out->rz = -b1;
    }
    func_8002B5F8(out);
}
