#include "ctx.h"

typedef struct {
    s16 x;
    s16 y;
    s16 z;
    s16 pad;
    s32 unk8;
    s32 unkC;
} Rec;

typedef struct {
    Rec r[25];
} RecSet;

extern s16 D_800C2C60[];
typedef struct {
    RecSet cur;
    RecSet prev;
} Both;

extern Both D_800E8EF0;
extern RecSet D_800E9080[];
extern void func_8000ED10(u8 *a0, u8 a1, s32 a2);

void func_800358C0(void)
{
    s32 i;
    s32 a;
    s32 u1;
    s32 u3;

    func_8000ED10((u8 *)&D_800E8EF0, 0, 800);
    for (i = 0; i < 25; i++) {
        D_800E8EF0.cur.r[i].z = 0x32;
    }
    for (i = 0; i < 7; i++) {
        a = i * 0x2000 + 0xA000;
        D_800E8EF0.cur.r[i * 3].x = (D_800C2C60[((a >> 4) - 0x400) & 0xFFF] * 15) >> 12;
        D_800E8EF0.cur.r[i * 3].y = (D_800C2C60[(a >> 4) & 0xFFF] * 5) >> 11;
        u1 = (a + 0x200) >> 4;
        D_800E8EF0.cur.r[i * 3 + 1].x = (D_800C2C60[(u1 - 0x400) & 0xFFF] * 0x41) >> 14;
        u3 = (a - 0x200) >> 4;
        D_800E8EF0.cur.r[i * 3 + 1].y = (D_800C2C60[u1 & 0xFFF] * 0x2D) >> 14;
        D_800E8EF0.cur.r[i * 3 + 2].x = (D_800C2C60[(u3 - 0x400) & 0xFFF] * 0x41) >> 14;
        D_800E8EF0.cur.r[i * 3 + 2].y = (D_800C2C60[u3 & 0xFFF] * 0x2D) >> 14;
    }
    D_800E9080[0] = D_800E9080[-1];
}
