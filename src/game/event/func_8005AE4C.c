#include "ctx.h"

typedef struct {
    s32 limit;
    s32 counter;
    s32 arg;
} Ctr;

extern Ctr D_800CA924;
extern u32 D_800EFD70;
extern s32 D_800EFDBC;
extern s32 D_800F0624;
void func_800BCB04(s32 x);
void func_8006F5F8(s32 x, s32 y);

void func_8005AE4C(void)
{
    if (0 < D_800CA924.limit && (D_800EFD70 & 4) == 0) {
        D_800CA924.counter = D_800CA924.counter + 1;
        if (D_800CA924.counter == D_800CA924.limit) {
            func_800BCB04(D_800CA924.arg);
        }
    }
    if (D_800EFDBC == 9 && D_800F0624 != 0) {
        func_800BCB04(D_800F0624);
    }
    func_8006F5F8(1, 0);
}
