#include "ctx.h"

typedef struct { s32 f00; u8 pad04[0x38]; s32 f3C; u8 pad40[0xC]; s32 f4C; } Blk;
typedef struct { u8 pad[0x22]; s16 f22; } Blk2;
typedef struct { u8 pad[0x2C]; s32 f2C; } Obj2C;
typedef struct { u8 pad[0x64]; s16 f64; } Obj64;

extern Blk D_800EFD70;
extern Blk2 D_800F05A8;
extern Obj2C *D_800EFD64;
extern Obj64 *D_800CA930;
extern s32 D_800C8CD8;
extern s32 D_800C2594;
extern s32 D_800E1898;
extern s32 D_800E7728;
extern s32 D_800E3710;
extern s32 D_800CCCA4;
extern volatile s32 D_800CCCA8;
extern s32 D_800C9684;
extern u8 D_800C9080[];
extern s32 D_800C2350;
extern s32 D_800C8CE0;
extern s32 D_800E3700;
extern s32 D_800C2940;
extern u8 D_800F1BC4;
extern s32 D_800F0DA0;

extern s32 func_80065858(void);
extern void func_8000F164(void);
extern void func_8004A5FC(s32 a);
extern void func_80064DF4(s32 a, s32 b);
extern void func_80064D28(s32 a, s32 b);
extern void func_80000F0C(void);
extern void func_80019E78(void);
extern void func_80044358(void);
extern void func_80028600(s32 a);
extern void func_8001A190(void);
extern void func_80028864(void);
extern void func_8005D270(void);
extern void func_8003E1FC(s32 a);
extern void func_8003A694(void *p);
extern void func_8006AC40(void);
extern void func_8003C21C(void);
extern void func_80067B80(void);
extern void func_800282C8(void);
extern void func_80052C90(void);
extern void func_8000F240(void *p);
extern void func_8003A780(void);
extern void func_80071990(void);
extern void func_8004F47C(void);
extern void func_80035638(void);

void func_80015E60(void)
{
    while (func_80065858() != 0) { }
    func_8000F164();
    if (D_800C8CD8 != 0) {
        D_800C2594 = 1;
    } else {
        D_800C2594 = D_800EFD70.f3C;
    }
    D_800E1898 = D_800C2594;
    if (D_800EFD70.f4C == 9) {
        func_8004A5FC(D_800E7728);
    }
    if ((D_800EFD70.f00 & 0x8085) == 0 && (D_800EFD70.f4C != 1 || D_800CA930->f64 == 0)) {
        func_80064DF4(D_800EFD70.f3C, D_800F05A8.f22 - D_800EFD70.f3C);
        func_80064D28(D_800EFD70.f3C, D_800F05A8.f22 - D_800EFD70.f3C);
    }
    D_800E3710 = 0;
    if ((D_800EFD70.f00 & 0x81) == 0 && (D_800F1BC4 & 2) != 0 && D_800C2940 != 0 &&
        (D_800EFD70.f3C == 1 || D_800EFD70.f4C == 8) &&
        D_800EFD64->f2C != 2 && (D_800EFD70.f00 & 0x04040000) == 0) {
        D_800CCCA4 = 2;
    } else {
        D_800CCCA4 = 1;
    }
    while (D_800CCCA8 != D_800CCCA4) {
    }
    func_80000F0C();
    func_80019E78();
    if (D_800EFD64->f2C == 2) {
        func_80044358();
    }
    func_80028600(D_800E7728);
    func_8001A190();
    D_800C9684 = 0;
    if (0 < D_800C2594) {
        do {
            func_80028864();
            D_800C9684 = D_800C9684 + 1;
        } while (D_800C9684 < D_800C2594);
    }
    func_8005D270();
    func_8003E1FC(D_800E7728);
    func_8003A694(D_800C9080);
    func_8006AC40();
    func_8003C21C();
    func_80067B80();
    if (D_800EFD64->f2C == 1) {
        func_800282C8();
    }
    D_800C2350 = D_800C2350 | 0x800;
    func_80052C90();
    func_8000F240(D_800C9080);
    func_8003A780();
    func_80071990();
    func_8004F47C();
    D_800C8CE0 = 0;
    if (D_800EFD70.f4C == 8 && (D_800EFD70.f00 & 0x80) == 0 && D_800E3700 == 0) {
        D_800C8CE0 = 1;
    }
    func_80035638();
    if (D_800E1898 >= 2) {
        D_800F0DA0 = 2;
    } else {
        D_800F0DA0 = 1;
    }
}
