#include "ctx.h"

typedef struct {
    s32 next;
    u8 b4, b5, b6, b7, b8, b9;
    u8 pad[2];
    u32 tag;
    u32 pad2;
    u8 buf[0x50];
} Blk;

extern Blk D_80100A30;
extern s32 D_800D1E0C;

void func_8000ED10(u8 *a0, u8 a1, s32 a2);
u32 func_800AC890(void);
void func_800AC900(u32 a0);

Blk *func_800BF3C0(void)
{
    u32 r;
    if (D_80100A30.tag != 0xA8000000) {
        D_80100A30.b4 = 3;
        D_80100A30.b5 = 5;
        D_80100A30.b8 = 0xC;
        D_80100A30.b6 = 0xD;
        D_80100A30.b7 = 2;
        D_80100A30.tag = 0xA8000000;
        D_80100A30.b9 = 1;
        func_8000ED10(D_80100A30.buf, 0, 0x60);
        r = func_800AC890();
        D_80100A30.next = D_800D1E0C;
        D_800D1E0C = (s32)&D_80100A30;
        func_800AC900(r);
    }
    return &D_80100A30;
}
