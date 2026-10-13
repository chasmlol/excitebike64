#include "ctx.h"

typedef struct {
    u8 pad[0x22];
    s16 n;
} EbTbl;

extern u8 D_800F0C98[];
extern EbTbl D_800F05A8;
extern s16 D_800F05CA;
extern u8 *D_800CA930;
extern s32 D_800EFDBC;
void func_8000ED10();

void func_800650E4(void)
{
    s32 i;
    s32 j;
    s32 n;
    u8 *arr;
    EbTbl *t;
    u8 *hb;

    func_8000ED10(D_800F0C98, -1, 6);
    if (D_800EFDBC == 1) {
        EbTbl *p = &D_800F05A8;
        i = 0;
        if (p->n > 0) {
            t = p;
            do {
                j = 0;
                if (t->n > 0) {
                    arr = D_800CA930;
                    hb = (u8 *)0x800F0000;
                    __asm__ __volatile__("" : "+r"(hb));
                    n = *(s16 *)(hb + 0x5CA);
inner:
                    if (*(u8 *)((u32)arr + (u32)j + 0x6c) == i) {
                        goto store;
                    }
                    j++;
                    if (j < n) {
                        goto inner;
                    }
                }
store:
                D_800F0C98[i] = (u8)j;
                i++;
            } while (i < t->n);
        }
    }
}
