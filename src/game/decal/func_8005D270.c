#include "ctx.h"

typedef struct {
    u8 pad[8];
    s16 a;
    s16 b;
    u8 pad2[4];
} E16;

void *func_800BC908(s32 size, u32 flags, s32 owner, s32 tag);
void func_8000ED10(u8 *a0, u8 a1, s32 a2);

extern char D_800D8B90[];
extern u8 *D_800F0774;
extern E16 *D_800F07F8;
extern s32 D_800F0770;
extern u8 *D_800F0820;
extern u8 D_800F0798[];
extern u8 D_800F07D8[];
extern u8 D_800F0778[];
extern u8 D_800F0828[];
extern u8 D_800F07B8[];
extern u8 D_800F0800[];

void func_8005D270(void)
{
    s32 i;

    D_800F0774 = func_800BC908(0xC00, 0, (s32)D_800D8B90, 0x67);
    D_800F07F8 = func_800BC908(0x2000, 0, (s32)D_800D8B90, 0x68);
    D_800F0770 = 0;
    func_8000ED10(D_800F0798, 0, 0x20);
    func_8000ED10(D_800F07D8, 0, 0x20);
    func_8000ED10(D_800F0778, 0, 0x20);
    func_8000ED10(D_800F0828, 0, 0x20);
    func_8000ED10(D_800F0774, 0, 0xC00);
    func_8000ED10(D_800F07B8, 0, 0x20);
    func_8000ED10(D_800F0800, 0, 0x20);
    for (i = 0; i < 0x80; i++) {
        D_800F0774[i * 0x18] = 0;
    }
    for (i = 0; i < 0x200; i += 4) {
        D_800F07F8[i].a = 0;
        D_800F07F8[i].b = 0;
        D_800F07F8[i + 1].a = 0x200;
        D_800F07F8[i + 1].b = 0;
        D_800F07F8[i + 2].a = 0;
        D_800F07F8[i + 2].b = 0;
        D_800F07F8[i + 3].a = 0x200;
        D_800F07F8[i + 3].b = 0;
    }
    D_800F0820 = func_800BC908(8, 0, (s32)D_800D8B90, 0x85);
    D_800F0820[0] = 0xFF;
    D_800F0820[1] = 0xDC;
    D_800F0820[2] = 0x88;
    D_800F0820[3] = 0x58;
    D_800F0820[4] = 0x38;
    D_800F0820[5] = 0x58;
    D_800F0820[6] = 0x8C;
    D_800F0820[7] = 0x1F;
}
