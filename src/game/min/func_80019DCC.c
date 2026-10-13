#include "ctx.h"

extern void func_80015124(s32 a);
extern void func_8002D8A0(void);
extern void func_800229B8(void);
extern void func_8005E960(void);
extern void func_8003F658(void);
extern void func_8003A720(void);
extern void func_8006B160(void);
extern void func_8003C2AC(void);
extern void func_8000F600(void);
extern void func_80069C6C(void);
extern void func_8003A84C(void);
extern void func_80072A9C(void);
extern void func_8004B6CC(void);
extern void func_8005B33C(void);

extern s32 D_800EFDBC;
extern u32 D_800C2350;

void func_80019DCC(void)
{
    func_80015124(0);
    func_8002D8A0();
    func_800229B8();
    func_8005E960();
    func_8003F658();
    func_8003A720();
    func_8006B160();
    func_8003C2AC();
    func_8000F600();
    func_80069C6C();
    func_8003A84C();
    func_80072A9C();
    if (D_800EFDBC == 9) {
        func_8004B6CC();
    }
    if (D_800C2350 & 0x800) {
        func_8005B33C();
    }
}
