#include "ctx.h"

typedef struct {
    u8 b0;
    u8 b1;
    u8 b2;
    u8 b3;
} Ent;

extern s32 D_800F5A54;
extern u16 *D_800F5A68[];
extern u16 *D_800C29D8[];

void func_800AD7E0(s32 a, s32 b);
void func_800AD910(void *p, s32 n);

void func_800A11BC(s32 param)
{
    u8 *st;
    s32 bas;
    s32 i;
    s32 row, k, col;
    s32 t0;
    u8 *src;
    u16 *dst;
    Ent *e;
    u32 v;

    st = (u8 *)0x80380000;
    if (st[0x2001] & 0x10) {
        func_800AD7E0(0, 0x400000);
        bas = D_800F5A54;
        if (st[0x2000] & 8) {
            bas += 0x4000;
        }
        for (i = 63; i >= 0; i--) {
            e = (Ent *)0x80394000 + i;
            if (e->b0 < 0xF1 && (e->b2 & 0x20)) {
                t0 = e->b2 & 3;
                t0 <<= 4;
                row = e->b1 << 6;
                src = (u8 *)(bas + row);
                col = e->b3 + 0x1c;
                dst = D_800C29D8[param] + ((e->b0 + 1) * 0x140 + col);
                for (row = 0; row < 8; row++) {
                    col = e->b3 + 0x1c;
                    if (e->b2 & 0x40) {
                        src += 3;
                        for (k = 0; k < 4; k++) {
                            v = *src & 0xF;
                            if (v != 0 && col > 0x23 && col < 0x11c && !(*dst & 2)) {
                                *dst = D_800F5A68[param][v + t0];
                            }
                            dst++;
                            col++;
                            v = *src >> 4;
                            if (v != 0 && col > 0x23 && col < 0x11c && !(*dst & 2)) {
                                *dst = D_800F5A68[param][v + t0];
                            }
                            dst++;
                            col++;
                            src--;
                        }
                        src += 9;
                    } else {
                        for (k = 0; k < 4; k++) {
                            v = *src >> 4;
                            if (v != 0 && col > 0x23 && col < 0x11c && !(*dst & 2)) {
                                *dst = D_800F5A68[param][v + t0];
                            }
                            dst++;
                            col++;
                            v = *src & 0xF;
                            if (v != 0 && col > 0x23 && col < 0x11c && !(*dst & 2)) {
                                *dst = D_800F5A68[param][v + t0];
                            }
                            dst++;
                            col++;
                            src++;
                        }
                        src += 4;
                    }
                    dst += 0x138;
                }
            }
        }
        func_800AD910(D_800C29D8[param], 0x25800);
    }
}
