#include "ctx.h"

extern s32 D_800D33F0;
extern s32 D_800D33E4;
extern s32 *D_800D33E8;
extern s32 D_800D33EC;
extern s32 D_800FA840;
extern s32 D_800FA844;
typedef struct { s32 f0; s32 f4; s32 f8; s32 fc; s32 f10; s32 f14; } TblEnt;
extern TblEnt D_800FA860[];
extern s32 D_800FC060[];
extern void *D_800FE860;

typedef s32 (*MethodFn)(void *);

void func_8000EE20(void);
void func_8000EE28(void);
void func_8000ED10(u8 *a0, u8 a1, s32 a2);

s32 func_800B7990(s32 a0, s32 a1, s32 a2, s32 a3, s32 a4)
{
    s32 *p;
    s32 *w;
    s32 *q;
    s32 *prev;
    void *obj;
    s32 i;
    s32 cnt;
    s32 quo;
    s32 ret;
    s32 m1;
    s32 t5;
    s32 id;
    MethodFn fn;

    if (D_800D33F0 == 0) {
        return -1;
    }
    if (D_800FA860[a0].f4 == 0) {
        return -1;
    }
    i = 0;
    func_8000EE20();
    m1 = -1;
    __asm__ __volatile__("" : "+r"(m1));
    w = D_800FC060;
    for (i = 0; i < 0x80; i++) {
        if (*w == m1) {
            *w = 0;
            break;
        }
        w += 0x14;
    }
    func_8000EE28();
    if (i == 0x80) {
        p = NULL;
    } else {
        p = D_800FC060 + i * 0x14;
    }
    if (p == NULL) {
        return -1;
    }
    func_8000ED10((u8 *)p, 0, 0x50);
    id = D_800D33E4;
    __asm__ __volatile__("" : "+r"(id));
    p[6] = a0;
    p[7] = a1;
    p[9] = a2;
    p[14] = a2 << 16;
    p[10] = a3;
    t5 = a4;
    __asm__ __volatile__("" : "+r"(t5));
    p[12] = 0;
    p[13] = 0;
    __asm__ __volatile__("" : "+r"(t5));
    p[11] = t5;
    obj = D_800FE860;
    p[2] = -1;
    p[4] = id;
    fn = *(MethodFn *)((u8 *)obj + 0x48);
    D_800D33E4 = id + 1;
    ret = fn(obj);
    quo = (a1 << 8) / D_800FA844;
    cnt = 0;
    p[5] = ret;
    p[18] = quo;
    func_8000EE20();
    q = D_800D33E8;
    prev = NULL;
    while (q != NULL) {
        prev = q;
        if (prev[11] <= p[11]) {
            break;
        }
        if (prev[12] & 1) {
            cnt++;
        }
        q = (s32 *)prev[1];
    }
    p[1] = (s32)q;
    if (q != NULL) {
        p[0] = q[0];
        if (q[0] != 0) {
            ((s32 *)q[0])[1] = (s32)p;
        }
        q[0] = (s32)p;
    } else {
        if (prev != NULL) {
            prev[1] = (s32)p;
        }
        p[0] = (s32)prev;
    }
    if (D_800D33E8 == NULL || q == D_800D33E8) {
        D_800D33E8 = p;
        p[0] = 0;
    }
    if (cnt < D_800FA840) {
        D_800D33EC++;
    }
    func_8000EE28();
    return p[4];
}
