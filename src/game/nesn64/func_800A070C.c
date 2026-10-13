#include "ctx.h"

typedef struct Nes {
    u8 cpu[0x10000];
    u8 ppu[0x10000];
} Nes;

#define NES ((Nes *)0x80380000)

extern Gfx *D_800F5A44;
extern s32 D_800F5A48;
extern s32 D_800F5A4C;
extern u8 *D_800F5A54;
extern u16 *D_800F5A58;
extern u16 *D_800F5A60[];
extern s32 D_800F5AA0;
extern s32 D_800E30A8;

void func_800A070C(s32 row)
{
    s32 i, j, k, idx;
    u16 **pp;
    u8 *base;
    u8 *ntp;
    s32 nt;
    s32 y;
    s32 sx, fine;
    s32 col, x;
    s32 pal, key, ofs, tile, a, sh;

    if ((NES->cpu[0x2001] & 8) && D_800F5AA0 >= 10) {
        D_800F5A4C = -1;
        if (D_800F5A48 == 0) {
            k = 0x3F00;
            idx = 0;
            for (i = 0; i < 4; i++) {
                for (j = 0; j < 4; j++) {
                    D_800F5A60[D_800E30A8][idx] = D_800F5A58[NES->ppu[k]];
                    k++;
                    if (j == 0) {
                        D_800F5A60[D_800E30A8][idx] &= 0xFFFC;
                    } else {
                        D_800F5A60[D_800E30A8][idx] |= 3;
                    }
                    idx++;
                }
                idx += 12;
            }
            D_800F5A48 = 1;
            gDPPipeSync(D_800F5A44++);
            gDPLoadTLUT_pal256(D_800F5A44++, D_800F5A60[D_800E30A8]);
        }
        base = D_800F5A54;
        gDPPipeSync(D_800F5A44++);
        if (NES->cpu[0x2000] & 0x10) {
            base += 0x4000;
        }
        nt = (NES->cpu[0x2000] & 3) << 10;
        nt += 0x2000;
        ntp = &NES->ppu[nt];
        y = row << 5;
        sx = NES->ppu[0x411C] >> 3;
        fine = NES->ppu[0x411C] & 7;
        x = (0x1C - fine) << 2;
        nt += y;
        nt += sx;
        col = sx + 1;
        x += 0x20;
        nt++;

        for (i = col; i < 0x20; i++) {

            tile = NES->ppu[nt];
            ofs = tile << 6;
            a = (col >> 2) + ((row >> 2) << 3);
            sh = ((col >> 1) & 1) | (row & 2);
            sh <<= 1;
            pal = (ntp[a + 0x3C0] >> sh) & 3;
            key = tile | (pal << 8);
            if (key != D_800F5A4C) {
                gDPLoadTextureBlock_4b(D_800F5A44++, base + ofs, G_IM_FMT_CI, 16, 8, pal,
                                       G_TX_WRAP, G_TX_WRAP, 4, 3, G_TX_NOLOD, G_TX_NOLOD);
            }
            D_800F5A4C = key;
            gSPTextureRectangle(D_800F5A44++, x, y, x + 32, y + 32, G_TX_RENDERTILE, 0, 0, 1 << 10, 1 << 10);
            x += 32;
            nt++;
            col++;
        }
        nt = ((NES->cpu[0x2000] & 3) << 10) ^ 0x400;
        nt += 0x2000;
        ntp = &NES->ppu[nt];
        col = 0;
        nt += row << 5;
        if (fine != 0) {
            sx++;
        }
        for (i = 0; i < sx; i++) {

            tile = NES->ppu[nt];
            ofs = tile << 6;
            a = (col >> 2) + ((row >> 2) << 3);
            sh = ((col >> 1) & 1) | (row & 2);
            sh <<= 1;
            pal = (ntp[a + 0x3C0] >> sh) & 3;
            key = tile | (pal << 8);
            if (key != D_800F5A4C) {
                gDPLoadTextureBlock_4b(D_800F5A44++, base + ofs, G_IM_FMT_CI, 16, 8, pal,
                                       G_TX_CLAMP, G_TX_CLAMP, 4, 3, G_TX_NOLOD, G_TX_NOLOD);
            }
            D_800F5A4C = key;
            gSPTextureRectangle(D_800F5A44++, x, y, x + 32, y + 32, G_TX_RENDERTILE, 0, 0, 1 << 10, 1 << 10);
            x += 32;
            nt++;
            col++;
        }
    }
}
