#include "ctx.h"

extern u32 D_800EFD64;
extern u32 D_800EFDBC;
extern u32 D_800C9570;
extern u32 D_800EFD70;
extern u32 D_800E7728;
extern u32 D_800C2358;
extern u32 D_800CCCCC;
extern u32 D_800F0DA0;
extern u32 D_800CA920;

void func_80015CD8(void)
{
    u32 *r;
    u32 t;
    u32 f;

    func_8000D0D8();
    if ((*(u32 *)(D_800EFD64 + 0x50) & 2) != 0 && (D_800EFDBC - 0xD) < 2) {
        func_800BCB04(D_800C9570);
        D_800C9570 = 0;
    }
    D_800EFD70 = D_800EFD70 & 0xFFFEFFFF;
    func_80062D38();
    func_8005E960();
    func_8003F658();
    r = (u32 *)&D_800EFD70;
    func_8000F600();
    func_8004B6CC();
    func_8003A720();
    func_8006B160();
    func_800020CC();
    func_80066268();
    func_800229B8();
    func_8005AE4C();
    func_8003C2AC();
    func_800BFDD4(D_800E7728);
    func_8006A944();
    func_80069C6C();
    func_80071978();
    func_8003A84C();
    func_80072A9C();
    if (D_800C2358 != 0) {
        func_8000CC48();
        func_8001A0D4();
    }
    t = D_800EFD70 & 0xFFFFBFFF;
    f = D_800EFD70 & 1;
    D_800EFD70 = t;
    if (f == 0) {
        func_8004834C();
    }
    func_80015124(1);
    D_800CCCCC = 0;
    D_800F0DA0 = 1;
    if (D_800C2358 != 0) {
        D_800CA920 = 7;
        r[0x13] = 0;
    }
}
