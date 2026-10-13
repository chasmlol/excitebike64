#include "ctx.h"

extern void *(*D_800D560C)(void *);
extern u32 D_800D5614;
extern u8 *D_800D5610;
extern u8 D_800DB170[];

void func_8000ECB8();
void func_800AEC34(void *a, void *b, u32 c);
void func_800BCB04(void *a);
void *func_800BC908(u32 a, u32 b, void *c, u32 d);

typedef struct {
    u32 flags;
    u32 size;
} Hdr;

void func_800BB350(Hdr **param_1)
{
    Hdr *p;
    Hdr *q;
    u8 **bp;

    p = *param_1;
    if ((p->flags & 0xF) >= 2) {
        if (D_800D560C != 0) {
            if (D_800D5614 < p->size + 0x1c) goto spin;
            func_800AEC34(D_800D5610, p, p->size + 0x1c);
            func_800BCB04(*param_1);
            p = (Hdr *)D_800D5610;
            goto join;
        spin:
            while (1) {}
        join:
            q = D_800D560C(p);
            bp = &D_800D5610;
            if (q == 0) {
                func_8000ECB8(1);
            }
            if (p != (Hdr *)*bp) {
                func_800BCB04(p);
            }
            if (q == (Hdr *)*bp) {
                p = func_800BC908(p->size + 0x1c, 0, D_800DB170, 0x67b);
                func_800AEC34(p, q, p->size + 0x1c);
                q = p;
            }
            *param_1 = q;
        } else {
            func_8000ECB8(1);
        }
    }
}
