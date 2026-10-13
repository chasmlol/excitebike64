#include "ctx.h"

typedef struct {
    s32 a;
    s32 arg;
    s32 pad[4];
} Ent18;

typedef struct {
    s32 key;
    s32 arg;
} Ent8;

typedef struct {
    u8 pad[0x3C];
    s32 arg;
} Ent40;

extern s32 D_800C2B64;
extern s32 D_800C2B44;
extern s32 D_800C2AD0;
extern Ent8 D_800C2C10[];
extern Ent18 D_800C2B68[];
extern u8 D_800E32A8[];
extern Ent40 *D_800E35CC;
extern s32 D_800D5638;

void func_800B99B0(s32 a0);
void func_80011098(s32 *p);
void func_800BC370(void);
void func_800B8F2C(void);
void func_8004F534(void);
void func_800B78F0(s32 param_1);
void func_800B98B4(void);
void func_800BBDE0(void);
void func_8004C3C4(void);
void func_800B7BE0(void);

void func_8000F600(void)
{
    s32 i;
    u32 j;
    u8 *p;
    Ent8 *q;
    s32 a;

    if (D_800C2B64 != 0) {
        D_800C2B64 = 0;
        if (D_800C2B44 != 0) {
            func_8004C3C4();
        }
        func_800B99B0(0);
        for (i = 0; i < 8; i++) {
            p = D_800E32A8 + i * 0x64;
            if (D_800C2B64 != 0) {
                func_80011098((s32 *)(p + 0xC));
                func_80011098((s32 *)(p + 0x4));
                func_80011098((s32 *)(p + 0x44));
                func_80011098((s32 *)(p + 0x50));
                func_80011098((s32 *)(p + 0x4C));
                func_80011098((s32 *)(p + 0x48));
            }
        }
        func_8004F534();
        func_800B8F2C();
        func_800B7BE0();
        func_800B98B4();
        if (D_800C2AD0 != 0) {
            for (j = 0; j < 7; j++) {
                func_800B78F0(D_800C2B68[j].arg);
            }
            D_800C2AD0 = 0;
        }
        if (D_800C2C10[0].key != 0) {
            i = 0;
            do {
                if (D_800C2C10[i].arg != -1) {
                    func_800B78F0(D_800C2C10[i].arg);
                }
                i++;
            } while (D_800C2C10[i].key != 0);
        }
        for (i = 0; i < 16; i++) {
            a = D_800E35CC[i].arg;
            if (a != -1) {
                func_800B78F0(a);
            }
        }
        func_800BC370();
        while (D_800D5638 != 0) {
            func_800BBDE0();
        }
    }
}
