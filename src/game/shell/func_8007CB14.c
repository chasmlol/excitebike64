#include "ctx.h"

typedef struct {
    s32 pad0[2];
    char *names[20];
} Entry; /* 0x58 */

typedef struct {
    u8 pad0[0x1C];
    s32 sel[8];
    s32 flag;
    u8 pad40[0x34];
    s32 *obj[40];
} State;

typedef struct {
    s32 x, y;
} Pos;

extern State D_800EFD70;
extern Entry D_800CDB78[];
extern Pos D_800E1718;
extern char D_800D9AD4[];

s32 *func_800C1E60(s32 *param_1, s32 param_2, s32 param_3, s32 param_4, s32 param_5);
char *func_800C1F2C(s32 *p, char *s);
void func_800C0ACC(s32 *a, s32 x, s32 y);

void func_8007CB14(s32 p, s32 q)
{
    s32 i, n, off;

    if (D_800EFD70.flag != 1) {
        return;
    }
    func_800C1E60(D_800EFD70.obj[20], 0x40, 2, 2, 0x20);
    func_800C1F2C(D_800EFD70.obj[20], D_800D9AD4);
    func_800C0ACC(D_800EFD70.obj[20], 0x140, 0x34);
    func_800C1F2C(D_800EFD70.obj[p * 20 + 17], D_800CDB78[D_800EFD70.sel[p]].names[0]);
    func_800C0ACC(D_800EFD70.obj[p * 20 + 17], D_800E1718.x + 0x10E, q + D_800E1718.y - 0x6F);
    func_800C1F2C(D_800EFD70.obj[p * 20 + 18], D_800CDB78[D_800EFD70.sel[p]].names[1]);
    func_800C0ACC(D_800EFD70.obj[p * 20 + 18], D_800E1718.x + 0x10B, q + D_800E1718.y - 0x62);
    func_800C1F2C(D_800EFD70.obj[p * 20 + 19], D_800CDB78[D_800EFD70.sel[p]].names[2]);
    func_800C0ACC(D_800EFD70.obj[p * 20 + 19], D_800E1718.x + 0x108, q + D_800E1718.y - 0x55);
    for (i = 0; i < 4; i++) {
        func_800C1F2C(D_800EFD70.obj[p * 20 + 6 + i], D_800CDB78[D_800EFD70.sel[p]].names[i + 4]);
        func_800C0ACC(D_800EFD70.obj[p * 20 + 6 + i], D_800E1718.x + 60 - i * 3, q + D_800E1718.y - 0x6F + i * 13);
    }
    n = 4;
    while (D_800CDB78[D_800EFD70.sel[p]].names[n + 4] != 0) {
        n++;
    }
    off = -100;
    for (; D_800CDB78[D_800EFD70.sel[p]].names[i + 4] != 0; i++) {
        func_800C1F2C(D_800EFD70.obj[p * 20 + 6 + i], D_800CDB78[D_800EFD70.sel[p]].names[i + 4]);
        func_800C0ACC(D_800EFD70.obj[p * 20 + 6 + i], D_800E1718.x + 60 - i * 3, q + D_800E1718.y + off + i * 13);
    }
}
