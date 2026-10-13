#include "ctx.h"

extern u8 D_800E1C48[];
void func_8000ED10(u8 *a0, s32 a1, s32 a2);
void func_8000CD00();
void func_800AD910(u8 *a0, s32 a1);
void func_800AD7E0(u8 *a0, s32 a1);
void func_800AC240(s32 a0, s32 a1, u8 *a2);

void func_8008C424(s32 c, u8 *buf, s32 n)
{
    s32 full;
    s32 v;
    u32 i;
    u32 j;
    u8 *p;

    v = 0x55;
    func_8000CD00();
    func_8000ED10(buf, 0x55, n << 3);
    if (0 < n) {
        do {
            i = 0;
            full = 0;
            if (n != 0) {
                p = buf;
                do {
                    for (j = 0; j < 8; j++) {
                        if (buf[i * 8 + j] != v) {
                            break;
                        }
                    }
                    if (j != 8) {
                        full = full + 1;
                    } else {
                        func_8000ED10(p, v + 1, 8);
                        func_800AD910(p, 8);
                        func_800AC240((s32)D_800E1C48, (u8)(c + i), p);
                        func_800AD7E0(p, 8);
                    }
                    i = i + 1;
                    p = p + 8;
                } while (i < n);
            }
            v = v + 1;
        } while (full < n);
    }
    func_800AD7E0(buf, n << 3);
}
