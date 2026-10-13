#include "ctx.h"

typedef struct {
    s32 val;
    char *str[3];
} Entry;

typedef struct {
    u8 pad0[0xC];
    u8 flags;
    u8 num;
    u8 padE[2];
    char *name;
    u8 pad14[0xC];
    u32 count;
    Entry *entries;
} Track;

typedef struct {
    u8 *f0;
    u8 pad4[0x60];
    s16 f64;
} Ctx930;

typedef struct {
    u32 state;
    u8 pad4[0x22];
    s16 f26;
    u8 pad28[4];
    s32 f2C;
    u8 pad30[0x28];
    s32 f58;
    u8 pad5C[0x14];
    s32 f70;
    u8 pad74[4];
    s32 f78;
} Game;

typedef struct {
    u8 pad[0x420];
    f32 f420;
    u8 pad2[0x3BC];
} Bike;

extern Ctx930 *D_800CA930;
extern Track D_800CADE4[];
extern Game D_800F05A8;
extern s32 *D_800F0530[];
extern u8 *D_800EFD68;
extern u8 *D_800EFD6C;
extern u8 *D_800F0740;
extern u32 D_800F06B0;
extern s32 D_800F13EC;
extern s32 D_800C8CE4;
extern s32 D_800E2038;
extern s8 D_800F06AC;
extern s32 D_800F0738;
extern char *D_800CB098[];
extern u8 D_800C9A3C[];
extern Bike D_800E37C0[];
extern volatile s32 D_800CCCA8;
extern s32 D_800D88D0[];
extern s32 D_800F05D4;
extern s32 D_800F0600;

s32 func_800C2224(void);
void func_8000EE30(s32 v, char *out);
s32 *func_800C1E60(s32 *param_1, s32 param_2, s32 param_3, s32 param_4, s32 param_5);
char *func_800C1F2C(s32 *p, char *s);
Gfx *func_80058294(Gfx *gdl, s32 arg1, u8 *str, s32 x, s32 y);
Gfx *func_800C1394(Gfx *gdl);
void func_8005313C(s32 param_1, char *param_2);
u8 *func_800C0ACC(s32 *p, s32 x, s32 y);
Gfx *func_8000A690(s32 x, s32 y, u8 *img, Gfx *gdl, s32 a4, s32 a5, s32 a6, s32 a7, s32 a8);
Gfx *func_80009754(Gfx *gdl, s32 a1, s32 a2, s32 a3);
Gfx *func_80054ED0(Gfx *gdl, s32 a1);
s32 func_800AEC10(char *s);

#define gDPRaw(pkt, a, b) \
{ \
    Gfx *_g = (Gfx *)(pkt); \
    _g->words.w0 = (a); \
    _g->words.w1 = (b); \
}

#define CUR_FLAGS (D_800CADE4[D_800CA930->f64].flags)

