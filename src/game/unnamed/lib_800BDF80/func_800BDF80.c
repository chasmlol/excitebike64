#include "ctx.h"

typedef struct Node {
    struct Node *next;
    struct Node *prev;
    u8 *buf;
    s32 c;
    s32 d;
} Node;

typedef struct {
    u8 pad[0x10];
    u32 flags;
    s32 f14;
    s32 f18;
    u32 f1C;
} Rec;

typedef struct {
    s32 a, b, c, d;
} Slot;

extern s32 D_800D5700;
extern u8 D_800DB410[];
extern s32 D_80100670;
extern void *D_80100674;
extern Node *D_80100678;
extern Node *D_8010067C;
extern Node D_80100680;
extern s32 D_80100694;
extern u32 D_80100698;
extern u8 *D_8010069C;
extern s32 D_801006A0;
extern Rec *D_801006A4;
extern Slot D_801006B0[];
extern u8 *D_801006F0;

void *func_800BC908(s32 size, u32 flags, s32 owner, s32 tag);
void func_800BCB04(void *ptr);
void func_800AEC34(void *dst, void *src, s32 n);
void func_8000ED10(u8 *a0, u8 a1, s32 a2);
void func_800BEB68(void *param_1, void *param_2, s32 param_3);

s32 func_800BDF80(u8 *src)
{
    Node *p;
    Node *last;
    u8 *mem;
    u8 *buf;
    s32 off;
    s32 i;
    u32 n;
    s32 size;
    u32 max;
    u32 v;
    u32 pages;
    s32 k;

    if (D_80100670 == 0) {
        D_80100674 = mem = func_800BC908((D_800D5700 + 2) * sizeof(Node) + (off = (D_800D5700 + 1) << 13), 0, (s32)D_800DB410, 0xC2);
        if (mem == NULL) {
            return -1;
        }
        buf = mem - 0x2000;
        p = (Node *)(mem + off);
        i = 0;
        D_80100678 = p;
        for (; i < D_800D5700 + 2U; i++) {
            p->next = p + 1;
            p->prev = p - 1;
            p->buf = buf;
            p->c = 0;
            p->d = -1;
            p++;
            buf += 0x2000;
        }
        D_8010067C = last = p - 1;
        D_8010069C = src;
        D_801006F0 = last->buf;
        D_80100678->prev = NULL;
        last->next = NULL;
        D_80100678->buf = NULL;
        last->buf = NULL;
        func_800BEB68(src, D_801006F0, 0x2000);
        n = *(u32 *)D_801006F0;
        D_8010069C += 4;
        D_80100698 = n;
        size = n * 32;
        D_801006A0 = (s32)D_8010069C + size;
        if (size + 4 > 0x2000U) {
            func_800BCB04(D_80100674);
            D_80100674 = NULL;
            return -1;
        }
        D_801006A4 = func_800BC908(size, 0, (s32)D_800DB410, 0x100);
        func_800AEC34(D_801006A4, D_801006F0 + 4, size);
        max = 0;
        i = 0;
        if (D_80100698 != 0) {
        do {
            func_800BEB68((void *)(D_801006A4[i].f18 += D_801006A0), D_801006F0, 0x10);
            if (D_801006F0[0] == 's' && D_801006F0[1] == 'W' && D_801006F0[2] == 'd' && D_801006F0[3] == 0xB0) {
                D_801006A4[i].flags |= 1;
                v = (D_801006F0[8] << 24) | (D_801006F0[9] << 16) | (D_801006F0[10] << 8) | D_801006F0[11];
                D_801006A4[i].f1C = v;
                if (max < v) {
                    max = v;
                }
            } else {
                D_801006A4[i].f1C = D_801006A4[i].f14;
            }
        i++;
        } while (i < D_80100698);
        }
        i = (max + 0x1FFF) >> 13;
        max = i + 4;
        D_80100694 = 0;
        func_8000ED10((u8 *)&D_80100680, 0, sizeof(Node));
        if (max > 0x800) {
            D_80100694 = (i + 5) * 4;
            D_80100680.buf = func_800BC908(D_80100694, 0, (s32)D_800DB410, 0x13D);
            if (D_80100680.buf == NULL) {
                return -1;
            }
            D_80100680.c = 0;
            D_80100680.d = -1;
        }
        for (i = 3; i >= 0; i--) {
            D_801006B0[i].a = 0;
        }
        D_80100670 = 1;
    }
    return 0;
}
