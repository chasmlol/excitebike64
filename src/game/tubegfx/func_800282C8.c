#include "ctx.h"

extern u32 *D_800C9640;
extern u32 *D_800E7834;
void func_800679C0(void);
void func_80072EF0(void);

void func_800282C8(void)
{
    u32 *p = D_800C9640;
    u32 *c;
    u32 i = 0;
    u32 *q;
    u32 m;

    if (*p != 0) {
        m = 0xfffffff7;
        c = p;
        q = D_800E7834;
        do {
            i++;
            *q = *q & m;
            q++;
        } while (i < *c);
    }
    func_800679C0();
    func_80072EF0();
}
