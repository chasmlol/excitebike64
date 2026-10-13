#include "ctx.h"

void func_8000ED10(u8 *a0, u8 a1, s32 a2);
void func_8000EF8C(char *a0, char *a1);
s32 func_800AEC10(char *s);
void func_8005CD30(char *p, f32 v, s32 n);

void func_8005D050(char *param_1, f32 param_2, f32 param_3)
{
    char buf[40];
    s32 n;
    s32 i;

    func_8000ED10((u8 *)buf, 0x20, 0x1E);
    func_8000EF8C(buf, param_1);
    n = func_800AEC10(buf);
    func_8005CD30(buf + n, param_2, 3);
    i = 0;
    if (0 < n + 6) {
        do {
            if (buf[i] == 0) {
                buf[i] = ' ';
            }
            i++;
        } while (i < n + 6);
    }
    func_8005CD30(buf + 6 + n, param_3, 3);
}
