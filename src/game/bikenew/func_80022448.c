#include "ctx.h"

typedef struct {
    s32 pos[3];          /* 0x000 */
    u8 pad0C[0x48 - 0x0C];
    Vec3f prev;          /* 0x048 */
    Vec3f vel;           /* 0x054 */
    Vec3f acc;           /* 0x060 */
    Vec3f fpos;          /* 0x06C */
    u8 pad78[0x44C - 0x78];
    f32 time;            /* 0x44C */
    u8 pad450[0x5B0 - 0x450];
    s32 in;              /* 0x5B0 */
    s32 inPrev;          /* 0x5B4 */
    u8 pad5B8[0x5D0 - 0x5B8];
    u8 b5D0;             /* 0x5D0 */
    u8 pad5D1;
    u8 b5D2;             /* 0x5D2 */
    u8 pad5D3[0x6E8 - 0x5D3];
    u8 b6E8;             /* 0x6E8 */
    u8 pad6E9[0x6F4 - 0x6E9];
    f32 f6F4;            /* 0x6F4 */
    u8 pad6F8[0x744 - 0x6F8];
    s32 s744;            /* 0x744 */
    u8 pad748[0x7A4 - 0x748];
    u8 b7A4;             /* 0x7A4 */
    u8 b7A5;             /* 0x7A5 */
    u8 pad7A6[0x7E0 - 0x7A6];
} Bike;

extern u32 D_800F0600;
extern s32 D_800E3770[];
extern Bike D_800E37C0[];
extern s32 D_800C2364;
extern s32 D_800C8CD8;
extern s32 D_800E3700;
extern s32 D_800E76D0;
extern s32 D_800EE140;
extern u32 D_800EFD70;
extern s32 D_800EFDBC;
extern u8 D_800E76CC;

void func_8001FE1C(Bike *b);
void func_80021EC8(Bike *b, s32 n);
void func_80036230(void);
void func_8006C340(void);
void func_80047A90(void);
void func_8006B4EC(void);
void func_800644E0(void);
void func_80063768(void);
void func_80063D30(void);

void func_80022448(s32 n) {
    Bike *b;
    s32 i;
    s32 v;
    s32 u;
    s32 pad[2];
    f32 t, t2, tk, t2k;
    f32 x, y, z;

    if (D_800F0600 & 0x800) {
        return;
    }
    for (i = 0; i < 8; i++) {
        b = &D_800E37C0[i];
        if (b->b6E8 == 0) {
            b->inPrev = b->in;
            v = D_800E3770[i];
            b->in = v;
            if (D_800C2364 != 0 && (D_800C8CD8 == 0 || i != D_800E3700)) {
                b->in = v ^ 0xC;
            }
        } else {
            u = b->in;
            b->b5D2 = 0;
            b->b5D0 = 0;
            b->in = 0;
            b->inPrev = u;
        }
    }
    D_800E76D0 += n;
    if (D_800EE140 != 0) {
        for (b = D_800E37C0; b < D_800E37C0 + 8; b++) {
            if (b->b7A5 == 0) {
                func_8001FE1C(b);
            }
        }
    } else {
        for (b = D_800E37C0; b < D_800E37C0 + 8; b++) {
            if (b->b7A5 != 0) {
                continue;
            }
            if (b->b7A4 != 0) {
                func_8001FE1C(b);
                continue;
            }
            if (b->b6E8 != 0 && (b->s744 < -30 || (b->s744 >= 11 && b->f6F4 > 0.5f))
                && !(b == D_800E37C0 && (D_800EFD70 & 0x81))) {
                if (((D_800E76D0 ^ (b - D_800E37C0)) & 1) != 0) {
                    func_80021EC8(b, 2);
                    b->fpos = b->prev;
                } else {
                    t = b->time / 60.0f;
                    t2 = t * t * 0.5f;
                    x = b->vel.x * t;
                    y = b->vel.y * t;
                    z = b->vel.z * t;
                    b->fpos.x += x + b->acc.x * t2;
                    b->fpos.y += y + b->acc.y * t2;
                    b->fpos.z += z + b->acc.z * t2;
                    t *= 1000.0f;
                    t2 *= 1000.0f;
                    b->pos[0] = (f32)b->pos[0] + (b->vel.x * t + b->acc.x * t2);
                    b->pos[1] = (f32)b->pos[1] + (b->vel.y * t + b->acc.y * t2);
                    b->pos[2] = (f32)b->pos[2] + (b->vel.z * t + b->acc.z * t2);
                }
            } else {
                for (i = 0; i < n; i++) {
                    func_80021EC8(b, 1);
                }
                b->fpos = b->prev;
            }
        }
        switch (D_800EFDBC) {
        case 2:
            func_8006C340();
            break;
        case 5:
            func_80047A90();
            break;
        case 6:
            func_8006B4EC();
            break;
        case 7:
        case 9:
        case 10:
            break;
        case 8:
            func_800644E0();
            func_80063768();
            break;
        default:
            func_80063D30();
            break;
        }
    }
    func_80036230();
    D_800E76CC = 0;
}
