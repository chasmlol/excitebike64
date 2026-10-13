#include "ctx.h"

typedef struct {
    u8 pad0[0x48];
    f32 f48;
    f32 f4C;
    f32 f50;
    u8 pad54[0x7E0 - 0x54];
} Bike;

typedef struct {
    s32 x, y, z;
    s32 rx, ry, rz;
} Rec6;

typedef struct {
    u8 pad0[0x58];
    s32 f58;
    u8 pad5C[0x6C - 0x5C];
    s16 f6C;
} State;

extern State D_800F05A8;
extern Bike D_800E37C0[];
extern s32 D_800EFDAC;
extern Vec3f D_800F1510;
extern s16 D_800C2C60[];

extern void func_8000ECB8(s32) __attribute__((noreturn));
s32 func_8002B5F8(Rec6 *src);
void func_8001254C(s32 x, s32 y, u16 *pAng, s32 *pRatio);
s32 func_8002A8C0(Bike *p, Rec6 *out, s32 z);

#define COST(i) D_800C2C60[(((i) >> 4) - 0x400) & 0xFFF]
#define SINT(i) D_800C2C60[((i) >> 4) & 0xFFF]

s32 func_80029CE0(Bike *bike, Rec6 *out) {
    u16 a1;
    s32 ratio;
    u16 a2;
    f32 s;
    f32 fz;
    s32 n;
    s32 z;
    s32 i;
    s32 ang, q, ang3, q3;

    n = D_800F05A8.f58 & 0xF;
    if (n != 0) {
    i = n - 1;
    if (i == ((bike - D_800E37C0) >> ((D_800EFDAC - 1) >> 1))) {
        switch (n) {
        case 1:
            s = 1000.0f;
            q = D_800F05A8.f6C * 10500 / 180;
            { s32 r = 0x4844; s16 *tb = D_800C2C60;
            ang = D_800F05A8.f6C * 15000 / 180 - 8000;
            out->x = D_800F1510.x * s + (f32)(((r - q) * tb[(((ang) >> 4) - 0x400) & 0xFFF]) >> 14); }
            out->y = 6000.0f - (f32)D_800F05A8.f6C * s * 3.0f / 180.0f;
            fz = D_800F1510.z * s - (f32)(((0x4844 - D_800F05A8.f6C * 10500 / 180) * SINT(D_800F05A8.f6C * 15000 / 180 - 8000)) >> 14);
            out->z = z = fz;
            func_8001254C(D_800F1510.x * s - (f32)out->x, D_800F1510.z * s - (f32)z, &a1, &ratio);
            func_8001254C(ratio, D_800F1510.y * s - (f32)out->y, &a2, &ratio);
            out->ry = 0;
            out->rx = a2 - 0x4000;
            out->rz = -a1;
            break;
        case 2:
            s = 1000.0f;
            q3 = D_800F05A8.f6C * 10500 / 180;
            { s32 r = 0x4844; s16 *tb = D_800C2C60;
            ang3 = D_800F05A8.f6C * 15000 / 180 - 8000;
            out->x = D_800F1510.x * s - (f32)(((r - q3) * tb[(((ang3) >> 4) - 0x400) & 0xFFF]) >> 14); }
            out->y = 6000.0f - (f32)D_800F05A8.f6C * s * 3.0f / 180.0f;
            fz = D_800F1510.z * s + (f32)(((0x4844 - D_800F05A8.f6C * 10500 / 180) * SINT(D_800F05A8.f6C * 15000 / 180 - 8000)) >> 14);
            out->z = z = fz;
            func_8001254C(D_800F1510.x * s - (f32)out->x, D_800F1510.z * s - (f32)z, &a1, &ratio);
            func_8001254C(ratio, D_800F1510.y * s - (f32)out->y, &a2, &ratio);
            out->ry = 0;
            out->rx = a2 - 0x4000;
            out->rz = -a1;
            break;
        default:
            func_8000ECB8(1);
        }
    } else switch (n) {
    case 1: {
        Bike *p = &D_800E37C0[i];
        out->x = p->f48 * 1000.0f + (f32)((COST(D_800F05A8.f6C * 14000 / 180 - 7000) * 625) >> 11);
        out->y = 1000;
        out->z = p->f50 * 1000.0f - (f32)((SINT(D_800F05A8.f6C * 14000 / 180 - 7000) * 625) >> 11);
        out->rx = 1000;
        out->rz = D_800F05A8.f6C * 14000 / 180 - 7000;
        out->ry = 0;
        break; }
    case 2: {
        Bike *p = &D_800E37C0[i];
        out->x = p->f48 * 1000.0f + (f32)((COST(7000 - D_800F05A8.f6C * 14000 / 180) * 625) >> 11);
        out->y = 1000;
        out->z = p->f50 * 1000.0f + (f32)((SINT(7000 - D_800F05A8.f6C * 14000 / 180) * 625) >> 11);
        out->rx = 1000;
        out->rz = D_800F05A8.f6C * 14000 / 180 + 0x64A8;
        out->ry = 0;
        break; }
    }
    return func_8002B5F8(out);
    } else {
        return func_8002A8C0(bike, out, -7000);
    }
}
