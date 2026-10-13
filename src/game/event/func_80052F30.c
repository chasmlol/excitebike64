#include "ctx.h"

typedef struct { char c[9]; } Str9;
typedef struct { char c[2]; } Str2;

extern Str9 D_800D8438;
extern char D_800D8444[];
extern char D_800D8448[];
extern char D_800D844C[];

void func_8000EE30(s32 v, char *buf);
s32 func_800AEC10(char *buf);
void func_8000EF8C(char *a0, char *a1);
void func_8000ED30(char *dst, char *src);

void func_80052F30(s32 param_1, char *param_2)
{
    char buf[32];

    if (param_1 > 0x9E340) {
        *(Str9 *)param_2 = D_800D8438;
        return;
    }
    func_8000EE30(param_1 / 0xE10, buf);
    if (func_800AEC10(buf) == 1) {
        *(Str2 *)param_2 = *(Str2 *)D_800D8444;
        func_8000ED30(param_2, buf);
    } else {
        func_8000EF8C(param_2, buf);
    }
    func_8000ED30(param_2, D_800D8448);
    func_8000EE30((param_1 / 60) % 60, buf);
    if (func_800AEC10(buf) == 1) {
        func_8000ED30(param_2, D_800D8444);
        func_8000ED30(param_2, buf);
    } else {
        func_8000ED30(param_2, buf);
    }
    func_8000ED30(param_2, D_800D844C);
    func_8000EE30(((param_1 % 60) * 100) / 60, buf);
    if (func_800AEC10(buf) == 1) {
        func_8000ED30(param_2, D_800D8444);
        func_8000ED30(param_2, buf);
    } else {
        func_8000ED30(param_2, buf);
    }
}
