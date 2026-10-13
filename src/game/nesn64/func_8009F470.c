#include "ctx.h"

typedef struct { u8 b[0x4000]; } B4000;
typedef struct { u8 b[0x2000]; } B2000;
typedef struct { u8 b[0x2812]; } B2812;
typedef struct { u8 pad[0x10000]; u8 arr[0x4000]; } Mem;

extern s32 D_800F5AA0;
extern u8 *D_800F5A50;
extern B2812 *D_800F5A40;
extern u8 *D_800F5A54;
extern u16 *D_800F5A58;
extern void *D_800F5A60[];
extern void *D_800F5A68[];
extern void *D_800D0660;
extern s32 D_800CEC04;
extern s32 D_800CEC00;
extern s32 D_800D14D8;
extern char D_800DAF30[];
extern char D_800DAF38[];
extern u8 D_800D0514[];

extern void func_8000ECB8() __attribute__((noreturn));
void func_8000ED10(u8 *a0, u8 a1, s32 a2);
s32 func_800A2F60(u8 *param_1, void *param_2);
void func_800BFDD4(s32 *param_1);
void func_800A1484(void);
void *func_800BC908(s32 size, u32 flags, char *owner, s32 tag);
void func_800A2D64(void);
s32 func_800BF530(char *path, s32 **out);
u8 *func_800BF79C(s32 *a, s32 b);
void func_800A070C();

void func_8009F470(void)
{
    s32 *obj;
    u8 *src;
    s32 m, t, x, y;
    u32 r, g, b;
    s32 i, j, n, k;
    u32 v, c;
    u16 *p;

    D_800F5AA0 = 0;
    if (func_800BF530(D_800DAF30, &obj) != 0) {
        func_8000ECB8(1);
    }
    D_800F5A50 = (u8 *)0x80380000;
    func_8000ED10(D_800F5A50, 0, 0x141C0);
    src = func_800BF79C(obj, D_800D14D8);
    *(B4000 *)(D_800F5A50 + 0x8000) = *(B4000 *)src;
    *(B4000 *)(D_800F5A50 + 0xC000) = *(B4000 *)src;
    *(B2000 *)(D_800F5A50 + 0x10000) = *(B2000 *)(src + 0x4000);
    func_800A2F60(D_800F5A50, func_800A070C);
    D_800F5A40 = func_800BC908(0x2812, 0, D_800DAF38, 0x9C);
    *D_800F5A40 = *(B2812 *)(src + 0x6000);
    func_800BFDD4(obj);
    D_800F5A54 = func_800BC908(0x8000, 0, D_800DAF38, 0xA4);
    m = 0x80380000;
    t = 0x10000;
    for (i = 0; i < 0x2000; i += 0x10) {
        k = i * 4;
        for (j = 0; j < 8; j++) {
            x = i + j;
            y = x + 8;
            c = *(u8 *)(y + m + t);
            v = *(u8 *)(x + m + t);
            v |= c << 8;
            for (n = 0; n < 8; n += 2) {
                c = (v >> 7) & 1;
                if (v & 0x8000) {
                    c |= 2;
                }
                v <<= 1;
                D_800F5A54[k] = c << 4;
                c = (v >> 7) & 1;
                if (v & 0x8000) {
                    c |= 2;
                }
                v <<= 1;
                D_800F5A54[k] |= c;
                k++;
            }
            k += 4;
        }
    }
    D_800F5A58 = func_800BC908(0x200, 0, D_800DAF38, 0xBF);
    p = D_800F5A58;
    for (i = 0; i < 0x100; i++) {
        j = D_800D0514[i * 3] >> 3;
        j <<= 11;
        n = D_800D0514[i * 3 + 1] >> 3;
        n <<= 6;
        k = D_800D0514[i * 3 + 2] >> 3;
        k <<= 1;
        p[i] = j | n | k | 1;
        if (p[i] == 0x8685) {
            p[i] &= 0xFFFE;
        }
    }
    for (i = 0; i < 2; i++) {
        D_800F5A60[i] = func_800BC908(0x200, 0, D_800DAF38, 0xCC);
        D_800F5A68[i] = func_800BC908(0x200, 0, D_800DAF38, 0xCD);
    }
    D_800D0660 = 0;
    for (i = 0; i < 3; i++) {
        func_800A2D64();
    }
    D_800D0660 = func_800A070C;
    D_800CEC04 = 1;
    func_800A1484();
    D_800CEC00 = 0;
}
