#include "ctx.h"

typedef struct {
    f32 x, y, z;
} Vec3;

typedef struct {
    u8 pad0[0x28];
    u32 flags;
} Model;

typedef struct {
    u8 pad0[4];
    s32 f458;
    u8 pad8[0x40 - 8];
    Vec3 pos;
} Sub;

typedef struct {
    u8 pad0[0x34];
    u16 ang;
    u8 pad36[0x44 - 0x36];
    u8 f44;
    u8 pad45[0x54 - 0x45];
    Vec3 f54;
    Vec3 vel;
    u8 pad6C[0xB8 - 0x6C];
    Vec3 fB8;
    u8 padC4[0x420 - 0xC4];
    f32 f420;
    u8 pad424[0x454 - 0x424];
    Sub sub;
    u8 pad4A0[0x4E4 - 0x4A0];
    Model *model;
    u8 pad4E8[0x5B0 - 0x4E8];
    u32 f5B0;
    u8 pad5B4[4];
    s32 f5B8;
    u8 pad5BC[0x5CF - 0x5BC];
    u8 f5CF;
    u8 pad5D0[0x6E8 - 0x5D0];
    u8 f6E8;
    u8 pad6E9[0x774 - 0x6E9];
    s32 f774;
    u8 pad778[0x7A5 - 0x778];
    u8 f7A5;
    u8 pad7A6[0x7E0 - 0x7A6];
} Bike;

typedef struct {
    u8 b[4];
} Col;

extern s32 D_800EE140;
extern s32 D_800E1898;
extern s32 D_800E3710;
extern s32 D_800C999C;
extern Col D_800DC0E8[];
extern Bike D_800E37C0[];
extern s32 D_800E18A0[];
extern Col D_800E1B50[];
extern s32 D_800E76D0;
extern u32 D_800F05A8;
extern u16 D_800E7FC8;
extern s16 D_800C2C60[];

u32 func_8000EFE8(void);
void func_80036374(s32 type, s32 b, Vec3 *pos, Vec3 *vel, u8 *col);

void func_800368A4(void)
{
    Vec3 pad0;
    Vec3 A;
    Vec3 B;
    Vec3 pad1;
    Vec3 pad2;
    s32 i, j;
    s32 bA;
    s32 bB, bC;
    s32 ang;
    f32 s, c, d;
    f32 kr;

    if (D_800EE140 != 0 || D_800E1898 >= 4 || D_800E3710 != 0) {
        return;
    }
    if (++D_800C999C & 3) {
        return;
    }
    kr = 12800.0f;
    for (i = 0; i < 8; i++) {
        Bike *b = &D_800E37C0[i];
        if (b->f7A5 != 0) continue;
        if (D_800E18A0[i] == 0) continue;
        bA = D_800E1B50[i].b[3] >> 4;
        if (b->f44 != 0) continue;
        if (b->sub.f458 <= 0 && (b->model->flags & 2)) {
            A.x = 0.0f;
            A.y = 0.0f;
            A.z = 0.0f;
            B = b->sub.pos;
            B.y += 0.3f;
            if (b->f420 < 8.0f) {
                B.x -= (8.0f - b->f420) * b->fB8.x * 0.125f;
                B.z -= (8.0f - b->f420) * b->fB8.z * 0.125f;
            }
            bC = 0;
            bB = 0;
            if ((b->f5B0 & 0x20000) || b->f5CF == 1) {
                bB = 1;
            }
            if (D_800E76D0 - b->f774 < 15) {
                bC = 1;
            } else if (D_800E76D0 - b->f5B8 < 10) {
                bC = 1;
            } else if (b->f5CF == 2) {
                bC = 1;
            }
            if (bB && (D_800E7FC8 & 1)) {
                bC = 1;
            }
            if (bC) {
                if (D_800F05A8 >= 5 || (D_800E76D0 & 1)) {
                    A.y = 0.07f;
                    A.z += (s32)(func_8000EFE8() - 0x80) / kr;
                    A.x += (s32)(func_8000EFE8() - 0x80) / kr;
                    ang = (s16)b->ang >> 4;
                    s = -D_800C2C60[(ang - 0x400) & 0xFFF] * 6.1035156e-05f;
                    c = D_800C2C60[ang & 0xFFF] * 6.1035156e-05f;
                    d = b->vel.x * c - b->vel.z * s;
                    A.x -= c * d / 250.0f;
                    A.z += s * d / 250.0f;
                    func_80036374(0, bA, &B, &A, D_800DC0E8[i].b);
                }
            } else if (bB) {
                func_80036374(0, bA, &B, &A, D_800DC0E8[i].b);
            }
        }
        if (((b->f5B0 & 0x20000) || D_800E76D0 - b->f774 < 10) && (b->model->flags & 4)) {
            if (b->f6E8 == 0 || (func_8000EFE8() & 1)) {
                for (j = 0; j < 2; j++) {
                    Sub *sp;
                    A = D_800E37C0[i].f54;
                    A.x = A.x / 80.0f + (((s32)func_8000EFE8() - 128.0f) * 0.01f) * 0.0078125f;
                    A.y = ((s32)func_8000EFE8() / 50.0f + 5.0f) / 100.0f;
                    A.z = A.z / 80.0f + (((s32)func_8000EFE8() - 128.0f) * 0.01f) * 0.0078125f;
                    sp = &D_800E37C0[i].sub;
                    func_80036374(1, bA, &sp->pos, &A, D_800DC0E8[i].b);
                }
            }
        }
    }
}
