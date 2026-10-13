#include "ctx.h"

typedef struct Hdr {
    struct Hdr *next;
    u8 pad4;
    u8 b5;
    u8 pad6;
    u8 b7;
    u8 pad8[8];
    u16 size;
} Hdr;

typedef struct {
    s32 pad0;
    u32 *tbl;
} Arc;

extern char *D_8010537C;
extern char D_800D5740[];
extern s32 *D_800D5870;
extern char D_800DB4D0[];
extern char D_800DB4DC[];
extern char D_800DB504[];
extern char D_800DB520[];
extern char D_800EE380[];
extern s32 *D_80105370;
extern s32 D_80105374;
extern s32 *D_80105378;
extern s32 *D_80105380[16];

s32 *func_800BD730(s32 a0, s32 a1, char *a2, s32 a3);
void *func_800BC908(s32 size, u32 flags, char *owner, s32 tag);
s32 *func_800BD798(s32 *p, u32 size, s32 a, char *b, s32 c);
s32 *func_800BF79C(Arc *a0, u32 a1);
s32 func_800C1A6C(void *hdr);
void func_800BFEA4(u32 *p);
void func_800BCB04(void *ptr);
void func_800AEC34(void *dst, void *src, s32 n);
void func_800AEC60(char *buf, char *fmt);
void func_8000ECC4();
void func_8000ECB8(s32) __attribute__((noreturn));

s32 func_800C06D0(Arc *arc, u32 **ids, char *name)
{
    s32 *base;
    s32 *src;
    s32 *h;
    s32 *tbl;
    u32 k, j;

    if (name != NULL) {
        D_8010537C = name;
    } else {
        D_8010537C = D_800D5740;
    }
    k = 0x30000;
    if (D_800D5870 != NULL) {
        base = D_800D5870;
    } else {
        base = func_800BD730(k, 0, D_800DB4D0, 0x124);
    }
    if (base == NULL) {
        func_800AEC60(D_800EE380, D_800DB4DC);
        func_8000ECC4();
        return -1;
    }
    D_80105378 = (s32 *)((u8 *)base + k - 0x1000);
    D_80105374 = 0;
    tbl = func_800BC908(0x2800, 0, D_800DB4D0, 0x132);
    D_80105370 = tbl;
    for (k = 0; k < 40; k++) {
        *tbl++ = -1;
        for (j = 1; j < 64; j++) {
            *tbl++ = 0x80000000;
        }
    }
    D_80105380[0] = base;
    for (k = 1; k < 16; k++) {
        if (*ids == NULL) {
            break;
        }
        h = func_800BF79C(arc, **ids);
        if (h == NULL) {
            func_800BCB04(D_80105370);
            D_80105370 = NULL;
            func_800BCB04(D_80105380[0]);
            D_80105380[0] = NULL;
            func_800AEC60(D_800EE380, D_800DB504);
            func_8000ECC4();
            return -1;
        }
        j = ((Hdr *)h)->size;
        func_800C1A6C(h);
        src = h;
        j += 0x10;
        j += 0xF;
        j >>= 2;
        j >>= 1;
        j >>= 1;
        j--;
        D_80105380[k] = base;
        while (j != -1) {
            base[0] = src[0];
            base[1] = src[1];
            base[2] = src[2];
            base[3] = src[3];
            base += 4;
            src += 4;
            j--;
        }
        func_800BFEA4(arc->tbl + **ids);
        ids++;
    }
    for (; k < 16; k++) {
        D_80105380[k] = NULL;
    }
    func_800BCB04(D_80105370);
    src = D_80105378 - D_80105374 * 0x400;
    k = D_80105374 << 8;
    k |= 0xFF;
    D_80105370 = NULL;
    while (k != -1) {
        base[0] = src[0];
        base[1] = src[1];
        base[2] = src[2];
        base[3] = src[3];
        base += 4;
        src += 4;
        k--;
    }
    k = (u8 *)base - (u8 *)D_80105380[0];
    if (D_800D5870 != NULL) {
        base = func_800BC908(k, 0, D_800DB4D0, 0x1A0);
        if (base == NULL) {
            func_8000ECB8(1);
        }
        func_800AEC34(base, D_800D5870, k);
    } else {
        base = func_800BD798(D_80105380[0], k, 0, D_800DB4D0, 0x1A9);
    }
    if (base == NULL) {
        func_800AEC60(D_800EE380, D_800DB520);
        func_8000ECC4();
        return -1;
    }
    D_80105378 = (s32 *)((u8 *)base + k - 0x1000);
    k = (u8 *)base - (u8 *)D_80105380[0];
    for (j = 0; j < 16; j++) {
        h = D_80105380[j];
        if (h == NULL) {
            break;
        }
        D_80105380[j] = (s32 *)((u8 *)h + k);
        if (((Hdr *)h)->b5 == 0) {
            ((Hdr *)h)->next = NULL;
        } else {
            ((Hdr *)h)->next = (Hdr *)((u8 *)h + (((Hdr *)h)->b7 * 8 + 0x10));
        }
    }
    return 0;
}
