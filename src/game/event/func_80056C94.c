#include "ctx.h"

typedef struct {
    s32 x, y, w, h;
} Rect;

typedef struct {
    u8 pad0[0x48];
    f32 f48;
    u8 pad4C[4];
    f32 f50;
    u8 pad54[0x7E0 - 0x54];
} Player;

typedef struct {
    u32 w0;
    u8 pad4[0x2C - 0x4];
    s32 w2C;
    u8 pad30[0x58 - 0x30];
    s32 w58;
    u8 pad5C[0x68 - 0x5C];
    s16 h68;
    s16 h6A;
} Mode;

typedef struct {
    u8 pad0[0x3C];
    s32 w3C;
} Game;

extern Rect D_800E1718[][4];
extern s32 D_800E1898;
extern Player D_800E37C0[];
extern Vec3f D_800F1510;
extern Mode D_800F05A8;
extern Game D_800EFD70;
extern s32 D_800EFDAC;
extern u8 *D_800F0528;
extern s32 *D_800F0530[];
extern s32 *D_800F054C;
extern s32 D_800F13EC;
extern s32 D_800F13F0;
extern char D_800D865C[];
extern char D_800D871C[];

void func_8000ED30(char *dst, char *src);
s32 func_800C2224(void);
void func_8000EE30(s32 v, char *out);
s32 *func_800C1E60(s32 *param_1, s32 param_2, s32 param_3, s32 param_4, s32 param_5);
char *func_800C1F2C(s32 *p, char *s);
Gfx *func_800C1394(Gfx *gdl);
void func_80052F30(s32 param_1, char *param_2);
u8 *func_800C0ACC(s32 *p, s32 x, s32 y);
s32 func_800AEC10(char *s);
Gfx *func_8000A690(s32 x, s32 y, u8 *img, Gfx *gdl, s32 a4, s32 a5, s32 a6, s32 a7, s32 a8);

#define gDPRaw(pkt, a, b) \
{ \
    Gfx *_g = (Gfx *)(pkt); \
    _g->words.w0 = (a); \
    _g->words.w1 = (b); \
}

