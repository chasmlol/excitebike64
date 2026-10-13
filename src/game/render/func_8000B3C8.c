#include "ctx.h"


extern s32 D_800DC0B0;
extern s32 D_800E76D0;
extern s32 D_800E19BC;
extern s32 D_800C2594;
typedef struct { s32 x, y, w, h; s32 pad[12]; } View;
extern View D_800E1718[];
extern u32 D_800C2280;
extern u8 *D_800C27B4;
extern s32 *D_800C27AC;
extern s32 *D_800C27B0;
extern u8 **D_800E1560;

s32 func_800C2224(void);
u8 *func_800C1F04(u8 *p);
char *func_800C1F2C(s32 *p, char *s);
void func_800C0ACC(s32 *p, s32 x, s32 y);
Gfx *func_800C1394(Gfx *gdl);

Gfx *func_8000B3C8(Gfx *gdl)
{
    View *e;
    s32 i, y, done, alt, col;
    s32 c;
    s32 *font;
    s32 n;
    char buf[64];

    D_800E19BC = 0xF0 - (D_800E76D0 / 2) % (D_800DC0B0 << 4);
    func_800C2224();
    gDPSetScissor(gdl++, G_SC_NON_INTERLACE, D_800E1718[D_800C2594 - 1].x, D_800E1718[D_800C2594 - 1].y, D_800E1718[D_800C2594 - 1].x + D_800E1718[D_800C2594 - 1].w, D_800E1718[D_800C2594 - 1].y + D_800E1718[D_800C2594 - 1].h);
    if (D_800C2280 & 0x800) {
        gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
        gDPSetTexturePersp(gdl++, G_TP_NONE);
        gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFCF87C);
        gDPSetRenderMode(gdl++, 0x0C084000, 0);
        gDPSetTextureLUT(gdl++, G_TT_RGBA16);
        gDPLoadTLUT_pal256(gdl++, D_800E1560[1]);
        for (i = 0; i < 0xF0; i += 8) {
            gDPLoadTextureBlock(gdl++, D_800E1560[3] + i * 256, G_IM_FMT_CI, G_IM_SIZ_8b, 256, 8, 0,
                                G_TX_CLAMP, G_TX_CLAMP, 8, 3, G_TX_NOLOD, G_TX_NOLOD);
            gSPTextureRectangle(gdl++, 0, i << 2, 0x800, (i + 8) << 2, G_TX_RENDERTILE, 0, 0, 0x200, 0x400);
        }
    }
    y = D_800E19BC;
    i = 0;
    done = 0;
    alt = 0;
    if (y < 0x104) {
    n = 0;
    do {
        switch (D_800C27B4[i]) {
        case 0xD:
        case '@':
            buf[n] = 0;
            if (y >= -0x13 && n != 0) {
                if (alt) {
                    font = D_800C27B0;
                } else {
                    font = D_800C27AC;
                }
                func_800C1F04((u8 *)font);
                func_800C1F2C(font, buf);
                func_800C0ACC(font, col * 128 - font[6] / 2, y);
                alt = 0;
            }
            n = 0;
            if (D_800C27B4[i] == '@') {
                i++;
                c = D_800C27B4[i];
                switch (c) {
                case 'E':
                    done = 1;
                    i = -1;
                    break;
                case 'H':
                    alt = 1;
                    break;
                case '1':
                case '2':
                case '3':
                    col = c - '0';
                    break;
                }
            } else {
                y += 16;
            }
            i++;
            break;
        case 10:
            i++;
            break;
        default:
            buf[n++] = D_800C27B4[i++];
            break;
        }
    } while (!done && y < 0x104);
    }
    return func_800C1394(gdl);
}
