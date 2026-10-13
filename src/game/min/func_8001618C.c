#include "ctx.h"

typedef struct {
    u32 flags;          /* 0x00 */
    u8 pad04[0x38];
    s32 f3C;            /* 0x3C */
    s32 f40;            /* 0x40 */
    s32 f44;            /* 0x44 */
    u8 pad48[4];
    s32 f4C;            /* 0x4C */
    u8 pad50[0x75C - 0x50];
    s32 f75C;           /* 0x75C */
} Ctl;

typedef union {
    u32 w;
    struct {
        u32 x : 4;
        u32 y : 4;
        u32 rest : 24;
    } b;
} Pad;

typedef struct {
    u8 pad[0x10];
    s32 f10;
} S29CC;

typedef struct {
    u8 pad[0x64];
    s16 f64;
} S930;

extern Pad D_800E30B0;
extern u32 D_800E25B0;
extern Ctl D_800EFD70;
extern u32 D_800E36F8;
extern s32 D_800E374C;
extern s32 D_800E370C;
extern s32 D_800CA920;
extern S930 *D_800CA930;
extern S29CC *D_800C29CC;
extern u32 D_800C2350;

void func_8003F658(void);
void func_8002D8A0(void);
void func_8003A720(void);
void func_8006B160(void);
void func_8003C2AC(void);
void func_80069C6C(void);
void func_80072A9C(void);
void func_8004B6CC(void);
void func_8001683C(void);
void func_80015124(s32);
void func_800229B8(void);
void func_8005E960(void);
void func_8000F600(void);
void func_8003A84C(void);
void func_8005B33C(void);
void func_80014E10(void);
void func_80050838(s32, s32);
void func_80015E60(void);
void func_8005B05C(void);
void func_8001102C(s32);

void func_8001618C(void)
{
    s32 up, down, b10, b20, b40;
    u32 f;

    if ((D_800E30B0.w & 3) && ((D_800E30B0.w & 1) ? D_800E30B0.b.x : -D_800E30B0.b.x) >= 4) {
        D_800E30B0.w |= 0x800;
    } else if ((D_800E30B0.w & 3) && ((D_800E30B0.w & 1) ? D_800E30B0.b.x : -D_800E30B0.b.x) < -3) {
        D_800E30B0.w |= 0x1000;
    }
    if ((D_800E30B0.w & 0xC) && ((D_800E30B0.w & 8) ? ((s32)D_800E30B0.w >> 24 & 0xF) : -((s32)D_800E30B0.w >> 24 & 0xF)) >= 4) {
        D_800E30B0.w |= 0x4000;
    } else if ((D_800E30B0.w & 0xC) && ((D_800E30B0.w & 8) ? ((s32)D_800E30B0.w >> 24 & 0xF) : -((s32)D_800E30B0.w >> 24 & 0xF)) < -3) {
        D_800E30B0.w |= 0x2000;
    }
    f = D_800E30B0.w;
    up = (f & 0x2000) && !(D_800E25B0 & 0x2000);
    down = (f & 0x4000) && !(D_800E25B0 & 0x4000);
    b10 = (f & 0x10) && !(D_800E25B0 & 0x10);
    b20 = (f & 0x20) && !(D_800E25B0 & 0x20);
    b40 = (f & 0x40) && !(D_800E25B0 & 0x40);

    if (D_800EFD70.f4C == 0 && D_800EFD70.f3C >= 2) {
        if (up && D_800EFD70.f40 >= 2) D_800EFD70.f40--;
        if (down && D_800EFD70.f40 < 6) D_800EFD70.f40++;
    }

    switch (D_800E36F8) {
    case 0:
        if (D_800E374C != -1) {
            if (--D_800E374C == 0) D_800E36F8 = 1;
        }
        break;
    case 2:
        if (D_800E374C == -3) {
            if (D_800CA920 == 3) {
                func_8001683C();
                D_800E36F8 = 7;
                D_800E374C = 100;
                D_800C29CC->f10 = 3;
            } else {
                D_800E36F8 = 3;
                D_800E374C = 2;
            }
        } else if (D_800E374C > 0) {
            if (--D_800E374C == 0) D_800E374C = -2;
        }
        break;
    case 3:
        if (D_800E374C == 0) {
            D_800E36F8 = 4;
            D_800E374C = -1;
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
            if (D_800EFD70.f4C == 9) func_8004B6CC();
            if (D_800C2350 & 0x800) func_8005B33C();
            func_8001683C();
            D_800EFD70.flags &= ~0x80;
            D_800E370C = D_800EFD70.f3C + D_800EFD70.f44;
            if (D_800EFD70.f4C == 8 && !(D_800EFD70.flags & 1)) {
                D_800E370C = 1;
            } else if (D_800EFD70.f4C == 0 && D_800EFD70.f3C == 2) {
                D_800E370C = D_800EFD70.f75C + 2;
            }
            func_80014E10();
            func_80050838(D_800E370C, D_800EFD70.f40);
            func_80015E60();
            if (D_800EFD70.f4C == 9) func_8005B05C();
        }
        break;
    case 1:
        if (b20 && (D_800EFD70.f4C != 1 || D_800CA930->f64 == 0)) {
            func_8001102C(2);
            D_800E36F8 = 2;
            D_800E374C = 20;
            D_800CA920 = 3;
            D_800EFD70.flags &= ~4;
        } else if (b10 || b40) {
            func_8001102C(1);
            D_800CA920 = 6;
            D_800E36F8 = 2;
            D_800E374C = 20;
        }
        break;
    }
}
