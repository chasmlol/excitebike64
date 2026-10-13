#include "ctx.h"

typedef struct {
    s32 w[4];
} Blk;

typedef struct {
    s32 w[4];
} __attribute__((packed)) BlkU;

extern u8 *D_800CEA40;
extern s32 *D_800C9570;
extern s32 *D_800E7728;
extern s32 D_800CA920;
extern u8 *D_800C29E0;
extern s32 *D_800F0158;
typedef struct { u8 pad[0x3E8]; s32 *p; } FB;
extern FB D_800EFD70;
extern u8 D_800DAB1C[];

void func_800BFDD4(s32 *p);
void func_80014AD0(void);
void func_8008ED48(void);
void func_800BCB04(void *p);
void func_800A5964(s32 a);
void func_80087E00(void);
void func_80090484(void);
void func_800B2B00(s32 a);
void func_8004834C(void);
void func_800AEC34(void *dst, void *src, s32 n);
void *func_800BC908(s32 a, s32 b, void *c, s32 d);

void func_80091270(void)
{
    s32 iVar7;
    s32 n;

    if ((*(s32 *)(D_800CEA40 + 0x174) & 0x400) != 0) {
        iVar7 = *D_800C9570;
    } else {
        iVar7 = -1;
    }
    func_800BFDD4(D_800E7728);
    func_80014AD0();
    func_8008ED48();
    func_800BCB04((void *)*(s32 *)(D_800CEA40 + 0x174C));
    func_800BCB04((void *)*(s32 *)(D_800CEA40 + 0x1750));
    func_800BFDD4(*(s32 **)(D_800CEA40 + 0x1720));
    func_800BCB04((void *)*(s32 *)(D_800CEA40 + 0x171C));
    func_800BCB04((void *)*(s32 *)(D_800CEA40 + 0x1718));
    func_800A5964(1);
    if ((*(s32 *)(D_800CEA40 + 0x174) & 0x6000) == 0) {
        D_800CA920 = 6;
        func_800BCB04(D_800CEA40);
        D_800CEA40 = 0;
    }
    func_80087E00();
    func_80090484();
    func_800B2B00(0xAA);
    if (iVar7 >= 0) {
        __builtin_memcpy(D_800C29E0, D_800F0158, 0x400);
        n = (iVar7 - 1) * 0xc + 0x110;
        func_800AEC34(D_800C29E0 + 0x400, D_800C9570, n);
        func_800BCB04(D_800C9570);
        func_800BCB04(D_800EFD70.p);
        D_800C9570 = func_800BC908(n, 0, D_800DAB1C, 0x651);
        D_800EFD70.p = func_800BC908(0x400, 0, D_800DAB1C, 0x652);
        func_800AEC34(D_800C9570, D_800C29E0 + 0x400, n);
        __builtin_memcpy(D_800EFD70.p, D_800C29E0, 0x400);
    }
    func_8004834C();
}
