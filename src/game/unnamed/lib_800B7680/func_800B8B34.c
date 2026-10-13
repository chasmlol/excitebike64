#include "ctx.h"

void func_8000ED10(u8 *a0, u8 a1, s32 a2);
void func_800B9F74(void);
u8 *func_800BC908(s32 a, s32 b, u8 *c, s32 d);

typedef struct {
    s32 pad0;
    s32 (*init)(s32, s32, s32, s32, s32 *, s32 *);
    void (*fail)(void);
    s32 pad[17];
    void (*setcb)(void (*)(void));
} Iface;

extern Iface *D_800FE860;
extern u8 D_800DB160[];
extern u8 *D_800FA854;
extern s32 D_800FA840;
extern s32 D_800FA848;
extern s32 D_800FA84C;
extern s32 D_800FA844;
extern s32 D_800FA850;
extern s32 D_800D33E8;
extern s32 D_800D33F0;
extern s32 D_800D33E0;
extern u8 D_800FA860[];
extern u8 D_800FC060[];

s32 func_800B8B34(s32 p1, s32 p2, s32 p3, s32 p4, s32 *p5, s32 *p6)
{
    s32 iv;
    u8 *ptr;
    s32 v5, v6;

    iv = D_800FE860->init(p1, p2, p3, p4, p5, p6);
    if (iv != 0) {
        return iv;
    }
    ptr = func_800BC908(p1 * 4, 0, D_800DB160, 0x19B);
    D_800FA854 = ptr;
    if (ptr == NULL) {
        D_800FE860->fail();
        return 1;
    }
    D_800FA854 = ptr;
    D_800FA840 = p1;
    D_800FA848 = p2;
    v6 = *p6;
    D_800FA84C = p4;
    D_800FA844 = v6;
    v5 = *p5;
    D_800D33E8 = 0;
    D_800FA850 = v5;
    func_8000ED10(ptr, 0, p1 * 4);
    func_8000ED10(D_800FA860, 0, 0x1800);
    func_8000ED10(D_800FC060, 0xFF, 0x2800);
    D_800FE860->setcb(func_800B9F74);
    D_800D33F0 = 1;
    D_800D33E0 = 0;
    return 0;
}
