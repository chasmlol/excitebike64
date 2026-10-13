#include "ctx.h"

extern s32 func_80065858(void);
extern u32 func_8005AED8(s32 key);
extern s32 func_8005B58C(s32 key);
extern void func_800BFEA4(u32 *p);
extern void func_8000ECB8() __attribute__((noreturn));
extern void *func_800BC908(s32 a, s32 b, void *c, s32 d);
extern void func_800AEC34(void *dst, void *src, s32 n);

extern s32 D_800D1514;
extern u8 D_800D6A50[];
extern u16 *D_800C9638;
extern u32 *D_800C9644;
extern u8 *D_800C29E0;
extern s32 *D_800C963C;
extern s32 *D_800E7728;
extern s32 D_800EFD70[];

void func_80027B7C(void)
{
    s32 idx;
    s32 i;
    u32 j;
    s32 n;
    s32 a, b;
    u8 *dst;
    s32 *offs;
    s32 sz;
    s32 *q;
    u16 *p;
    s32 t1, t2;

    while (func_80065858() != 0) {
    }
    D_800C9638 = (u16 *)func_8005AED8(D_800D1514);
    if (*D_800C9644 != *D_800C9638) {
        func_8000ECB8(1);
    }
    idx = 1;
    dst = D_800C29E0;
    offs = (s32 *)(D_800C29E0 + 0x1E000);
    for (i = 0; i < 4; i++) {
        for (j = 0; j < *D_800C9644; j++) {
            a = D_800C9638[idx];
            b = D_800C9638[idx + 1];
            if (i == 0 || i == D_800EFD70[15] - 1) {
                n = a + b;
                *offs++ = dst - D_800C29E0;
                func_800AEC34(dst, (u16 *)((idx << 1) + (s32)D_800C9638), (n + 2) * 2);
                dst += n * 2 + 4;
            }
            t1 = idx + 2;
            t2 = a + b;
            idx = t1 + t2;
        }
    }
    idx = 0;
    func_800BFEA4((u32 *)(D_800E7728[1] + func_8005B58C(D_800D1514) * 4));
    i = dst - D_800C29E0;
    D_800C9638 = func_800BC908(i, idx, D_800D6A50, 0x61B);
    func_800AEC34(D_800C9638, D_800C29E0, i);
    i = (s32)offs + (-0x1E000 - (s32)D_800C29E0);
    D_800C963C = func_800BC908(i, idx, D_800D6A50, 0x620);
    func_800AEC34(D_800C963C, D_800C29E0 + 0x1E000, i);
    p = D_800C9638;
    q = D_800C963C;
    for (; idx < i / 4; idx++) {
        *q++ += (s32)p;
    }
}
