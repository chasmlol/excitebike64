#include "ctx.h"

extern Gfx *D_800F5A44;
extern u8 *D_800F5A54;
extern u16 *D_800F5A58;
extern u16 *D_800F5A68[];
extern s32 D_800E30A8;

#define OAM ((u8 *)0x80394000)

void func_800A0D60(void)
{
    s32 i, j, x, k, s, idx;
    u16 **pp;
    u8 *base;
    u16 *pal;
    u8 *spr;
    s32 t;
    s32 y;
    u16 c;
    u8 *NES = (u8 *)0x80380000;
    u8 *mem;

    if (NES[0x2001] & 0x10) {
        base = D_800F5A54;
        gDPPipeSync(D_800F5A44++);
        gDPSetRenderMode(D_800F5A44++, 0x0C087008, 0);
        gDPSetCombine(D_800F5A44++, 0xFFFFFF, 0xFFFCF279);
        gDPSetAlphaCompare(D_800F5A44++, G_AC_THRESHOLD);
        if (NES[0x2000] & 8) {
            base += 0x4000;
        }
        k = 0x3F10;
        idx = 0;
        i = 0;
        mem = (u8 *)0x80380000;
        t = 0x10000;
        pal = D_800F5A58;
        pp = D_800F5A68;
    outer:
        for (j = 0; j < 4; j++) {
            x = pal[*(u8 *)(k + (s32)mem + t)] | 2;
            pp[D_800E30A8][idx] = x;
            k++;
            if (j == 0) {
                pp[D_800E30A8][idx] &= 0xFFFE;
            }
            idx++;
        }
        idx += 12;
        if (++i < 4) goto outer;
        gDPPipeSync(D_800F5A44++);
        gDPLoadTLUT_pal256(D_800F5A44++, D_800F5A68[D_800E30A8]);
        gDPPipeSync(D_800F5A44++);
        for (i = 63; i >= 0; i--) {
            spr = OAM + i * 4;
            if (spr[0] < 0xF1 && !(spr[2] & 0x20)) {
                j = spr[1] * 64;
                gDPLoadTextureBlock_4b(D_800F5A44++, base + j, G_IM_FMT_CI, 16, 8, spr[2] & 3,
                                       G_TX_CLAMP, G_TX_CLAMP, 4, 3, G_TX_NOLOD, G_TX_NOLOD);
                x = spr[3];
                x = (x + 0x1C) * 4;
                y = spr[0] + 1;
                y <<= 2;
                if (!(spr[2] & 0x40)) {
                    gSPTextureRectangle(D_800F5A44++, x, y, x + 32, y + 32, G_TX_RENDERTILE, 0, 0, 1 << 10, 1 << 10);
                } else {
                    gSPTextureRectangle(D_800F5A44++, x, y, x + 32, y + 32, G_TX_RENDERTILE, 7 << 5, 0, -1 << 10, 1 << 10);
                }
            }
        }
        gDPPipeSync(D_800F5A44++);
        gDPSetAlphaCompare(D_800F5A44++, G_AC_NONE);
    }
}
