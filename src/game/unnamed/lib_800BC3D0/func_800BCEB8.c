#include "ctx.h"

typedef struct {
    char *file;
    u32 info;
    u32 size;
    u32 chk;
} MemBlk;

extern u32 D_800D56C0[8];
extern u32 D_800D56E0[5];
extern u32 D_8010064C;

void func_800AEC60(char *dst, char *src);
void func_8000EFB0(char *s);
void func_8000ED30(char *dst, char *src);

char *strcpy(char *, const char *);

static inline void AddHex(char *s, u32 v)
{
    char dig[12];
    s32 i;

    for (i = 0; i < 8; i++) {
        s32 q = v / D_800D56C0[i];
        if (q < 10) dig[i] = q + '0';
        else dig[i] = q + '7';
        v -= q * D_800D56C0[i];
    }
    dig[8] = 0;
    func_8000ED30(s, dig);
}

static inline void AddDec(char *s, u32 v)
{
    char dig[12];
    s32 i;

    v &= 0xFFFF;
    for (i = 0; i < 5; i++) {
        s32 q = v / D_800D56E0[i];
        if (q < 10) dig[i] = q + '0';
        else dig[i] = q + '7';
        v -= q * D_800D56E0[i];
    }
    dig[5] = 0;
    for (i = 0; i < 4; i++) {
        if (dig[i] != '0') break;
        dig[i] = ' ';
    }
    func_8000ED30(s, dig);
}

s32 func_800BCEB8(s32 a, u32 b, s32 c)
{
    char buf[0x50];
    s32 count;
    MemBlk *p;

    char *s;
    char *f;

    func_800AEC60(buf, "\n");
    func_8000EFB0(buf);
    count = 0;
    if (a == 0) {
        return count;
    }

    for (p = (MemBlk *)(a + 0x20); (u32)p < b; p = (MemBlk *)((u8 *)p + (p->size << 4))) {
        if ((u32)p->file + p->info + p->size + p->chk != 0xB562439F) {
            strcpy(buf, " Addr 0x");
            AddHex(buf, (u32)p);
            func_8000ED30(buf, " Size ?????????? - Corrupt\n");
            if (c) {
                func_8000EFB0(buf);
            }
            count = -1;
            break;
        }
        if (!(p->info & 1)) {
            strcpy(buf, " Addr 0x");
            AddHex(buf, (u32)p);
            func_8000ED30(buf, " Size 0x");
            AddHex(buf, p->size << 4);
            func_8000ED30(buf, " - Free\n");
            if (c) {
                func_8000EFB0(buf);
            }
            count++;
            continue;
        }
        if ((p->info & 0xFFFE) < D_8010064C) {
            continue;
        }
        f = p->file;
        if (f == 0) {
            strcpy(buf, " Addr 0x");
            AddHex(buf, (u32)p);
            func_8000ED30(buf, " Size 0x");
            AddHex(buf, p->size << 4);
            func_8000ED30(buf, " - Time ");
            AddDec(buf, (p->info & 0xFFFE) >> 1);
        } else {
            s = f;
            while (*s++) {
            }
            for (s--; s >= f; s--) {
                if (*s == 0x5C || *s == 0x2F) break;
            }
            s++;
            strcpy(buf, " Addr 0x");
            AddHex(buf, (u32)p);
            func_8000ED30(buf, " Size 0x");
            AddHex(buf, p->size << 4);
            func_8000ED30(buf, " - Time ");
            AddDec(buf, (p->info & 0xFFFE) >> 1);
            func_8000ED30(buf, " Line ");
            AddDec(buf, *(u16 *)&p->info);
            func_8000ED30(buf, " File ");
            func_8000ED30(buf, s);
        }
        func_8000ED30(buf, "\n");
        if (c) {
            func_8000EFB0(buf);
        }
    }
    func_800AEC60(buf, "\n");
    if (c) {
        func_8000EFB0(buf);
    }
    return count;
}
