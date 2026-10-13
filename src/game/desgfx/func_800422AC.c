#include "ctx.h"

extern Gfx *D_800C29E0;
extern s32 D_800CA3DC[];
extern s32 D_800CA3EC[];
extern s32 D_800CA3FC[];
extern u8 D_800D6FD0[];
void *func_800BC908(s32 size, u32 flags, s32 owner, s32 tag);
void func_800AEC34(void *dst, void *src, s32 len);

s32 func_800422AC(s32 mask)
{
    Gfx *gdl = D_800C29E0;
    s32 i, j, k, b, c;

    for (i = 0; i < 2; i++) {
        for (j = 0; j < 2; j++) {
            k = i * 6 - j * 4;
            gSP2Triangles(gdl++, k + 10, k + 11, k + 16, 0, k + 10, k + 6, k + 11, 0);
            gSP2Triangles(gdl++, k + 11, k + 12, k + 16, 0, k + 11, k + 6, k + 12, 0);
        }
    }

    for (i = 0; i < 4; i++) {
        if ((mask >> i) & 1) {
            j = D_800CA3DC[i];
            b = D_800CA3EC[i];
            c = D_800CA3FC[i];
            gSP2Triangles(gdl++, j, j + b + c, j + 2 * b, 0, j + 2 * b, j + 3 * b + c, j + 4 * b, 0);
        } else {
            j = D_800CA3DC[i];
            b = D_800CA3EC[i];
            c = D_800CA3FC[i];
            gSP2Triangles(gdl++, j, j + b + c, j + b, 0, j + b, j + b + c, j + 2 * b, 0);
            gSP2Triangles(gdl++, j + 2 * b, j + 3 * b + c, j + 3 * b, 0, j + 3 * b, j + 3 * b + c, j + 4 * b, 0);
        }
    }

    gSPEndDisplayList(gdl++);
    i = (u8 *)gdl - (u8 *)D_800C29E0;
    gdl = func_800BC908(i, 0, (s32)D_800D6FD0, 0x508);
    func_800AEC34(gdl, D_800C29E0, i);
    return (s32)gdl;
}
