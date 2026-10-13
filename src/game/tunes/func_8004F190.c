#include "ctx.h"

typedef struct {
    s32 p0;
    s32 p1;
    s32 w8;
    s32 wC;
    s32 rest[15];
} Ent4C;

extern s32 D_800CA860;
extern s32 *D_800D5604;
extern s32 D_800D1244;
extern s32 D_800EECC4;
extern s32 D_800EECCC;
extern Ent4C *D_800EFD24;
extern s32 D_800CA844;
void func_8004C3C4(void);
void func_800BFEA4(u32 *p);
void func_800BCB04(s32 p);

void func_8004F190(void)
{
    s32 i;
    if (D_800CA860 != 0) {
        func_8004C3C4();
    }
    func_800BFEA4((u32 *)((u8 *)D_800D5604[1] + D_800D1244 * 4));
    i = 0;
    func_800BCB04(D_800EECC4);
    if (0 < D_800EECCC) {
        do {
            func_800BCB04(D_800EFD24[i].w8);
            func_800BCB04(D_800EFD24[i].wC);
            i++;
        } while (i < D_800EECCC);
    }
    func_800BCB04(D_800EFD24);
    D_800CA844 = 0;
}
