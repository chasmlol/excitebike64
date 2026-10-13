#include "ctx.h"

extern u32 boot_func_80001BF0(u32 a);
extern void boot_func_800020FC(u32 d, u32 v, u32 n);
extern void boot_func_80002010(u32 d, u32 s, u32 n);
extern void boot_func_800021C8(u32 a, u32 b, u32 n);
extern void boot_func_80001E0C(u32 d, u32 s, u32 n);
extern void boot_func_80001CF4(u32 d, u32 s, u32 n);
extern u8 D_8F018110[];
extern u32 D_8000F4A0[];
extern u32 D_8000F4A4[];

#define TB(o) (*(u32 *)((u32)D_8000F4A0 + (o)))
#define TF(o) (*(u32 *)((u32)D_8000F4A4 + (o)))
void boot_func_8000223C(u32 p)
{
    u32 dst = 0xB1FE0000;
    u32 src = boot_func_80001BF0(0xB1FFFFF4);
    u32 n = boot_func_80001BF0(0xB1FFFFF8);
    u32 k, o, len, flags, sm;

    if (src > 0xEFFFFFFF)
        src += (u32)D_8F018110;

    while (n != 0) {
        k = 0;
        if (TB(0) < src) {
            o = 0;
            do {
                k++;
                o = k * 8;
            } while (TB(o) < src);
        }
        o = k * 8;
        sm = src - 1;
        len = TB(o) - sm;
        if (n < len)
            len = n;
        flags = TF(o);
        if (!(flags & p)) {
            if (p == 1)
                boot_func_800020FC(dst, 0, len);
        } else if (flags & 4) {
            if (p == 1)
                boot_func_800021C8(dst, src, len);
            else
                boot_func_800021C8(src, dst, len);
        } else if (p == 1) {
            boot_func_80002010(dst, src, len);
        } else if (flags & 8) {
            boot_func_80001E0C(dst, src, len);
        } else {
            boot_func_80001CF4(dst, src, len);
        }
        dst += len;
        n -= len;
        src += len;
    }
}
