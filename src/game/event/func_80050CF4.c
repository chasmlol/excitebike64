#include "ctx.h"

extern s32 D_800C8CE8;
extern s32 D_800C8CE0;
extern s32 D_800EFDBC;
extern s32 D_800EFDB0;
extern s32 *D_800CA930;
extern s32 D_800CA9C8[];
extern s32 D_800E37C0[];
extern s32 *D_800EFD64;
typedef struct { s32 f0; s32 arr[4]; u8 b[4][3]; u8 pad20[0xC]; s32 val; } SS;
extern SS D_800F05A8;
#define S (&D_800F05A8)
extern s32 D_800EFD70[];
void func_8004EE2C(s32 a);

s32 func_80050CF4(s32 *p)
{
    u8 ret = 0;
    s32 i;
    s32 j;
    s32 *ch;
    u8 *src;

    if (((u8 *)p)[0x6E8] != 0) goto done;
    if (D_800C8CE8 != 0) goto done;
    if (D_800EFDBC == 1 && D_800CA930 != 0 && ((*D_800CA930 - (s32)D_800CA9C8) * 0x38E38E39 >> 2) == 5) goto done;
    ch = D_800EFD64;
    if (ch[0x14] & 2) goto done;
    if (S->val >= S->arr[2]) goto done;
    if (D_800EFD70[19] == 8 && p != D_800E37C0 + D_800C8CE0 * 0x1f8) goto done;
    if (D_800EFD70[16] != *(s16 *)((u8 *)ch + 0x1E)) goto done;

    ret = S->val < S->arr[0];
    for (i = 0; i < 3; i++) {
        if (S->val <= S->arr[i]) {
            for (j = 1; j >= i; j--) {
                S->arr[j + 1] = S->arr[j];
                S->b[j + 1][0] = S->b[j][0];
                S->b[j + 1][1] = S->b[j][1];
                S->b[j + 1][2] = S->b[j][2];
            }
            break;
        }
    }
    S->arr[i] = S->val;
    S->b[i][0] = 0xFF;
    S->b[i][1] = 0xFF;
    S->b[i][2] = (D_800EFDBC == 8) ? 0 : (u8)(((u32)((s32)p - (s32)D_800E37C0) * 0xBEFBEFBFu) >> 5);
    D_800EFD64[0x14] |= 0x100;
    if (ret) func_8004EE2C((((s32)p - (s32)D_800E37C0) * -0x41041041) >> 5);
done:
    return ret;
}
