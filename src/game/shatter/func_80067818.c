#include "ctx.h"

extern u32 func_8000EFE8(void);
extern s32 D_800CCCE0;
extern s32 *D_800F1444;
extern s32 *D_800C9640;

void func_80067818(s32 param_1) {
    s32 *ent;
    s32 *p;
    s32 cnt;
    s32 key;
    s32 i;
    s32 uVar5;
    s32 off;
    u8 *rec;
    s32 t;
    s32 n2;

    t = D_800CCCE0;
    i = 0;
    if (t > 0) {
        cnt = t;
        p = D_800F1444;
        key = (s32)(D_800C9640 + (param_1 * 8 + 1));
    loop:
        ent = p;
        if (*ent == key) {
            goto chk;
        }
        i++;
        p = ent + 0xc;
        if (i < cnt) {
            goto loop;
        }
    }
chk:
    uVar5 = 0;
    if (i != D_800CCCE0) {
        ent[2] = 0;
        if (ent[1] > 0) {
            off = 0;
            do {
                rec = (u8 *)ent[11] + off;
                *(u16 *)(rec + 0x2A) = 3;
                if ((uVar5 & 1) != 0) {
                    *(u16 *)(rec + 0xC) = *(u16 *)(rec - 0x20);
                    *(u16 *)(rec + 0xE) = *(u16 *)(rec - 0x1E);
                    *(u16 *)(rec + 0x10) = *(u16 *)(rec - 0x1C);
                    *(u16 *)(rec + 0x24) = *(u16 *)(rec - 0x8);
                    *(u16 *)(rec + 0x26) = *(u16 *)(rec - 0x6);
                    *(u16 *)(rec + 0x28) = *(u16 *)(rec - 0x4);
                } else {
                    *(u16 *)(rec + 0xC) = 0;
                    *(u16 *)(rec + 0xE) = 0;
                    *(u16 *)(rec + 0x10) = 0;
*(u16 *)(rec + 0xC) = *(u16 *)(rec + 0xC) + ((s32)func_8000EFE8() - 0x80) / 2;
*(u16 *)(rec + 0xE) = *(u16 *)(rec + 0xE) + (((s32)func_8000EFE8() - 0x80) >> 2) / 2;
*(u16 *)(rec + 0x10) = *(u16 *)(rec + 0x10) + ((s32)func_8000EFE8() - 0x80) / 2;
*(u16 *)(rec + 0x24) = ((s32)func_8000EFE8() - 0x80) * 4;
*(u16 *)(rec + 0x26) = ((s32)func_8000EFE8() - 0x80) * 5;
*(u16 *)(rec + 0x28) = ((s32)func_8000EFE8() - 0x80) * 2;
                }
                n2 = ent[1];
                off += 0x2C;
            } while ((s32)(++uVar5) < n2);
        }
    }
}

