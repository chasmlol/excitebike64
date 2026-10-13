#include "ctx.h"

__attribute__((noreturn)) void func_8000ECB8(s32 a0);
void func_800BCB04(void *ptr);
void *func_800BCCF0(void *ptr, s32 size, char *tag, s32 id);
void func_800AD990(void);
s32 func_800BEC20(s32 a0, s32 a1, s32 a2, s32 a3, s32 a4);
s32 func_800BEF2C(s32 a0);
void func_800BEF8C(s32 a0);
s32 func_80065858(void);
s32 func_800BDE38(void);
void func_800AD7E0(s32 a0, s32 a1);
void func_800AD890(s32 a0, s32 a1);
void *memcpy(void *, const void *, u32);

typedef struct { u8 b[48]; } Blk48;
typedef struct { u8 b[16]; } Blk16;

typedef struct {
    u8 pad[0x50];
    s32 f50;
} Lvl;

typedef struct {
    s32 f0;
    s32 f4;
    s32 f8;
    s32 fC;
    s32 f10;
    s32 f14;
    s32 f18;
    s32 f1C;
    s32 f20;
    s32 f24;
    s32 f28;
} OvlTab;

extern s32 D_800C8D1C;
extern s32 D_800EFD70;
extern Lvl *D_800EFD64;
extern u8 D_800731D0[];
extern u8 D_800A42C0[];
extern u8 D_800A9C20[];
extern u8 D_80073200[];
extern u8 D_800A4290[];
extern u8 D_800A42D0[];
extern u8 D_800A9C10[];
extern char D_800D6170[];
extern s32 D_801009A4;
extern s32 D_800C2950;
extern s32 D_800C29E0;
extern OvlTab D_800E20A8;
extern s32 D_80100654;
extern void *D_80100658;
extern s32 D_8010065C;
extern void *D_80100660;

void func_800197B4(s32 x)
{
    Blk48 buf;
    s32 h;
    s32 cart;
    s32 sz;
    s32 q;
    u8 *p3;
    u8 *p6;
    u8 *p7;

    if (x == D_800C8D1C || (D_800EFD70 & 1)) {
        return;
    }
    p3 = D_800731D0;
    p6 = D_800A42C0;
    p7 = D_800A9C20;
    if ((u32)p3 & 0xF) func_8000ECB8(0x33);
    if ((u32)p6 & 0xF) func_8000ECB8(0x34);
    if ((u32)p7 & 0xF) func_8000ECB8(0x35);
    if (x != 0) {
        sz = (u32)&D_800A42C0[-0x30] - (u32)p3;
        func_800BCCF0(p3 + 0x30, sz, D_800D6170, 0xBDD);
        if (!(D_800EFD64->f50 & 2)) {
            func_800BCCF0(p6 + 0x10, (u32)&D_800A9C20[-0x10] - (u32)p6, D_800D6170, 0xBE0);
        }
        func_800AD990();
        if (D_800C2950 == 0) {
            cart = 0xB0000C00;
            while ((h = func_800BEC20(D_801009A4, 0, (s32)p3 + cart + 0x30, (s32)(p3 + 0x30), sz)) < 0);
            while (func_800BEF2C(h) != 3);
            func_800BEF8C(h);
            while ((h = func_800BEC20(D_801009A4, 0, cart + (s32)p6 + 0x10, (s32)(p6 + 0x10), (q = (s32)p7 - 0x10) - (s32)p6)) < 0);
            while (func_800BEF2C(h) != 3);
            func_800BEF8C(h);
        } else {
            while (func_80065858() != 0);
            cart = D_800E20A8.f28 + 0x28;
            cart += D_800E20A8.fC;
            while ((h = func_800BEC20(D_801009A4, 0, cart, D_800C29E0, D_800E20A8.f14)) < 0);
            while (func_800BEF2C(h) != 3);
            func_800BEF8C(h);
            D_80100660 = p3;
            D_80100658 = p6;
            D_8010065C = D_800C29E0;
            D_80100654 = D_800C29E0 + D_800E20A8.fC;
            buf = *(Blk48 *)p3;
            func_800BDE38();
            *(Blk48 *)p3 = buf;
            cart += D_800E20A8.f14;
            while ((h = func_800BEC20(D_801009A4, 0, cart, D_800C29E0, D_800E20A8.f1C)) < 0);
            while (func_800BEF2C(h) != 3);
            func_800BEF8C(h);
            D_80100660 = p6;
            D_80100658 = p7;
            __builtin_memcpy(&buf, p6, 16);
            D_8010065C = D_800C29E0;
            D_80100654 = D_800C29E0 + D_800E20A8.fC;
            func_800BDE38();
            *(Blk16 *)p6 = *(Blk16 *)&buf;
        }
        func_800AD7E0(0, 0x800000);
        func_800AD890(0, 0x800000);
        D_800C8D1C = 1;
    } else {
        func_800BCB04(p3 + 0x30);
        if (!(D_800EFD64->f50 & 2)) {
            func_800BCB04(p6 + 0x10);
        }
        D_800C8D1C = 0;
    }
}
