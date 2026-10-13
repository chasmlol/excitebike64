#include "ctx.h"

typedef struct {
    s32 unk0;
    s32 *unk4;
} Unk7728;

typedef struct {
    u8 pad[8];
    s16 a[5];
} Row;

extern void **D_800C2354;
extern Unk7728 *D_800E7728;
extern s32 D_800D088C;
extern s32 D_800D09B0;
extern s32 D_800D085C;
extern s32 D_800EFDBC;
extern s32 *D_800C8D68[];
extern Row **D_800CA930;
extern s32 D_800CA920;
extern u32 D_800EFD70;

void func_800C204C(void *arg0);
void func_800BCB04(void *arg0);
void func_800BFEA4(s32 *arg0);
void func_80048230(Unk7728 *arg0);

void func_8001683C(void) {
    s32 i;
    s32 off;

    for (i = 0; i < 0x4A; i++) {
        off = i * 4;
        func_800C204C(*(void **)((s32)D_800C2354 + off));
        func_800BCB04(*(void **)((s32)D_800C2354 + off));
    }
    func_800BFEA4(&D_800E7728->unk4[D_800D088C]);
    func_800BFEA4(&D_800E7728->unk4[D_800D09B0]);
    func_800BFEA4(&D_800E7728->unk4[D_800D085C]);
    if (D_800EFDBC == 1) {
        for (i = 0; i < 5; i++) {
            func_800BFEA4(&D_800E7728->unk4[*D_800C8D68[(*D_800CA930)->a[i]]]);
        }
    }
    func_800BCB04(D_800C2354);
    D_800C2354 = NULL;
    if (D_800CA920 != 3) {
        D_800EFD70 &= ~0x80;
    }
    func_80048230(D_800E7728);
}
