#include "ctx.h"

typedef struct {
    u8 pad0[0x48];
    f32 f48;
    u8 pad4C[4];
    f32 f50;
    u8 pad54[0x3FF - 0x54];
    u8 b3FF;
    u8 pad400[0x20];
    f32 f420;
    u8 pad424[0x66A - 0x424];
    s16 h66A;
    u8 pad66C[0x68C - 0x66C];
    s32 w68C;
    u8 pad690[0x69C - 0x690];
    s32 w69C;
    u8 pad6A0[0x7E0 - 0x6A0];
} Player;

typedef struct {
    s32 w0;
    u8 pad4[0x22];
    s16 h26;
} Mode;

extern s32 *D_800F0530[];
extern volatile s32 D_800CCCA8;
extern s32 D_800C2594;
extern Player D_800E37C0[];
extern s32 D_800F05D4;
extern s32 D_800E1718[][4][4];
extern s32 D_800E1898;
extern Mode D_800F05A8;
extern s32 D_800E3710;
extern s32 D_800F05E8;
extern s32 D_800EFD70;
extern s32 D_800E2060;
extern char D_800D84A8[];

s32 func_800C2224(void);
void func_800C1D54(s32 a, s32 b, s32 c, s32 d);
void func_8000EE30(s32 v, char *out);
s32 *func_800C1E60(s32 *param_1, s32 param_2, s32 param_3, s32 param_4, s32 param_5);
char *func_800C1F2C(s32 *p, char *s);
void func_8005CD30(char *p, f32 x, s32 n);
Gfx *func_800C1394(Gfx *gdl);
void func_80052F30(s32 param_1, char *param_2);
void func_800C0ACC(s32 *p, s32 x, s32 y);
Gfx *func_80009754(Gfx *gdl, s32 a1, s32 a2, s32 pl);
Gfx *func_8005A4C8(Gfx *gdl, s32 a1, s32 a2, s32 pl);
Gfx *func_80054ED0(Gfx *gdl, s32 pl);

