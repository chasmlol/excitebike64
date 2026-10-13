#include "ctx.h"

void func_8000ED30(char *dst, char *src);
s32 func_80060078(u8 *s, u32 *a1, u32 *a2, u32 *a3);
void func_8000EE30(s32 v, char *out);
s32 func_800AEC10(char *s);

extern u8 D_800D946C[];
extern u8 D_800D9478[];
extern u8 D_800D9480[];
extern char D_800CB210[];
extern char D_800D91CC[];
extern char D_800D91D0[];
extern char D_800D9474[];
extern s16 D_800F01F8;

s32 func_800732D0(u8 *src, char *dst, s32 flag)
{
    s32 trk, q, w, ret;
    s32 n, m, v;

    dst[0] = 0;
    if (func_80060078(src, (u32 *)&trk, (u32 *)&q, (u32 *)&w) == 0) {
        w = w - 0x102;
        __builtin_memcpy(dst, D_800D946C, 8);
        func_8000ED30(dst, D_800CB210 + trk * 160);
        if (flag == 0) {
            func_8000ED30(dst, D_800D91CC);
            v = w / 3600;
            if (v < 10) {
                func_8000ED30(dst, D_800D91D0);
            }
            func_8000EE30(v, dst + func_800AEC10(dst));
            func_8000ED30(dst, D_800D9474);
            n = func_800AEC10(dst);
            v = w / 60 % 60;
            dst[n] = v / 10 + '0';
            n++;
            dst[n] = v % 10 + '0';
            n++;
            dst[n] = '.';
            v = w % 60 * 100 / 60;
            n++;
            dst[n] = v / 10 + '0';
            n++;
            dst[n] = v % 10 + '0';
            n++;
            dst[n] = 0;
        }
        ret = D_800F01F8;
        ret = ret ^ 3;
    } else {
        if (src[4] != 0x35) {
            __builtin_memcpy(dst, D_800D9480, 28);
            return 1;
        }
        __builtin_memcpy(dst, D_800D9478, 8);
        m = func_800AEC10(dst);
        dst[m + 10] = 0;
        m = func_800AEC10(dst);
        __builtin_memcpy(dst + m, src + 6, 10);
        ret = D_800F01F8;
        ret = ret ^ 1;
    }
    return (ret == 0) << 1;
}
