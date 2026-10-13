#include "ctx.h"

typedef struct { s16 a, b, c, d, e, pad; } Rec;
typedef struct {
    s32 n;
    s32 ntrk;
    s32 id;
    u8 len[8];
    u8 data[8][30];
    Rec rec[1];
} Obj;

extern Obj *D_800C9570;
extern u8 D_800DB040[];

void *func_800BC908(s32 size, u32 flags, s32 owner, s32 tag);
void func_800BCB04(void *ptr);
void func_800AEC34(void *dst, void *src, s32 n);

void func_800A44EC(u8 *p)
{
    s32 id;
    u16 n;
    u16 start;
    s32 ntrk;
    s16 i;
    s32 k;
    s32 j;
    Obj *o;
    Rec *r;

    id = p[1];
    p += 0xC;
    n = *((u16 *)p)++;
    start = *((u16 *)p)++;
    ntrk = *((s16 *)p)++;
    if (D_800C9570 != NULL) {
        func_800BCB04(D_800C9570);
    }
    D_800C9570 = func_800BC908(((s16)n - 1) * sizeof(Rec) + sizeof(Obj), 0, (s32)D_800DB040, 0xFD);
    D_800C9570->id = id;
    D_800C9570->ntrk = ntrk;
    for (i = 0; i < ntrk; i++) {
        D_800C9570->len[i] = p[1];
        func_800AEC34((u8 *)(i * 30 + (u32)D_800C9570) + 0x14, p + 2, D_800C9570->len[i] * 4 - 2);
        p += D_800C9570->len[i] * 4;
    }
    D_800C9570->n = (s16)n;
    for (k = 0; k < (s16)n; k++) {
        j = (k - (s16)start + (s16)n) % (s16)n;
        D_800C9570->rec[j].a = p[0] >> 3;
        D_800C9570->rec[j].b = (p[1] >> 6) | ((p[0] & 7) << 2);
        D_800C9570->rec[j].c = p[1] & 0x3F;
        D_800C9570->rec[j].d = p[2] >> 6;
        D_800C9570->rec[j].e = p[2] & 0x3F;
        p += 3;
    }
}