Gfx *func_800535B8(Gfx *gdl, s32 a1, s32 a2, s32 pl)
{
    char buf[32];
    f32 v;
    s32 x1, y1;
    s32 t;
    s32 *p;

    func_800C2224();
    func_800C1E60(D_800F0530[0], 0x40, 1, 2, 0x20);
    func_800C1E60(D_800F0530[1], 0x40, 1, 2, 0x20);
    func_800C1E60(D_800F0530[2], 0x40, 1, 4, 0x20);
    func_800C1E60(D_800F0530[3], 0x40, 1, 4, 0x20);
    func_800C1E60(D_800F0530[7], 0x40, 1, 6, 0x20);
    func_800C1E60(D_800F0530[8], 0x40, 1, 6, 0x20);
    if (D_800CCCA8 == 2) {
        func_800C1D54(0x10000, 0x20000, 0, 0);
    }
    if (D_800C2594 == 1) {
        func_800C1E60(D_800F0530[6], 0x40, 3, 2, 0x12);
        v = D_800E37C0[pl].f420;
        func_8000EE30((s32)((v > 0.0f ? v * 3600.0f : -v * 3600.0f) / 1612.9f), buf);
        func_800C1F2C(D_800F0530[6], buf);
        x1 = 0x1BA;
        if (D_800CCCA8 == 2) {
            x1 = 0x228;
        }
        y1 = 0xD6;
        if (D_800CCCA8 == 2) {
            y1 = 0x180;
        }
        func_800C0ACC(D_800F0530[6], x1, y1);
        gdl = func_80009754(gdl, a1, a2, pl);
        func_800C1E60(D_800F0530[4], 0x40, 1, 2, 0x20);
        func_800C1E60(D_800F0530[5], 0x40, 1, 4, 0x20);
        func_80052F30(D_800E37C0[pl].w69C - D_800F05D4, buf);
        func_800C1F2C(D_800F0530[4], "Time:");
        func_800C0ACC(D_800F0530[4], 0x23, D_800CCCA8 == 2 ? 0x180 : 0xD6);
        func_800C1F2C(D_800F0530[5], buf);
        func_800C0ACC(D_800F0530[5], 100, D_800CCCA8 == 2 ? 0x180 : 0xD6);
    } else {
        func_800C1E60(D_800F0530[4], 0x40, 2, 2, 0x20);
        func_800C1E60(D_800F0530[5], 0x40, 2, 4, 0x20);
        t = -D_800F05D4 + D_800E37C0[pl].w69C;
        if (D_800E37C0[pl].b3FF) {
            t = D_800E37C0[pl].w68C;
        }
        if (t < 0) {
            t = 0;
        }
        func_80052F30(t, buf);
        func_800C1F2C(D_800F0530[4], "Time:");
        func_800C0ACC(D_800F0530[4], D_800E1718[D_800E1898 - 1][pl][0] + 10, 0x1E);
        func_800C1F2C(D_800F0530[5], buf);
        func_800C0ACC(D_800F0530[5], D_800E1718[D_800E1898 - 1][pl][0] + 0x32, 0x1E);
        func_800C1E60(D_800F0530[6], 0x40, 3, 2, 0x12);
        v = D_800E37C0[pl].f420;
        func_8000EE30((s32)((v > 0.0f ? v * 3600.0f : -v * 3600.0f) / 1612.9f), buf);
        func_800C1F2C(D_800F0530[6], buf);
        func_800C0ACC(D_800F0530[6],
                      D_800E1718[D_800E1898 - 1][pl][0] + D_800E1718[D_800E1898 - 1][pl][2] - 0x2D,
                      D_800E1718[D_800E1898 - 1][pl][1] + D_800E1718[D_800E1898 - 1][pl][3] - 0xB);
        gdl = func_8005A4C8(gdl, a1, a2, pl);
    }
    if (D_800F05A8.h26 != 0 && D_800F05A8.w0 != 0 && (D_800F05A8.w0 != 1 || D_800F05A8.h26 < 0x5B)) {
        gdl = func_80054ED0(gdl, pl);
    }
    if (D_800E37C0[pl].h66A != 0 && D_800E3710 == 0) {
        func_800C1E60(D_800F0530[7], 0x40, 1, 4, 0x21);
        func_800C1F2C(D_800F0530[7], "Stage Clear!");
        func_800C0ACC(D_800F0530[7],
                      D_800E1718[D_800E1898 - 1][pl][0] + D_800E1718[D_800E1898 - 1][pl][2] / 2,
                      D_800E1718[D_800E1898 - 1][pl][1] + D_800E1718[D_800E1898 - 1][pl][3] / 2);
    }
    if (D_800E37C0[pl].b3FF) {
        func_800C1E60(D_800F0530[7], 0x40, 1, 4, 0x21);
        if (D_800F05E8 == 0 || pl != 0) {
            func_800C1F2C(D_800F0530[7], "Game Over");
        } else {
            func_800C1F2C(D_800F0530[7], D_800D84A8);
        }
        func_800C0ACC(D_800F0530[7],
                      D_800E1718[D_800E1898 - 1][pl][0] + D_800E1718[D_800E1898 - 1][pl][2] / 2,
                      D_800E1718[D_800E1898 - 1][pl][1] + D_800E1718[D_800E1898 - 1][pl][3] / 2);
    }
    if (D_800EFD70 & 0x20000000) {
        s32 *p1;
        s32 *p2;
        s32 *p3;
        p1 = func_800C1E60(D_800F0530[9], 0x20, 2, 2, 0x10);
        func_8005CD30(buf, D_800E37C0[0].f48, 5);
        func_800C1F2C(p1, buf);
        func_800C0ACC(p1, 0x28, 0x1E);
        p2 = func_800C1E60(D_800F0530[10], 0x20, 2, 2, 0x10);
        func_8005CD30(buf, D_800E37C0[0].f50, 5);
        func_800C1F2C(p2, buf);
        func_800C0ACC(p2, 0x28, 0x28);
        p3 = func_800C1E60(D_800F0530[10], 0x20, 2, 2, 0x10);
        func_8000EE30(D_800E2060, buf);
        func_800C1F2C(p3, buf);
        func_800C0ACC(p3, 0x28, 0x32);
    }
    return func_800C1394(gdl);
}
