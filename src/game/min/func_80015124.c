#include "ctx.h"

typedef struct { u8 pad[0x64]; s16 idx; } PtrA;
typedef struct { u8 pad[0x1C]; s32 *ptr; u8 pad2[8]; } Ent;

extern u32 D_800EFD70[];
extern s32 D_800CA920;
extern s32 D_800C8CD8;
extern s32 D_800C8CDC;
extern s32 D_800C8CE4;
extern s32 D_800C8CEC;
extern s32 *D_800E7728;
extern PtrA *D_800CA930;
extern Ent D_800CADE4[];

void func_800BFEA4(u32 *p);
void func_8005F188(void);
void func_800197B4(s32 x);

void func_80015124(s32 param_1)
{
    u32 *g = (u32 *)D_800EFD70;
    if ((g[0] & 0x85) != 0) {
        if ((g[0] & 0x80) == 0) {
            return;
        }
        if (D_800CA920 != 3) {
            return;
        }
    }
    if (D_800C8CD8 == 0) {
        if (D_800C8CDC != 0) {
            D_800C8CE4 = 0;
            func_800BFEA4((u32 *)((s32 *)D_800E7728[1] + D_800CADE4[D_800CA930->idx].ptr[0]));
            goto T;
        }
        if (D_800CA920 == 3 || g[19] == 9) goto T;
        if (g[19] < 10) goto F0;
        if (g[19] >= 15) goto CALL;
        if (g[19] < 13) goto CALL;
        goto T;
F0:
        if (g[19] == 2) goto T;
    }
CALL:
    func_8005F188();
T:
    if (D_800C8CEC == 0 && param_1 != 0) {
        func_800197B4(1);
    }
}
