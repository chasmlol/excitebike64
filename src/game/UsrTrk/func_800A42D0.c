#include "ctx.h"

typedef struct { u8 b[10]; } Blk10;

typedef struct Node {
    s32 hdr;
    struct Node *next;
    u8 p8[5];
    u8 f0D;
    u16 f0E;
    u8 p10;
    u8 f11;
    u8 p12;
    u8 f13;
    void *f14;
} Node;

typedef struct {
    u8 pad0[0x17C];
    Node *list;
    u8 pad1[0x10];
    Node *cur;
    u8 pad2[0x16BA - 0x194];
    Blk10 blk;
    u8 pad3[0x16F4 - 0x16C4];
    s32 n16F4;
    u8 pad4[0x17FB - 0x16F8];
    u8 b17FB;
} Gs;

typedef struct { u16 f0; s16 cnt; u8 pad[0x3C]; } Ent40;
typedef struct { u16 w[15]; } Row;

extern Gs *D_800CEA40;
extern Ent40 D_800CE280[];
extern Row D_800F55B8[];

s32 func_800A42D0(u8 *buf)
{
    u8 *p = buf;
    Node *n = D_800CEA40->list;
    s32 cnt, curi, k, j, tot;
    u16 e;
    u8 *d;
    s16 c;
    u8 *q;

    if (n != NULL) {
        cnt = 0;
        do {
            if (n == D_800CEA40->cur) {
                curi = cnt;
            }
            n = n->next;
            tot = cnt + 1;
            cnt = tot;
        } while (n != NULL && n != D_800CEA40->list);

        p[0] = 0x35;
        p[1] = D_800CEA40->b17FB;
        p += 2;
        *(Blk10 *)p = D_800CEA40->blk;
        p += 10;
        *(u16 *)p = tot;
        p += 2;
        *(u16 *)p = curi;
        p += 2;
        *(u16 *)p = *(u16 *)((u8 *)D_800CEA40 + 0x16F6);
        p += 2;
        for (k = 0; k < D_800CEA40->n16F4; k++) {
            c = D_800CE280[k + 18].cnt;
            *(u16 *)p = c;
            d = p + 2;
            for (j = 0; j < c * 2 - 1; j++) {
                ((u16 *)d)[j] = D_800F55B8[k].w[j];
            }
            p += c * 4;
        }
        n = D_800CEA40->list;
        do {
            e = ((u32)n->f14 - (u32)D_800CE280) >> 6;
            p[0] = ((n->f0D & 0x1F) << 3) | ((n->f0E & 0x1F) >> 2);
            p[1] = ((((u8 *)n)[0xF] & 0x1F) << 6) | (n->f11 & 0x3F);
            p[2] = ((n->f13 & 3) << 6) | (e & 0x3F);
            p += 3;
            n = n->next;
        } while (n != NULL && n != D_800CEA40->list);
    }
    return (s16)(p - buf);
}
