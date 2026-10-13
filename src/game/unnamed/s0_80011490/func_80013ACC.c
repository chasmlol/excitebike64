#include "ctx.h"

typedef struct {
    u8 pad0[0x34];
    s32 pos[3];
    s32 rot[3][3];
    u8 pad64[0x80 - 0x64];
} Obj;

typedef struct {
    s16 i[4][4];
    u16 f[4][4];
} MtxS;

void func_80013ACC(Obj *src, MtxS *dst, s32 n)
{
    s32 i;

    for (i = 0; i < n; i++) {
        dst->i[0][0] = src->rot[0][0] >> 14;
        dst->f[0][0] = src->rot[0][0] << 2;
        dst->i[0][1] = src->rot[0][1] >> 14;
        dst->f[0][1] = src->rot[0][1] << 2;
        dst->i[0][2] = src->rot[0][2] >> 14;
        dst->f[0][2] = src->rot[0][2] << 2;
        dst->i[0][3] = 0;
        dst->f[0][3] = 0;
        dst->i[1][0] = src->rot[1][0] >> 14;
        dst->f[1][0] = src->rot[1][0] << 2;
        dst->i[1][1] = src->rot[1][1] >> 14;
        dst->f[1][1] = src->rot[1][1] << 2;
        dst->i[1][2] = src->rot[1][2] >> 14;
        dst->f[1][2] = src->rot[1][2] << 2;
        dst->i[1][3] = 0;
        dst->f[1][3] = 0;
        dst->i[2][0] = src->rot[2][0] >> 14;
        dst->f[2][0] = src->rot[2][0] << 2;
        dst->i[2][1] = src->rot[2][1] >> 14;
        dst->f[2][1] = src->rot[2][1] << 2;
        dst->i[2][2] = src->rot[2][2] >> 14;
        dst->f[2][2] = src->rot[2][2] << 2;
        dst->i[2][3] = 0;
        dst->f[2][3] = 0;
        dst->i[3][0] = src->pos[0];
        dst->f[3][0] = 0;
        dst->i[3][1] = src->pos[1];
        dst->f[3][1] = 0;
        dst->i[3][2] = src->pos[2];
        dst->f[3][2] = 0;
        dst->i[3][3] = 1;
        dst->f[3][3] = 0;
        dst++;
        src++;
    }
}
