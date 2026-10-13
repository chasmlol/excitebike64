#include "ctx.h"

extern u32 *D_800E781C;
extern u32 *D_800E77C4;
extern u32 *D_800E77F8;
extern Gfx *func_80025060(Gfx *g, s32 a, s32 b, s32 c, s32 d);

/* G_MOVEWORD 0xDB, index 6 (segment table), offset seg*4 */
void func_8002803C(Gfx *gdl, s32 a, s32 b, s32 c) {
    Gfx *q;

    gDma1p(gdl++, 0xDB, D_800E781C[1], 0x4, 0x6);
    if (D_800E77C4 != 0) {
        gDma1p(gdl++, 0xDB, D_800E77C4[1], 0x8, 0x6);
    }
    if (D_800E77F8 != 0) {
        gDma1p(gdl++, 0xDB, D_800E77F8[1], 0xC, 0x6);
    }
    q = func_80025060(gdl, a, b, c, 0);
    func_80025060(q, a, b, c, 1);
}
