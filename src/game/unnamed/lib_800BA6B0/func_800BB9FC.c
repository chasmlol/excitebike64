#include "ctx.h"

typedef struct {
    u8 *buf;
    u32 len;
    u8 unk8[0x1C - 0x8];
} Chan;

extern Chan D_800FE8F0[];
extern u32 D_800FE8DC;
extern u16 D_800D3404[];

void func_800B9FE0(u8 *dst, u8 *src, s32 n);

void func_800BB9FC(u32 idx, u8 *src, s32 n) {
    s32 i;

    if (idx <= 0x100) {
        if (D_800FE8F0[idx].len < n) {
            n = D_800FE8F0[idx].len;
        }
        if (D_800FE8DC & 2) {
            func_800B9FE0(D_800FE8F0[idx].buf, src, n);
        } else {
            for (i = 0; i < n; i++) {
                D_800FE8F0[idx].buf[i] = D_800D3404[src[i]] >> 8;
            }
        }
    }
}
