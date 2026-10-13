#include "ctx.h"

typedef struct {
    u32 flags;
    u8 pad04[0x4C - 0x4];
    s32 f4C;
    u8 pad50[0x17C - 0x50];
    s32 f17C;
    u8 pad180[0x2E8 - 0x180];
    s32 state;
    u8 pad2EC[0x3EC - 0x2EC];
    s32 f3EC;
    u8 pad3F0[0x3FC - 0x3F0];
    s32 f3FC;
    s32 sel;
    u8 pad404[0x418 - 0x404];
    s16 f418;
    u8 f41A[6];
    s32 f420[6];
    u8 pad438[0x488 - 0x438];
    s16 f488;
    u8 pad48A[0x764 - 0x48A];
    u8 f764[6];
    u8 f76A[6];
} Menu;

typedef struct {
    u8 pad[0x22];
    u16 f22;
    u8 pad24[0x50 - 0x24];
    u8 f50[6];
} Cfg;

typedef struct {
    u8 pad[0x669];
    u8 f669;
    u8 pad66A[0x68C - 0x66A];
    s32 f68C;
    u8 pad690[0x7E0 - 0x690];
} Ent;

typedef struct {
    u8 pad[0x50];
    u32 f50;
} Trk;

typedef struct {
    u8 pad[0x16];
    s16 f16;
} Obj;

extern Menu D_800EFD70;
extern Cfg D_800F05A8;
extern Ent D_800E37C0[];
extern u8 *D_800CA930;
extern Trk *D_800EFD64;
extern s32 D_800C9570;
extern s32 D_800C8CE4;
extern s32 D_800C8CE8;
extern s32 D_800F0954;
extern s32 D_800F0A3C;
extern s32 D_800F0950;
extern char D_800CD79C[];
extern char D_800CD7F4[];
extern char D_800CD8D0[];
extern char D_800CD694[];

void func_8001102C(s32 param_1);
void func_800BCB04(void *ptr);
void func_8007AF38(s32 p1, s32 p2, s32 p3);

void func_80074EF4(Obj *p1, s32 p2, u32 *p3)
{
    u32 fl;
    s32 orig, v, i;

    D_800EFD70.f17C++;
    if (D_800EFD70.f17C == 60) {
        D_800EFD70.f17C = 0;
    }
    D_800EFD70.f3EC++;
    if (D_800EFD70.f3EC == 120) {
        D_800EFD70.f3EC = 0;
    }
    fl = *p3;
    if (fl & 0x20) {
        if (D_800EFD70.state == 3) {
            D_800EFD70.state = 0;
        }
        return;
    }
    if (fl & 0x10) {
        func_8001102C(1);
        if (D_800EFD70.state == 3) {
            switch (D_800EFD70.sel) {
            case 0:
                if (D_800EFD70.f4C == 8) {
                    if (!(D_800EFD70.flags & 0x80000)) {
                        D_800C8CE4 = 0;
                    }
                    func_8007AF38(0, 0, 0);
                } else if (D_800EFD64->f50 & 2) {
                    func_8007AF38(0, 0, 0);
                    D_800C8CE4 = 0;
                } else {
                    D_800C8CE4 = 0;
                    func_8007AF38((s32)D_800CD7F4, 0, 1);
                }
                break;
            case 1:
                if (D_800EFD70.f4C == 8) {
                    D_800EFD70.f488 = 2;
                    func_8007AF38((s32)D_800CD8D0, 1, 1);
                } else {
                    D_800EFD70.f418 = D_800F05A8.f22;
                    for (i = 0; i < 6; i++) {
                        D_800EFD70.f41A[i] = D_800F05A8.f50[i];
                        D_800EFD70.f420[i] = D_800E37C0[i].f68C;
                        D_800EFD70.f76A[i] = D_800E37C0[i].f669;
                        if (D_800CA930 != 0) {
                            D_800EFD70.f764[i] = (D_800CA930 + i)[0x6C];
                        }
                    }
                    func_8007AF38(0, 0, 1);
                    D_800C8CE8 = 1;
                    D_800EFD70.f3FC = (s32)p1;
                    D_800EFD70.flags |= 0x8000;
                }
                break;
            case 2:
                D_800EFD70.flags &= ~0x80000;
                if (D_800EFD64->f50 & 2) {
                    func_800BCB04((void *)D_800C9570);
                    D_800C9570 = 0;
                }
                func_8007AF38((s32)D_800CD79C, 0, 1);
                D_800C8CE4 = 0;
                break;
            }
        } else if (p1->f16 == 7) {
            func_8007AF38((s32)D_800CD694, 1, 1);
        } else {
            D_800EFD70.sel = 0;
            D_800EFD70.state = 3;
        }
        return;
    }
    if (D_800EFD70.state != 3) {
        return;
    }
    orig = D_800EFD70.sel;
    if (fl & 0x801) {
        if (D_800F0954 - D_800F0A3C >= D_800F0950 || D_800EFD70.f4C == 8) {
            D_800EFD70.sel = orig ^ 2;
        } else {
            D_800EFD70.sel = orig - 1;
            if (orig - 1 < 0) {
                D_800EFD70.sel = orig + 2;
            }
        }
    }
    if (*p3 & 0x1002) {
        if (D_800F0954 - D_800F0A3C >= D_800F0950 || D_800EFD70.f4C == 8) {
            D_800EFD70.sel ^= 2;
        } else {
            v = D_800EFD70.sel;
            D_800EFD70.sel = v + 1;
            if (v + 1 >= 3) {
                D_800EFD70.sel = v - 2;
            }
        }
    }
    if (D_800EFD70.sel != orig) {
        func_8001102C(0);
    }
}
