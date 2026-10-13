#include "ctx.h"

extern s32 *D_800F1430;
extern s32 D_800F0B5C;
extern s32 *D_800F0B64;
extern s32 *D_800F143C;
extern u8 D_800D8C10[];
void *func_800BC908(u32 size, s32 a1, void *a2, s32 a3);

typedef struct {
    s32 val;
    s32 zero;
    s32 pad;
} Rec;

void func_80062E08(void)
{
    s32 i;
    s32 n;
    s32 cnt;
    Rec *out;
    s32 *e;

    D_800F0B5C = D_800F1430[1];
    D_800F0B64 = (s32 *)func_800BC908(D_800F0B5C * 12, 0, D_800D8C10, 0x1B1);
    i = 0;
    n = D_800F0B5C;
    if (n > 0) {
        cnt = n;
        out = (Rec *)D_800F0B64;
        e = D_800F143C;
        do {
            s32 v = e[1];
            e += 4;
            i++;
            out->zero = 0;
            out->val = v;
            out++;
        } while (i < cnt);
    }
}
