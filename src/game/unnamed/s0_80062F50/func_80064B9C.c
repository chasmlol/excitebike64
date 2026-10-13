#include "ctx.h"

extern u8 D_800F0C90[];

u32 func_8000EFE8(void);

void func_80064B9C(s32 p1, s32 n)
{
    s32 arr[8];
    s32 *p;
    s32 i;
    s32 val;
    s32 a;
    s32 b;
    u8 flag;
    s32 t;

    i = 0;
    if (n > 0) {
        p = arr;
        val = i;
        do {
            *p = val;
            a = func_8000EFE8();
            b = func_8000EFE8();
            val += 100;
            i++;
            *p = *p + (a + b);
            p++;
        } while (i < n);
    }
    do {
        flag = 0;
        for (i = 0; i < n; i++) {
            if (arr[i] > arr[i + 1]) {
                t = D_800F0C90[i];
                D_800F0C90[i] = D_800F0C90[i + 1];
                D_800F0C90[i + 1] = t;
                t = arr[i];
                arr[i] = arr[i + 1];
                arr[i + 1] = t;
                flag = 1;
            }
        }
    } while (flag);
}
