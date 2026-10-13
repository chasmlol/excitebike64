#include "ctx.h"

typedef struct { s32 a; s32 b; } Pr;
typedef struct { u8 pad[0x2C]; s32 mode; } Cfg;

extern s32 D_800E80D0;
extern s32 D_800E80D4;
extern s32 D_800E8508;
extern s32 D_800E84F8;
extern s32 D_800E8938;
extern s32 D_800E8958;
extern Cfg *D_800EFD64;
extern Pr D_800E8940;
extern Pr D_800E8950;
typedef struct { u16 n; u16 id[1]; } Cell;
extern Cell **D_800E8D60;

extern s32 func_8002EB5C(s32 x, s32 z);
extern s32 func_8002E29C(s32 id, s32 x, s32 y, s32 z);

s32 func_8002E920(s32 x, s32 y, s32 z)
{
    s32 ix, iz, i, k;

    D_800E80D0 = -99999;
    D_800E8508 = 0;
    D_800E84F8 = 0;
    D_800E8938++;
    if (D_800EFD64->mode == 2) {
        return func_8002EB5C(x, z);
    }
    if (D_800E8958 != -1 && func_8002E29C(D_800E8958, x, y, z) != 0) {
        return D_800E80D4;
    }
    ix = x >> 4;
    ix = (ix - D_800E8940.a) * 16 / (D_800E8950.a - D_800E8940.a);
    iz = z >> 4;
    if (ix < 0) ix = 0;
    if (ix > 15) ix = 15;
    iz = (iz - D_800E8940.a) * 16 / (D_800E8950.b - D_800E8940.b);
    if (iz < 0) iz = 0;
    if (iz > 15) iz = 15;
    k = iz * 16 + ix;
    for (i = 0; i < D_800E8D60[k]->n; i++) {
        if (D_800E8D60[k]->id[i] != D_800E8958 && func_8002E29C(D_800E8D60[k]->id[i], x, y, z) != 0) {
            return D_800E80D4;
        }
    }
    if (D_800E80D0 != -99999) {
        return D_800E80D0;
    }
    return D_800E80D4;
}
