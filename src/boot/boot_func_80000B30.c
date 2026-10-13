#include "ctx.h"

typedef struct Rec {
    s32 state;        /* 0x00 */
    u8 r4[0x1C];      /* 0x04 */
    u8 b20[2];        /* 0x20 */
    u8 flag;          /* 0x22 */
    u8 b23;           /* 0x23 */
    u8 *rp;           /* 0x24 */
    s32 f28;          /* 0x28 */
    s32 f2c;          /* 0x2C */
    s32 f30;          /* 0x30 */
    s32 f34;          /* 0x34 */
    s32 f38;          /* 0x38 */
    s32 f3c;          /* 0x3C */
    s32 f40;          /* 0x40 */
    s32 f44;          /* 0x44 */
    s32 f48;          /* 0x48 */
} Rec;

extern s32 D_80017964;
extern s32 D_80017C00;
extern Rec D_80017980[];
extern void (*D_8000CE04)(s32, s32, s32);
s32 boot_func_80008030(void *a, s32 b, s32 c);
s32 boot_func_80006150(s32 a, void *b, s32 c);

void boot_func_80000B30(void)
{
    s32 i;
    s32 iVar2;

    D_80017964 = 0;
    D_80017C00 = 0;
    for (i = 0; i < 8; i++) {
        if (D_80017980[i].state == 1) {
            iVar2 = boot_func_80008030(D_80017980[i].r4, 0, 0);
            if (iVar2 == 0) {
                D_80017980[i].state = 3;
            } else {
                D_80017964 = D_80017964 + 1;
                return;
            }
        } else if (D_80017980[i].state == 2) {
            if (D_8000CE04 == 0) {
                D_80017980[i].rp = D_80017980[i].r4;
                D_80017C00 = D_80017C00 + 1;
                D_80017980[i].flag = 0;
                D_80017980[i].f28 = D_80017980[i].f3c;
                D_80017980[i].f2c = D_80017980[i].f40;
                D_80017980[i].f30 = D_80017980[i].f44;
                iVar2 = boot_func_80006150(D_80017980[i].f38, D_80017980[i].b20, D_80017980[i].f48);
                if (iVar2 == 0) {
                    D_80017980[i].state = 1;
                    return;
                }
            } else {
                D_8000CE04(D_80017980[i].f40, D_80017980[i].f3c, D_80017980[i].f44);
                D_80017980[i].state = 3;
            }
        }
    }
}
