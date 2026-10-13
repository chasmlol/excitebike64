#include "ctx.h"

typedef struct { u8 pad[3]; u8 f3; } S4;
typedef struct { u16 flags; u8 pad[4]; } S6;
typedef struct { u8 pad[0x68]; } S68;

extern u8 D_80017E78[];
extern u8 D_800180D0[];
extern u8 D_80017E94[];
extern S4 D_80017CB0[];
extern S6 D_80017C10[];
extern u8 D_80017C40[];
extern S68 D_80017CC0[];
extern S68 D_80017F10[];

void func_800074B0();
void func_80008290();
s32 func_80007010();
void func_80006D90();
void func_80008030();
void func_80006E10();
void func_80006E30();
void func_80006EB8();
u32 func_80009FB0();
s32 func_80009C74();
void func_80009B30();
s32 func_80009930();

s32 boot_func_8000182C(void)
{
    s32 n;
    s32 bVar1;
    s32 bVar2;
    s32 dd;
    s32 ret;
    u32 buf[2];
    u32 *bp;

    func_800074B0(D_80017E78, D_800180D0, 8);
    func_80008290(5, D_80017E78, 0);
    if (func_80007010(D_80017E78, D_80017E94, D_80017CB0) != 0) {
        while (1) {}
    }
    func_80006D90(D_80017E78);
    bp = buf;
    func_80008030(D_80017E78, bp, 1);
    func_80006E10(D_80017CB0);
    func_80006E30(D_80017E78);
    func_80008030(D_80017E78, bp, 1);
    func_80006EB8(D_80017C10);
    bVar1 = 0;
    bVar2 = 0;
    for (n = 0; n < 4; n++) {
        if (D_80017CB0[n].f3 == 0) {
            if ((D_80017C10[n].flags & 0x1000) && n == 0) {
                bVar1 = 1;
            }
            switch (func_80009FB0(D_80017E78, &D_80017CC0[n], n)) {
            case 0: dd = 0; break;
            case 1: dd = 1; break;
            case 2: dd = 2; break;
            case 3: dd = 3; break;
            case 4: dd = 4; break;
            case 5: dd = 5; break;
            case 6: dd = 6; break;
            case 7: dd = 7; break;
            case 8: dd = 8; break;
            case 9: dd = 9; break;
            case 10:
            case 11:
                if (func_80009C74(D_80017E78, &D_80017F10[n], n) == 0) {
                    func_80009B30(&D_80017F10[n], 0);
                } else if (func_80009930(D_80017E78, D_80017C40, n) != 0) {
                    bVar2 = 1;
                }
                break;
            }
        }
    }
    if (bVar1 != 0 || bVar2 != 0) {
        return 1;
    }
    return 0;
}
