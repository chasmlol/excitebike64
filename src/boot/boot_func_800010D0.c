#include "ctx.h"

typedef struct { u8 b[0x30]; } Blk;
typedef struct { s32 a; s32 b; } Seg;
typedef struct { s32 w0; s32 w1; Seg seg[4]; u32 rom; u32 sum; } Hdr;

void func_80000704(void *a, u32 b, u32 c);
void func_80001A24(void);
void func_80001A6C(void);
void func_80008EF0(s32 a);
void func_800093B0(void *a);
void func_800098D0(s32 a);
void func_80009400(s32 a);
void func_80006CE0(s32 a, s32 b);
void func_800113E4(void);
u32 func_80009570(u32 a);
void func_800074B0(void *a, void *b, s32 c);
void func_80009350(void *a, s32 b, s32 c);
void func_80008030(void *a, void *b, s32 c);
void func_80000C80(void);
u32 func_80000A30(u32 a, u32 b, u32 c, u32 d, u32 e);
u32 func_80000D28(u32 a);
void func_80006BB0(s32 a, s32 b);
void func_80001660(u32 a, u32 b, u32 c);
void func_80005C70(void);

extern s32 D_80000300;
extern u32 D_800180FC;
extern u32 D_800180F8;
extern u32 D_800180F0;
extern u32 D_80017CA8;
extern u32 D_80017CAC;
extern u32 D_80017E90;
extern u32 D_80017C04;
extern u32 D_800113B4;
extern u32 D_800113B8;
extern u32 D_800113BC;
extern u32 D_800113C0;
extern void (*D_80017EB8)(void);
extern u8 D_8000DE10[];
extern u8 D_8000F498[];
extern u8 D_8000FF30[];
extern u8 D_800107F0[];
extern u8 D_80017E98[];
extern u8 D_80017C28[];
extern u8 D_80011584[];
extern u8 D_800115F8[];
extern u8 D_800115E4[];
extern u8 D_800115E8[];
extern u8 D_800115EC[];
extern u8 D_800115F0[];
extern u8 D_B0012200[];
extern u8 D_80100000[];
extern u8 D_80100030[];

void boot_func_800010D0(void)
{
    Hdr hdr;
    s32 loc[2];
    s32 n;
    s32 off;
    s32 *q;
    s32 *base;
    u8 *buf;
    s32 y;
    s32 j;
    s32 k, d, m;
    s32 *pa;
    s32 *pb;
    s32 *ps;
    s32 *pd;
    u8 *src;
    u16 *pal;
    u16 *dst;

    u32 s1;
    u32 ret;
    u32 r;
    u32 t4;
    u32 fp;

    D_80017CA8 = 0x80200000;
    D_800180FC = 0x80300000;
    func_80000704((void *)0x80200000, 0, 0x28000);
    func_80001A24();
    func_80001A6C();
    func_80008EF0(10);
    if (D_80000300 == 1) {
        func_800093B0(D_8000FF30);
    } else if (D_80000300 == 2) {
        func_800093B0(D_800107F0);
    }
    func_800098D0(1);
    if (D_80000300 == 0) {
        for (;;) ;
    }
    func_80009400(0xAA);
    y = 0;
    func_80001A6C();
    D_800113B4 = (u32)D_8000DE10;
    D_800113B8 = (u32)D_8000DE10 + 0x1688;
    D_800113BC = D_800180FC;
    D_800113C0 = D_800180FC + 0x25800;
    func_800113E4();
    func_80001A6C();
    src = (u8 *)(D_800180FC + 0x28);
    pal = (u16 *)((u32)src + 0x12C00);
    dst = (u16 *)D_80017CA8;
    for (; y < 0xF0; y++) {
        j = 0;
        d = (0xEF - y) * 0x140;
        for (; j < 0x140; j++) {
            dst[d + j] = pal[src[y * 0x140 + j]];
        }
    }
    func_80006CE0(0, 0x800000);
    func_80001A6C();
    func_800098D0(0);
    func_80009570(D_80017CA8);
    y = 0;
    func_800098D0(y);
    func_80009570(D_80017CA8);
    func_800074B0(D_80017C28, D_80017E98, 8);
    func_80009350(D_80017C28, y, 1);
    func_80001A6C();
    do {
        func_80008030(D_80017C28, loc, 1);
        func_80001A6C();
        y++;
    } while (y < 0x78);
    t4 = (u32)D_B0012200;
    func_80000C80();
    buf = (u8 *)0x80100000;
    y = func_80000A30(D_80017C04, 0, t4, (u32)buf, 0xAF000);
    do {
    } while (func_80000D28(y) != 3);
    func_80006BB0(0, 0x400000);
    func_80001A6C();
    *(Blk *)&hdr = *(Blk *)buf;
    D_80017E90 = 0x80300000;
    D_80017CAC = 0;
    D_800180F8 = (u32)(buf + 0x28);
    base = (s32 *)&hdr;
    for (y = 0; y < 4; y++) {
        q = (s32 *)((u8 *)base + (y * 8 + 4));
        pa = base + y * 2;
        D_800113B4 = D_800180F8;
        D_800113B8 = D_800180F8 + q[2];
        D_800113BC = D_80017E90;
        D_800113C0 = D_80017E90 + pa[2];
        func_800113E4();
        D_800180F8 += q[2];
        D_80017E90 += pa[2];
        D_80017CAC += pa[2];
    }
    func_80001A6C();
    func_80001660((u32)D_80011584, (u32)D_800115F8 - (u32)D_80011584, 0x802F0000);
    n = hdr.seg[0].b + 0x28;
    hdr.rom = t4;
    hdr.sum = t4 + n + hdr.seg[1].b + hdr.seg[2].b + hdr.seg[3].b;
    *(Blk *)0x803FFFC0 = *(Blk *)&hdr;
    *(u32 *)((u32)D_800115E4 + D_800180F0) = 0x80300008;
    *(u32 *)((u32)D_800115EC + D_800180F0) = 0x80000408;
    *(u32 *)((u32)D_800115E8 + D_800180F0) = D_80017CAC;
    *(u32 *)((u32)D_800115F0 + D_800180F0) = 0x80000400;
    func_80001A6C();
    D_80017EB8 = (void (*)(void))0x802F0000;
    func_80005C70();
    D_80017EB8();
}
