#include "ctx.h"

typedef struct {
    s32 f00;
    u8 p04[0x22];
    s16 f26;
    u8 p28[4];
    s32 f2c;
    u8 p30[0x28];
    u32 f58;
    u8 p5c[0x14];
    s32 f70;
    s32 f74;
    s32 f78;
    void *f7c;
    s32 f80;
} Blk;

typedef struct {
    u8 p0[0xC];
    u8 fl;
    u8 p1[7];
    s32 w14;
    u8 p2[0x28 - 0x18];
} Ent;

extern Blk D_800F05A8;
extern s32 D_800F0180;
extern s32 D_800F0738;
extern u8 D_800F06AC;
extern u8 D_800F0746;
extern s32 D_800C8CE4;
extern u8 *D_800CA930;
extern Ent D_800CADE4[];
extern u8 D_800D7340[];

void func_80064DB8(void);
void *func_800BC908(s32 size, u32 flags, s32 owner, s32 tag);

#define CUR (D_800CADE4[*(s16 *)(D_800CA930 + 0x64)])

void func_8004F9B0(s32 a0)
{
    Blk *s;

    func_80064DB8();
    s = &D_800F05A8;
    if (s->f7c == 0) {
        s->f7c = func_800BC908(0x80, 0, (s32)D_800D7340, 0x16D);
    }
    D_800F0180 = 1;
    if (CUR.fl & 4) {
        s->f2c = 0;
        s->f58 |= 0x1000;
    } else {
        s->f58 &= ~0x1000;
    }
    if (CUR.fl & 8) {
        D_800F05A8.f26 = 0x3F;
        D_800F05A8.f00 = 1;
        D_800F05A8.f58 |= 0x10;
    } else {
        D_800F05A8.f58 &= ~0x10;
    }
    D_800F05A8.f58 &= ~0x4000;
    if (CUR.fl & 8) {
        D_800F05A8.f58 |= 0x18800;
        D_800F05A8.f80 = 600;
        if (CUR.fl & 2) {
            D_800F05A8.f2c = 0;
        } else if (D_800C8CE4 == 0) {
            D_800F05A8.f2c = CUR.w14;
        }
    } else {
        D_800F05A8.f00 = 0xE;
        D_800F05A8.f2c = 0;
    }
    if (D_800C8CE4 == 0) {
        D_800F05A8.f70 = 5;
        D_800F05A8.f58 &= ~0x40;
    } else {
        D_800F05A8.f26 = 0x3F;
        D_800F05A8.f70 = 0;
        D_800F05A8.f78 = 0x3C;
        D_800F05A8.f58 |= 0x40;
    }
    D_800F0738 = 0;
    D_800F06AC = 0;
    D_800F0746 = 0;
}
