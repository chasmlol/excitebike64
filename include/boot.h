/* What every source file of the boot loader includes. */
#ifndef BOOT_H
#define BOOT_H

#include "common.h"

/* src/boot/boot_80000450.c */
void boot_func_80000450(void);
void boot_func_800004A8(s32 param_1);

/* src/boot/boot_80000824.c */
void boot_func_80000824(s32 n, char *s);

/* src/boot/boot_8000095C.c */
void boot_func_8000095C(u8 *param_1, u32 param_2);
void boot_func_800009C4(u8 *p);

/* src/boot/boot_80000B30.c */
void boot_func_80000B30(void);
s32 boot_func_80000C80(void);

/* src/boot/boot_80000E40.c */
void boot_func_80000E40(s32 c, s32 x, s32 y, u16 *fb, s32 color);
void boot_func_80000EEC(u8 *s, s32 x, s32 y, u16 *fb, s32 color);
void boot_func_80000FC8(s32 x0, s32 x1, s32 y0);
void boot_func_800010D0(void);
void boot_func_80001660(u32 *from, u32 size, u32 *dst);
s32 boot_func_8000182C(void);
void boot_func_80001A24(void);
void boot_func_80001A6C(void);

/* src/boot/boot_80001AC4.c */
void boot_func_80001AC4(u16 *dst, u16 *src, s32 k);

/* src/boot/boot_80001B50.c */
u32 boot_func_80001B50(void);
void boot_func_80001B88(u32 a0);
u32 boot_func_80001B88_2(u32);
void boot_func_80001BB8(s32 *p, s32 v);
u32 boot_func_80001BF0(u32 *param_1);
u32 boot_func_80001C74(u32 addr);
u32 boot_func_80001CB4(u32 a);
void boot_func_80001CF4(u32 src, u8 *dst, u32 n);
void boot_func_80001E0C(u32 src, u32 dst, u32 cnt);
void boot_func_80001F20(u32 param_1, u8 param_2);
void boot_func_80001F98(u32 addr, u8 val);
void boot_func_800020FC(u8 *p0, u32 c0, u32 n0);
void boot_func_800021C8(u8 *dst, u8 *src, s32 n);
void boot_func_800023EC(void);
s32 boot_func_80002808(char *s);
void boot_func_80002828(s32 p1, s32 p2, s32 p3, s32 p4);
u32 boot_func_80002828_2(u32, u32, u32, u32);
s32 boot_func_80002828_3(s32 arg0, s32 arg1, s32 arg2, s32 arg3);
u32 boot_func_80002828_4(s32 a, s32 b, u32 c, s32 d);
s32 boot_func_80002828_5(s32 a0, s32 a1, u32 a2, s32 a3);
s32 boot_func_8000292C(u8 *arg0, s32 arg1);
s32 boot_func_800029BC(u8 *arg0, s32 arg1);
s32 boot_func_80002AB4(s32 fd, u8 *buf, u32 size);
s32 boot_func_80002B78(s32 fd, u8 *buf, u32 size);
u32 boot_func_80002C40(u32 p1, u32 p2, u32 p3);

/* src/boot/boot_80001C2C.c */
void boot_func_80001C2C(s32 param_1);
void boot_func_80001DD0(u32 addr, u32 val);
void boot_func_80002010(u8 *d, u8 *s, u32 n);
void boot_func_8000223C(u32 p);
void boot_func_800024EC(void);
void boot_func_800025B8(void);
u32 boot_func_800028E0(void);
u32 boot_func_80002A4C(u32 p);

/* the boot loader's copies of game functions (built from the game's C) */
void boot_func_80000510(s32 a, ...);
u16 boot_func_80000528(u32 r);
s32 boot_func_800006D4(s32 a0);
s32 boot_func_800006EC(s32 a0);
void boot_func_80000704(u8 *a0, u8 a1, s32 a2);
void boot_func_80000724(char *dst, char *src);
void boot_func_80000764(u8 *d, u8 *s, s32 n);
s32 boot_func_80000794(u8 *param_1, u8 *param_2);
void boot_func_80000814(void);
void boot_func_8000081C(void);
void boot_func_800008F0(u8 *s);
void boot_func_8000092C(char *a0, char *a1);
s32 boot_func_80000A30(s32 a0, s32 a1, s32 a2, s32 a3, s32 a4);
s32 boot_func_80000D28(s32 a0);
void boot_func_80000D88(s32 a0);
void boot_func_80000DB0(s32 x);

