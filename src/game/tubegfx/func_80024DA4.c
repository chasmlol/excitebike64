#include "ctx.h"

typedef struct { u32 a; u32 b; } Pair;

extern void func_80067AC4(void);
extern void func_800BCB04();
extern void func_80028278(void);
extern void func_8002E190(void);
extern void func_80072E7C(void);

extern u32 D_800E7814;
extern u32 D_800E77D4;
extern u32 D_800E7834;
extern u32 *D_800E78A0;
extern u32 *D_800E7828;
extern Pair D_800E7798;
extern u32 D_800E77F8;
extern u32 D_800E77C4;
extern u32 D_800C9648;
extern u32 D_800E7824;
extern u32 D_800C963C;
extern u32 D_800C9638;
extern u32 *D_800C9644;
extern u32 *D_800C9640;

void func_80024DA4(void)
{
    u32 i;

    func_80067AC4();
    i = 0;
    func_800BCB04(D_800E7814);
    func_800BCB04(D_800E77D4);
    func_800BCB04(D_800E7834);
    if (*D_800C9644 != 0) {
        do {
            func_800BCB04(D_800E78A0[i]);
            i++;
        } while (i < *D_800C9644);
    }
    i = 0;
    if (*D_800C9640 != 0) {
        do {
            func_800BCB04(D_800E7828[i]);
            i++;
        } while (i < *D_800C9640);
    }
    func_800BCB04(D_800E78A0);
    func_800BCB04(D_800E7828);
    func_80028278();
    func_800BCB04(D_800E7798.a);
    func_800BCB04(D_800E7798.b);
    func_800BCB04(D_800C963C);
    func_800BCB04(D_800C9638);
    D_800C963C = 0;
    D_800C9638 = 0;
    func_8002E190();
    func_80072E7C();
    D_800E77F8 = 0;
    D_800E77C4 = 0;
    D_800C9648 = 0;
    D_800E7824 = 0;
    D_800C9644 = (u32 *)0;
}
