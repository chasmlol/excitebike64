#include "ctx.h"

extern u8 D_800EFD70[];
extern s32 D_800E3700;
void func_8005EB68(u32 a, u32 b);

void func_8005F924(void)
{
    s32 i;
    u8 *q;
    u8 *p;
    u32 lo, hi;

    func_8005EB68(D_800EFD70[0x3F], 8);
    func_8005EB68(D_800EFD70[0x47], 8);
    func_8005EB68(D_800EFD70[0x13], 8);
    func_8005EB68(D_800EFD70[0x43], 8);
    for (i = 0; i < 6; i++) {
        q = D_800EFD70 + i;
        lo = q[0x43C] & 0xF;
        hi = (q[0x7AA] & 0xF) << 4;
        func_8005EB68(hi | lo, 8);
    }
    for (i = 0; i < 6; i++) {
        p = D_800EFD70;
        if (D_800E3700 == i) {
            func_8005EB68(((s32)p[i * 4 + 0x1F] | -0x80) & 0xFF, 8);
        } else {
            func_8005EB68(p[i * 4 + 0x1F], 8);
        }
    }
}