Gfx *func_80058438(Gfx *gdl, s32 arg1, s32 arg2, s32 arg3)
{
    Track *trk;
    Entry *e;
    Gfx *p;
    u8 *img;
    u8 *img2;
    s32 off;
    u32 i;
    s32 j;
    s32 k;
    s32 m;
    s32 d;
    s32 v;
    s32 n;
    s32 w;
    s32 x0;
    s32 x;
    s32 y;
    u8 flags;
    f32 f;
    char buf[32];
    s32 cx;
    u8 *g;

    trk = &D_800CADE4[D_800CA930->f64];
    func_800C2224();
    gDPSetTexturePersp(gdl++, G_TP_NONE);
    gDPSetPrimColor(gdl++, 0, 0, 0xD2, 0xD2, 0xD2, 0xFF);
    gDPRaw(gdl++, 0xE200001C, 0x404240);
    if (D_800F05A8.state == 0xD) {
        if (D_800F05A8.f70 == 0) {
            off = -(D_800F05A8.f78 * 2);
        } else {
            off = 0;
            if (D_800F05A8.f70 == 2) {
                off = -((60 - D_800F05A8.f78) * 2);
            }
        }
        gDPRaw(gdl++, 0xFCFFFFFF, 0xFFFCF279);
        gDPSetTextureLUT(gdl++, G_TT_RGBA16);
        gDPLoadTLUT_pal256(gdl++, *(u32 *)(D_800EFD68 + 4));
        cx = D_800F13EC / 2;
        w = D_800EFD68[0x10] * 4 - 10;
        gdl = func_8000A690(cx - w, off + 0x1E, D_800EFD68 + 0x10, gdl, arg2, 1, 1, 3, 0);
        gDPSetTextureLUT(gdl++, G_TT_NONE);
        gDPRaw(gdl++, 0xFC119623, 0xFF2FFFFF);
        gdl = func_8000A690(D_800F13EC / 2 - D_800EFD6C[0x18] * 4, off + 0x1A, D_800EFD6C + 0x18, gdl, arg2, 1, 1, 6, 0);
        func_800C1E60(D_800F0530[i], 0x40, 1, 2, 0x20);
        func_800C1F2C(D_800F0530[i], trk->name);
        func_800C0ACC(D_800F0530[i], 0x4D - i * 3, i * 13 + 0x44 + off);
        func_800C1E60(D_800F0530[3], 0x40, 1, 2, 0x21);
        __builtin_memcpy(buf, D_800D88D0, 9);
        buf[func_800AEC10(buf) - 1] += trk->num;
        func_800C1F2C(D_800F0530[3], buf);
        func_800C0ACC(D_800F0530[3], D_800F13EC / 2 + 3, off + 0x29);
    } else {
        if (D_800C8CE4 != 0) {
            if (D_800E2038 & 0x20) {
                func_800C1E60(D_800F0530[5], 0x40, 1, 2, 0x20);
                func_800C1F2C(D_800F0530[5], "Press START");
                func_800C0ACC(D_800F0530[5], 0x2F, 0x2D);
            }
        } else {
            flags = CUR_FLAGS;
            if (flags & 4) {
                func_800C1E60(D_800F0530[0], 0x40, 2, 6, 0x20);
                if (D_800F05A8.f58 & 0x1000) {
                    func_800C1F2C(D_800F0530[0], "Takedown Required");
                } else {
                    func_800C1F2C(D_800F0530[0], "Takedown Complete");
                }
                func_800C0ACC(D_800F0530[0], 0x21, 0x2D);
            } else if (flags & 8) {
              if (!(flags & 2)) {
                func_800C1E60(D_800F0530[0], 0x40, 1, 6, 0x20);
                v = D_800F05A8.f2C;
                if (v < 0) {
                    v = -v;
                }
                func_8005313C(v, buf);
                func_800C1F2C(D_800F0530[0], "Time: ");
                func_800C1F2C(D_800F0530[0], buf);
                if (D_800F05A8.f2C < 0) {
                    D_800F0530[0][1] = 8;
                }
                func_800C0ACC(D_800F0530[0], 0x21, 0x2D);
              }
            }
        }
        func_800C1E60(D_800F0530[1], 0x40, 1, 4, 0x20);
        func_800C1F2C(D_800F0530[1], trk->name);
        func_800C0ACC(D_800F0530[1], 0x3D, 0x20);
        gDPSetTextureLUT(gdl++, G_TT_NONE);
        gDPRaw(gdl++, 0xFC119623, 0xFF2FFFFF);
        if ((CUR_FLAGS & 2) && D_800F0738 >= 2 && D_800F0738 < 10) {
            func_800C1E60(D_800F0530[2], 0x40, 2, 2, 0x20);
            func_800C1F2C(D_800F0530[2], trk->entries[D_800F0738].str[0]);
            func_800C0ACC(D_800F0530[2], 0x140, 0x28);
            p = gdl;
            gDPSetTextureLUT(p++, G_TT_NONE);
            gDPRaw(p++, 0xFC119623, 0xFF2FFFFF);
            k = 27;
            x0 = 0x2EE;
            g = D_800EFD6C + 0x58;
            p = func_8000A690(x0 - g[0] * k * 8 - D_800EFD6C[0x60] * 8, 0x1C, D_800EFD6C + 0x60, p, arg2, 1, 1, 6, 0);
            for (m = 0; m < 27; m++) {
                p = func_8000A690(x0 - (k - m) * g[0] * 8, 0x1C, g, p, arg2, 1, 1, 6, 0);
            }
            gdl = func_80058294(p, arg2, trk->entries[D_800F0738].str[1], 0x140, 0x40);
            gDPSetTextureLUT(gdl++, G_TT_NONE);
            gDPRaw(gdl++, 0xFC119623, 0xFF2FFFFF);
        } else {
            if (D_800F06AC == 1) {
                if (D_800F0738 == 0) {
                    i = trk->count - 1;
                } else {
                    i = D_800F0738;
                }
                goto show;
            }
            if (D_800F06AC == -1) {
                for (j = 0; j < 3; j++) {
                    func_800C1E60(D_800F0530[j + 2], 0x40, 2, 2, 0x20);
                    func_800C1F2C(D_800F0530[j + 2], D_800CB098[j]);
                    func_800C0ACC(D_800F0530[j + 2], 0x20, 0xB2 + j * 9);
                }
            } else {
                if (D_800C8CE4 == 0 && !(D_800F05A8.f58 & 0x8000)) {
                    goto skip;
                }
                if (!(D_800F05A8.f58 & 0x800)) {
                    goto skip;
                }
                for (i = 0; i < trk->count; i++) {
                    d = trk->entries[i].val - D_800F05D4;
                    if (d <= 0) {
                        d = D_800F05D4 - trk->entries[i].val;
                    }
                    if (d >= 3 && !(D_800F0600 & 0x8000)) {
                        continue;
                    }
                    {
                    show:
                        n = 3;
                        if (trk->entries[i].str[2] == NULL) {
                            n = 2;
                            if (trk->entries[i].str[1] == NULL) {
                                n = 1;
                            }
                        }
                        for (j = 0; j < n; j++) {
                            func_800C1E60(D_800F0530[j + 2], 0x40, 2, 2, 0x20);
                            func_800C1F2C(D_800F0530[j + 2], trk->entries[i].str[j]);
                            func_800C0ACC(D_800F0530[j + 2], 0x20, 0xBB - (n - 1) * 9 / 2 + j * 9);
                        }
                        break;
                    }
                }
            }
            img = D_800EFD6C + 0x30;
            for (i = 0; i < 19; i++) {
                gdl = func_8000A690(i * img[0] * 8 + 0x10, 0xA9, img, gdl, arg2, 1, 1, 6, 0);
            }
            p = func_8000A690(img[0] * 0x98 + 0x10, 0xA9, D_800EFD6C + 0x38, gdl, arg2, 1, 1, 6, 0);
            gdl = func_80058294(p, arg2, D_800C9A3C, 0x136, 0xC3);
            gDPSetTextureLUT(gdl++, G_TT_NONE);
            gDPRaw(gdl++, 0xFC119623, 0xFF2FFFFF);
        }
    skip:
        if (D_800F05A8.state == 0xE || ((CUR_FLAGS & 8) && !(CUR_FLAGS & 2))) {
            img = D_800EFD6C + 0x30;
        } else {
            img = D_800EFD6C + 0x40;
        }
        for (i = 0; i < 9; i++) {
            gdl = func_8000A690(i * img[0] * 8 + 0x10, 0x14, img, gdl, arg2, 1, 1, 6, 0);
        }
        if (D_800F05A8.state == 0xE || ((CUR_FLAGS & 8) && !(CUR_FLAGS & 2))) {
            img2 = D_800EFD6C + 0x38;
        } else {
            img2 = D_800EFD6C + 0x48;
        }
        gdl = func_8000A690(img[0] * 0x48 + 0x10, 0x14, img2, gdl, arg2, 1, 1, 6, 0);
        gDPRaw(gdl++, 0xE200001C, 0x404240);
        gDPSetTextureLUT(gdl++, G_TT_RGBA16);
        gDPRaw(gdl++, 0xFCFFFFFF, 0xFFFCF279);
        gDPSetTexturePersp(gdl++, G_TP_NONE);
        gDPSetTextureLUT(gdl++, G_TT_RGBA16);
        if (D_800CA930->f64 > 0) {
            gDPLoadTLUT_pal256(gdl++, *(u32 *)(D_800F0740 + 4));
        } else {
            gDPLoadTLUT_pal256(gdl++, D_800F06B0);
        }
        gdl = func_8000A690(0x21, 0x1B, D_800F0740 + 0x20, gdl, arg2, 1, 1, 3, 0);
        if (D_800CA930->f64 < 16 && D_800CADE4[D_800CA930->f64 + 1].num <= D_800CA930->f0[1]) {
            gDPLoadTLUT_pal256(gdl++, *(u32 *)(D_800F0740 + 4));
        } else {
            gDPLoadTLUT_pal256(gdl++, D_800F06B0);
        }
        gdl = func_8000A690(0x96, 0x1B, D_800F0740 + 0x10, gdl, arg2, 1, 1, 3, 0);
        gDPSetTextureLUT(gdl++, G_TT_NONE);
        gDPRaw(gdl++, 0xFC119623, 0xFF2FFFFF);
    }
    gdl = func_80009754(gdl, arg1, arg2, 0);
    func_800C1E60(D_800F0530[6], 0x40, 3, 2, 0x12);
    f = D_800E37C0[arg3].f420;
    func_8000EE30((s32)((f > 0.0f ? f * 3600.0f : -f * 3600.0f) / 1612.9f), buf);
    func_800C1F2C(D_800F0530[6], buf);
    x = 0x1BA;
    if (D_800CCCA8 == 2) {
        x = 0x228;
    }
    y = 0xD6;
    if (D_800CCCA8 == 2) {
        y = 0x180;
    }
    func_800C0ACC(D_800F0530[6], x, y);
    if ((CUR_FLAGS & 8) && D_800F05A8.f26 != 0
        && (D_800F05A8.state < 5 || D_800F05A8.state == 15 || D_800F05A8.state == 14)
        && (D_800F05A8.state != 1 || D_800F05A8.f26 < 0x5B)) {
        gdl = func_80054ED0(gdl, arg3);
    }
    return func_800C1394(gdl);
}
