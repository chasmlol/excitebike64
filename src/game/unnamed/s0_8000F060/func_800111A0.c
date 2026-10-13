#include "ctx.h"

typedef struct {
    s32 f00;
    s32 f04;
    s32 f08;
    s32 f0c;
    s32 f10;
    s32 f14;
} T18;

typedef struct {
    s32 f00;
    s32 f04;
    s32 f08;
    s32 f0c;
    s32 f10;
    s32 f14;
    s32 f18;
    s32 f1c;
    s32 f20;
    s32 f24;
    u8 pad[0x64 - 0x28];
} Ent;

extern s32 D_800C2B64;
extern T18 D_800C2B68[];
extern Ent D_800E32A8[];

u32 func_800BA330(s32 p);
void func_800B8E08(s32 a, s32 *out);
void func_800BA3F4(s32 p1, s32 p2);
void func_800B93E8(s32 p1, u32 p2);

void func_800111A0(void)
{
    s32 i;
    s32 loc[4];
    u32 r;

    if (D_800C2B64 != 0) {
        for (i = 0; i < 8; i++) {
            if (D_800E32A8[i].f0c != -1 && (u32)(D_800E32A8[i].f20 - 1) < 4 && D_800E32A8[i].f24 == 0) {
                r = func_800BA330(D_800E32A8[i].f0c);
                func_800B8E08(D_800C2B68[D_800E32A8[i].f20].f04, loc);
                if ((s32)r >= loc[1]) {
                    func_800BA3F4(D_800E32A8[i].f0c, D_800C2B68[6].f04);
                    func_800B93E8(D_800E32A8[i].f0c, r - loc[1]);
                    D_800E32A8[i].f24 = 1;
                }
            }
        }
    }
}