Gfx *func_80056C94(Gfx *gdl, s32 p1, s32 p2, s32 p3) {
    char buf[32];
    char msg[64];
    s32 cx;
    s32 cy;
    s32 n;
    s32 team;

    func_800C2224();
    if (p3 == 0) {
        gDPSetScissor(gdl++, G_SC_NON_INTERLACE, D_800E1718[0][0].x, D_800E1718[0][0].y,
                      D_800E1718[0][0].x + D_800E1718[0][0].w - 1, D_800E1718[0][0].y + D_800E1718[0][0].h - 1);
        if (D_800F05A8.w0 == 7 || D_800F05A8.w0 == 9 || D_800F05A8.w0 == 11) {
            gDPSetPrimColor(gdl++, 0, 0, 0xE6, 0xE6, 0xE6, 0xE6);
            gDPRaw(gdl++, 0xE3000C00, 0);
            gDPRaw(gdl++, 0xD7000002, 0x80008000);
            gDPRaw(gdl++, 0xE3001001, 0);
            gDPRaw(gdl++, 0xFC119623, 0xFF2FFFFF);
            gDPRaw(gdl++, 0xE200001C, 0x404240);
            cx = 0x100;
            cy = 0x78;
            if (D_800EFDAC == 4) {
                cx = 0x13C;
            }
            gdl = func_8000A690((cx - D_800F0528[8] * 4) | 1, cy - (D_800F0528[9] >> 1) - 1, D_800F0528 + 8, gdl,
                                p2, 1, 1, 6, 0);
            gDPRaw(gdl++, 0xFCFFFFFF, 0xFFFDF6FB);
            gDPRaw(gdl++, 0xE200001C, 0x404240);
            gDPRaw(gdl++, 0xD7000000, 0x80008000);
            gDPSetPrimColor(gdl++, 0, 0, 0x00, 0x28, 0xFF, 0xFF);
            gDPFillRectangle(gdl++, cx + D_800E37C0[0].f50 * 0.25f / 0.625f - 2.0f, cy + D_800E37C0[0].f48 * 0.25f - 2.0f,
                             cx + D_800E37C0[0].f50 * 0.25f / 0.625f + 2.0f, cy + D_800E37C0[0].f48 * 0.25f + 2.0f);
            gDPSetPrimColor(gdl++, 0, 0, 0xFA, 0xFA, 0x28, 0xFF);
            gDPFillRectangle(gdl++, cx + D_800E37C0[1].f50 * 0.25f / 0.625f - 2.0f, cy + D_800E37C0[1].f48 * 0.25f - 2.0f,
                             cx + D_800E37C0[1].f50 * 0.25f / 0.625f + 2.0f, cy + D_800E37C0[1].f48 * 0.25f + 2.0f);
            if (D_800EFDAC == 4) {
                gDPSetPrimColor(gdl++, 0, 0, 0x28, 0xFA, 0x28, 0xFF);
                gDPFillRectangle(gdl++, cx + D_800E37C0[2].f50 * 0.25f / 0.625f - 2.0f, cy + D_800E37C0[2].f48 * 0.25f - 2.0f,
                                 cx + D_800E37C0[2].f50 * 0.25f / 0.625f + 2.0f, cy + D_800E37C0[2].f48 * 0.25f + 2.0f);
                gDPSetPrimColor(gdl++, 0, 0, 0xFA, 0x28, 0x28, 0xFF);
                gDPFillRectangle(gdl++, cx + D_800E37C0[3].f50 * 0.25f / 0.625f - 2.0f, cy + D_800E37C0[3].f48 * 0.25f - 2.0f,
                                 cx + D_800E37C0[3].f50 * 0.25f / 0.625f + 2.0f, cy + D_800E37C0[3].f48 * 0.25f + 2.0f);
            }
            gDPSetPrimColor(gdl++, 0, 0, 0xFA, 0xFA, 0xFA, 0xFF);
            gDPFillRectangle(gdl++, cx + D_800F1510.z * 0.25f / 0.625f - 1.0f, cy + D_800F1510.x * 0.25f - 1.0f,
                             cx + D_800F1510.z * 0.25f / 0.625f + 1.0f, cy + D_800F1510.x * 0.25f + 1.0f);
        }
        switch (D_800F05A8.w0) {
        case 6:
            func_800C1E60(D_800F0530[0], 0x40, 1, 4, 0x21);
            if (D_800EFDAC == 2) {
                func_800C1F2C(D_800F0530[0], "One on One Soccer: First Half");
            } else {
                func_800C1F2C(D_800F0530[0], D_800D865C);
            }
            func_800C0ACC(D_800F0530[0], D_800F13EC / 2, D_800F13F0 / 2);
            break;
        case 8:
        case 10:
        case 12:
            func_800C1E60(D_800F0530[0], 0x40, 1, 6, 0x21);
            switch (D_800F05A8.w0) {
            case 10:
                func_800C1F2C(D_800F0530[0], "Full Time Draw : Overtime Required");
                break;
            case 8:
                func_800C1F2C(D_800F0530[0], "Half Time");
                break;
            case 12:
                func_800C1F2C(D_800F0530[0], "Final");
                break;
            }
            func_800C0ACC(D_800F0530[0], D_800F13EC / 2, D_800F13F0 / 2 - 13);
            func_800C1E60(D_800F0530[1], 0x40, 1, 2, 0x20);
            func_800C1E60(D_800F0530[2], 0x40, 1, 2, 0x20);
            func_800C1E60(D_800F0530[3], 0x40, 1, 4, 0x20);
            func_800C1E60(D_800F0530[4], 0x40, 1, 4, 0x20);
            func_800C1F2C(D_800F0530[1], "Team 1:");
            func_800C1F2C(D_800F0530[2], "Team 2:");
            func_8000EE30(D_800F05A8.h68, buf);
            func_800C1F2C(D_800F0530[3], buf);
            func_8000EE30(D_800F05A8.h6A, buf);
            func_800C1F2C(D_800F0530[4], buf);
            func_800C0ACC(D_800F0530[1], D_800F13EC / 2 - 45, D_800F13F0 / 2);
            func_800C0ACC(D_800F0530[2], D_800F13EC / 2 - 45, D_800F13F0 / 2 + 13);
            func_800C0ACC(D_800F0530[3], D_800F13EC / 2 + 39, D_800F13F0 / 2);
            func_800C0ACC(D_800F0530[4], D_800F13EC / 2 + 39, D_800F13F0 / 2 + 13);
            if (D_800F05A8.w0 == 12) {
                func_800C1E60(D_800F0530[0], 0x40, 1, 4, 0x21);
                func_800C1F2C(D_800F0530[0], "Press START To Continue");
                func_800C0ACC(D_800F0530[0], D_800F13EC / 2,
                              D_800E1718[D_800E1898 - 1][0].y + D_800E1718[D_800E1898 - 1][0].h / 2);
                func_800C0ACC(D_800F0530[0], D_800F13EC / 2,
                              D_800E1718[D_800E1898 - 1][D_800E1898 - 1].y +
                                  D_800E1718[D_800E1898 - 1][D_800E1898 - 1].h / 2);
            }
            break;
        default:
            func_800C1E60(D_800F0530[0], 0x40, 1, 2, 0x20);
            func_800C1E60(D_800F0530[1], 0x40, 1, 2, 0x20);
            func_800C1E60(D_800F0530[2], 0x40, 1, 4, 0x20);
            func_800C1E60(D_800F0530[3], 0x40, 1, 4, 0x20);
            func_800C1F2C(D_800F0530[0], "Team 1:");
            func_800C1F2C(D_800F0530[1], "Team 2:");
            func_8000EE30(D_800F05A8.h68, buf);
            func_800C1F2C(D_800F0530[2], buf);
            func_8000EE30(D_800F05A8.h6A, buf);
            func_800C1F2C(D_800F0530[3], buf);
            if (D_800EFDAC == 4) {
                func_800C0ACC(D_800F0530[0], D_800F13EC / 2 - 45, 30);
                func_800C0ACC(D_800F0530[1], D_800F13EC / 2 - 45, D_800F13F0 - 30);
                func_800C0ACC(D_800F0530[2], D_800F13EC / 2 + 39, 30);
                func_800C0ACC(D_800F0530[3], D_800F13EC / 2 + 39, D_800F13F0 - 30);
            } else {
                func_800C0ACC(D_800F0530[0], 65, D_800F13F0 / 2 - 7);
                func_800C0ACC(D_800F0530[1], 65, D_800F13F0 / 2 + 7);
                func_800C0ACC(D_800F0530[2], 150, D_800F13F0 / 2 - 7);
                func_800C0ACC(D_800F0530[3], 150, D_800F13F0 / 2 + 7);
            }
            func_800C1E60(D_800F0530[4], 0x40, 1, 2, 0x22);
            func_800C1E60(D_800F0530[5], 0x40, 1, 4, 0x22);
            switch (D_800F05A8.w0) {
            case 11:
                func_800C1F2C(D_800F0530[4], "Overtime");
                break;
            case 7:
                func_800C1F2C(D_800F0530[4], "First Half");
                break;
            case 9:
                func_800C1F2C(D_800F0530[4], "Second Half");
                break;
            }
            func_80052F30(D_800F05A8.w2C, buf);
            func_800C1F2C(D_800F0530[5], buf);
            if (D_800EFDAC == 4) {
                func_800C0ACC(D_800F0530[4], D_800F13EC / 2 - 10, D_800F13F0 / 2 - 7);
                func_800C0ACC(D_800F0530[5], D_800F13EC / 2 - 10, D_800F13F0 / 2 + 7);
            } else {
                func_800C0ACC(D_800F0530[4], D_800F13EC - 65, D_800F13F0 / 2 - 7);
                func_800C0ACC(D_800F0530[5], D_800F13EC - 65, D_800F13F0 / 2 + 7);
            }
            break;
        }
    }
    n = D_800F05A8.w58 & 0xF;
    if (n != 0) {
        team = n - 1;
        func_800C1E60(D_800F0530[7], 0x40, D_800EFD70.w3C == 4 ? 2 : 1, 4, 0x21);
        if (team == (p3 >> ((D_800EFD70.w3C - 1) >> 1))) {
            if (D_800F05A8.w58 & 0x20) {
                func_800C1F2C(D_800F0530[7], "Team ");
                func_8000EE30((team ^ 1) + 1, buf);
                func_800C1F2C(D_800F0530[7], buf);
                func_800C1F2C(D_800F0530[7], " scored for you!");
            } else {
                func_800C1F2C(D_800F0530[7], D_800D871C);
            }
        } else {
            if (D_800F05A8.w58 & 0x20) {
                strcpy(msg, "You helped team ");
                func_8000EE30(n, msg + func_800AEC10(msg));
                func_8000ED30(msg, " score!");
            } else {
                strcpy(msg, "Team ");
                func_8000EE30(n, msg + func_800AEC10(msg));
                func_8000ED30(msg, " scored!");
            }
            func_800C1F2C(D_800F054C, msg);
        }
        func_800C0ACC(D_800F054C, D_800E1718[D_800E1898 - 1][p3].x + D_800E1718[D_800E1898 - 1][p3].w / 2,
                      D_800E1718[D_800E1898 - 1][p3].y + D_800E1718[D_800E1898 - 1][p3].h / 2);
    }
    return func_800C1394(gdl);
}
