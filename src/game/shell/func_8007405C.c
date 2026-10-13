#include "ctx.h"

typedef struct {
    u32 flags;
    u8 unk4[0xC - 0x4];
    s32 unkC;
    u8 unk10[0x1C - 0x10];
    s32 unk1C;
    u8 unk20[0x3C - 0x20];
    volatile s32 unk3C;
    u8 unk40[0x4C - 0x40];
    s32 mode;
    u8 unk50[0x410 - 0x50];
    s32 unk410;
} GameState;

typedef struct {
    u8 unk0[0x24];
} Cup;

typedef struct {
    Cup *cup;
    u8 unk4[0x64 - 0x4];
    s16 track;
    u8 unk66[0x74 - 0x66];
} Season;

typedef struct {
    u8 unk0[0xD];
    u8 id;
    u8 unkE[0x28 - 0xE];
} Track;

typedef struct {
    u8 unk0[0x2C1];
    u8 trackId;
} Unk1900;

extern GameState D_800EFD70;
extern Season D_800F0638;
extern Season *D_800CA930;
extern Cup D_800CA9C8[];
extern Track D_800CADE4[];
extern char *D_800EFD64;
extern char D_800CC250[];
extern volatile s32 D_800F0624;
extern Unk1900 D_800F1900;

void func_8000ED10(void *dst, s32 value, s32 size);

void func_8007405C(s32 arg0) {
    s32 i;

    if (arg0 != 0) {
        D_800EFD70.mode = 1;
        D_800CA930 = &D_800F0638;
        D_800EFD70.flags |= 0x80;
        func_8000ED10(&D_800F0638, 0, sizeof(Season));
        D_800CA930->cup = &D_800CA9C8[D_800EFD70.unkC];
        if (D_800EFD70.unk410 == 0) {
            D_800EFD64 = D_800CC250;
            D_800EFD70.mode = 9;
            D_800EFD70.unk3C = 1;
            D_800F0624 = 0;
            D_800EFD70.unk1C = 4;
            D_800EFD70.flags &= ~0x80;
            D_800CA930->cup = &D_800CA9C8[6];
            D_800CA930->track = 0;
            for (i = 0; i < 17; i++) {
                if (D_800CADE4[i].id == D_800F1900.trackId) {
                    D_800CA930->track = i;
                    return;
                }
            }
        }
    }
}
