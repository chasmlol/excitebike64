#include "ctx.h"

typedef struct {
    u8 pad[0x4C8];
    s16 msg[16];      /* 0x4C8 */
    u8 f4E8[0x68];    /* 0x4E8 */
    u32 a;            /* 0x550 */
    u32 b;            /* 0x554 */
    u32 c;            /* 0x558 */
    u32 d;            /* 0x55C */
} Task;

extern Task D_800E25C0[2];
extern OSMesgQueue D_800E2048;
extern OSMesgQueue D_800E3090;
extern u32 D_800C29F0;
extern u32 D_800C29F4;
extern u32 D_800C29F8;
extern u32 D_800C29FC;
extern u32 D_800C2350;
extern u32 D_800CEC04;

s32 func_80065858(void);
void func_800B0610(OSMesgQueue *q, OSMesg *m, s32 f);
s32 func_800B1150(OSMesgQueue *q, OSMesg m, s32 f);
u32 func_800B1330(void);
void func_800B2B00(u32 a0);
void func_800A11BC(s32 a0);
void func_800325FC(s32 a0);
void func_80033E30(s32 a0);

void func_8000E6E8(void)
{
    s16 *msg;
    u32 idx;
    u32 t;
    u32 v;

    for (;;) {
        func_800B0610(&D_800E2048, (OSMesg *)&msg, 1);
        switch (*msg) {
        case 1:
            D_800C29F0++;
            idx = func_80065858() != (s32)D_800E25C0[0].f4E8;
            if (idx != D_800C29F8) {
                D_800E25C0[idx].a = func_800B1330();
            }
            D_800C29F8 = idx;
            if ((D_800C29FC ^ D_800C2350) & 0x1000) {
                D_800C29FC = D_800C2350 & 0x1000;
                func_800B2B00(D_800C29FC ? 0x6A : 0xAA);
            }
            if (MQ_GET_COUNT(&D_800E3090) < 14) {
                func_800B1150(&D_800E3090, msg, 0);
            }
            break;
        case 2:
            idx = msg != D_800E25C0[0].msg;
            D_800C29F4++;
            if (D_800CEC04 != 0) {
                func_800A11BC(idx);
            } else {
                func_800325FC(idx);
                func_80033E30(idx);
            }
            t = func_800B1330();
            D_800E25C0[idx].b = t;
            D_800E25C0[idx].c = t - D_800E25C0[idx].a;
            func_800B1150(&D_800E3090, msg, 1);
            break;
        }
    }
}
