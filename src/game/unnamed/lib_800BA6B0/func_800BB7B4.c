#include "ctx.h"

extern s32 D_800DB8C0;
extern s32 D_800DB8C4;
extern s32 D_800DB8C8;
extern s32 D_800DB8D0;
extern s32 D_800DB8D4;
extern s32 D_800DB8D8;
extern u8 D_800DB8DC;
extern s32 D_800FE8DC;
extern void func_800DBD00(void);
extern void func_800DBCA4(void);

void func_800BB7B4(s32 a, s32 b, s32 c) {
    s32 flags;

    D_800DB8C4 = a;
    D_800DB8C8 = b;
    flags = D_800FE8DC;
    D_800DB8C0 = c;
    if (flags & 1) {
        D_800DB8C0 = c << 1;
    }
    if (flags & 2) {
        D_800DB8D0 = 0;
        D_800DB8DC = 0;
        D_800DB8D4 = (s32)0xFFFF8001;
        D_800DB8D8 = 0x7FFF;
        func_800DBD00();
    } else {
        D_800DB8DC = 0;
        D_800DB8D0 = 0;
        D_800DB8D4 = 0;
        D_800DB8D8 = 0xFF;
        func_800DBCA4();
    }
}
