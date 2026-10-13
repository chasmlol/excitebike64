#include "ctx.h"

typedef struct Node {
    struct Node *next;
    u8 *name;
    u32 val;
} Node;

u32 func_800AEC10(u8 *s);
s32 func_8000ECE0(s32 a0);
s32 func_8000EDA0(u8 *param_1, u8 *param_2);
void func_8000ECB8(s32 a);
void func_8000EB00(s32 a, ...);
void func_800BF79C(u32 *a, u32 v);
extern char D_800DB454[];

void func_800BFF60(u32 *param_1, u8 *param_2, u32 *param_3)
{
    s32 i = 0;
    u32 sum = 0;
    u32 n;
    Node *p;

    for (;;) {
        n = func_800AEC10(param_2);
        if (!((u32)i < n)) break;
        if (!(i < 16)) break;
        sum += func_8000ECE0(param_2[i]);
        i++;
    }
    {
        s32 base = (s32)param_1;
        p = *(Node **)(base + ((sum & 0xFF) << 2) + 8);
    }
    if (p != NULL) {
        do {
            if (func_8000EDA0(p->name, param_2) == 0) break;
            p = p->next;
        } while (p != NULL);
        if (p != NULL) goto LAB;
    }
    func_8000EB00((s32)D_800DB454, param_2);
    func_8000ECB8(1);
LAB:
    if (param_3 != NULL) *param_3 = p->val;
    func_800BF79C(param_1, p->val);
}
