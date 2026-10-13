#include "ctx.h"

void func_8000EF8C(char *a0, char *a1);
s32 func_800AEC10(char *s);
void func_8000EE30(s32 a, char *b);

void func_8005D1D0(char *param_1, s32 param_2) {
    char pad[16];
    char buf[40];
    char *b;

    b = buf;
    func_8000EF8C(b, param_1);
    func_8000EE30(param_2, b + func_800AEC10(b));
}
