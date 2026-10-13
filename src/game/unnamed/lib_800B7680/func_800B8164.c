#include "ctx.h"

extern s32 D_800D33F0;
extern s32 *D_800D33E8;
extern s32 D_800D33EC;
extern s32 D_800FA840;
void func_8000EE20(void);
void func_8000EE28(void);

void func_800B8164(s32 param_1, s32 param_2)
{
    s32 *p;
    s32 *q;
    s32 *n;
    s32 cnt;

    if (D_800D33F0 == 0) {
        goto done;
    }
    func_8000EE20();
    p = D_800D33E8;
    if (p == 0) {
        goto nf;
    }
loop:
    if (p[4] == param_1) {
        goto found;
    }
    p = (s32 *)p[1];
    if (p != 0) {
        goto loop;
    }
nf:
    func_8000EE28();
    p = 0;
join:
    if (p == 0) {
        goto done;
    }
    p[0xb] = param_2;
    func_8000EE20();
    if (p[0] != 0) {
        ((s32 *)p[0])[1] = p[1];
    }
    if (p[1] != 0) {
        ((s32 *)p[1])[0] = p[0];
    }
    if (p == D_800D33E8) {
        D_800D33E8 = (s32 *)p[1];
    }
    cnt = 0;
    func_8000EE28();
    func_8000EE20();
    q = 0;
    n = D_800D33E8;
    while (n != 0) {
        q = n;
        if (q[0xb] <= p[0xb]) {
            break;
        }
        if ((q[0xc] & 1) != 0) {
            cnt = cnt + 1;
        }
        n = (s32 *)q[1];
    }
    p[1] = (s32)n;
    if (n == 0) {
        goto nullins;
    }
    p[0] = n[0];
    if (n[0] != 0) {
        ((s32 *)n[0])[1] = (s32)p;
    }
    n[0] = (s32)p;
    goto tail;
found:
    func_8000EE28();
    goto join;
nullins:
    if (q != 0) {
        q[1] = (s32)p;
    }
    p[0] = (s32)q;
tail:
    if (D_800D33E8 == 0 || n == D_800D33E8) {
        D_800D33E8 = p;
        p[0] = 0;
    }
    if (cnt < D_800FA840) {
        D_800D33EC = D_800D33EC + 1;
    }
    func_8000EE28();
done:
    ;
}