/* libultra, by address (libultra_map.json) */
void func_80005C70(void);  /* __osDisableInt */
void func_80005D00(s32 a, void *b, void *c, s32 d);  /* osCreatePiManager */
s32 boot_func_80006150(s32 a, void *b, s32 c);  /* osEPiStartDma */
s32 func_80006210(void);  /* osCartRomInit */
void func_80006BB0(s32 a, s32 b);  /* osInvalDCache */
void func_80006C60(u32 a, u32 b);  /* osInvalICache */
void func_80006CE0(s32 a, s32 b);  /* osWritebackDCache */
void boot_func_80006D60(void);  /* osWritebackDCacheAll */
void func_80006D90();  /* osContStartQuery */
void func_80006E10();  /* osContGetQuery */
void func_80006E30();  /* osContStartReadData */
void func_80006EB8();  /* osContGetReadData */
s32 func_80007010();  /* osContInit */
u32 boot_func_80007460(char *s);  /* strlen */
void func_800074B0(void *a, void *b, s32 c);  /* osCreateMesgQueue */
s32 boot_func_80008030(void *a, s32 b, s32 c);  /* osRecvMesg */
int func_80008030();  /* osRecvMesg */
void func_80008290();  /* osSetEventMesg */
void boot_func_800084E0(void *a, s32 b, void *c, s32 d, void *e, s32 f);  /* osCreateThread */
void func_800084E0(OSThread *t, OSId id, void (*entry)(void *), void *arg, void *sp, OSPri pri);  /* osCreateThread */
void boot_func_800086B0(void *a, s32 b);  /* osSetThreadPri */
void boot_func_80008780(void *a);  /* osStartThread */
void func_80008780(OSThread *t);  /* osStartThread */
void func_80008EF0(s32 a);  /* osCreateViManager */
void func_80009350(void *a, s32 b, s32 c);  /* osViSetEvent */
void func_800093B0(void *a);  /* osViSetMode */
void func_80009400(s32 a);  /* osViSetSpecialFeatures */
u32 func_80009570(u32 a);  /* osViSwapBuffer */
void func_800098D0(s32 a);  /* osViBlack */
s32 func_80009930();  /* osGbpakInit */
void func_80009B30();  /* __osMotorAccess */
s32 func_80009C74();  /* osMotorInit */
u32 func_80009FB0();  /* osPfsInitPak */
void func_8000C2D8(void);  /* __osInitialize_common */
void func_8000C5B0(void);  /* __osInitialize_autodetect */

/* other code */
void func_80002D20(void);
u32 boot_func_80002E04(void);
u32 func_80002E04(void);
void boot_func_80002E10(u32 v);
void func_80002E10(u32 a);
void func_80008030_2(void *a, void *b, s32 c);
void func_80008030_3();
void func_80008290_2(s32 a, void *b, s32 c);
void func_800113E4(void);

extern s32 D_80000300;
extern u32 D_80000318;
extern s32 D_8000CDF0;
extern void (*D_8000CE04)(s32, s32, s32);
extern u8 D_8000CE10[];
extern u8 D_8000DE10[];
extern u8 D_8000F498[];
extern u32 D_8000F4A0[];
extern u32 D_8000F4A4[];
extern u32 D_8000F4A8;
extern u32 D_8000F4B8;
extern u8 D_8000FF30[];
extern u8 D_800107F0[];
extern char D_80011240[];
extern u32 D_800113B4;
extern u32 D_800113B8;
extern u32 D_800113BC;
extern u32 D_800113C0;
extern u8 D_80011584[];
extern u8 D_800115E4[];
extern u8 D_800115E8[];
extern u8 D_800115EC[];
extern u8 D_800115F0[];
extern u8 D_800115F8[];
extern OSThread D_80011600;
extern u8 D_800137B0[];
extern s32 D_80015960;
extern s32 D_80017964;
extern s32 D_80017968[];
extern s32 D_80017984[];
extern s32 D_80017BE0[];
extern s32 D_80017C00;
extern s32 D_80017C04;
extern u8 D_80017C28[];
extern u8 D_80017C40[];
extern u32 D_80017CA8;
extern u32 D_80017CAC;
extern u8 D_80017E60[];
extern u8 D_80017E78[];
extern u32 D_80017E90;
extern u8 D_80017E94[];
extern u8 D_80017E98[];
extern void (*D_80017EB8)(void);
extern u8 D_800180B0[];
extern u8 D_800180D0[];
extern u32 D_800180F0;
extern u32 D_800180F8;
extern u32 D_800180FC;
extern volatile s32 D_80018100;
extern u32 D_80018288;
extern u8 D_80018430;
extern u8 D_80018432;
extern u8 D_80100000[];
extern u8 D_80100030[];
extern u8 D_8F018110[];
extern u8 D_B0012200[];

#endif
