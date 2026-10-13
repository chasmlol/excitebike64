#include "ctx.h"

typedef struct { u8 r0; u8 key[7]; } Rec;

extern Rec D_800CCF20[13];
extern Rec D_800CCF88[8];
extern Rec *D_800F2110[];
extern Rec *D_800F2128[];

static inline u8 cmp(Rec *a, Rec *b)
{
    s32 k;

    for (k = 0; k < 7; k++) {
        if (a->key[k] > b->key[k]) {
            return 0;
        }
        if (a->key[k] < b->key[k]) {
            return 1;
        }
    }
    return 0;
}

void func_8006F620(void)
{
    u8 flag;
    s32 i;
    u32 m;
    s32 off;
    Rec *t, *pa, *pb, *p1;
    Rec tmp;

    t = D_800CCF20;
    p1 = &t[1];
    do {
        flag = 0;
        i = 0;
        pa = t;
        off = 8;
        pb = p1;
        for (; i < sizeof(D_800CCF20) / sizeof(Rec) - 2; i++) {
            if (cmp(pa, pb)) {
                flag = 1;
                tmp = *pa;
                *pa = *(Rec *)(off + (u32)t);
                *(Rec *)(off + (u32)t) = tmp;
            }
            pa++;
            off += 8;
            pb++;
        }
    } while (flag);
    for (i = 0; i < 5; i++) {
        for (m = 0; m < sizeof(D_800CCF20) / sizeof(Rec); m++) {
            if (D_800CCF20[m].key[0] <= i + 2) {
                break;
            }
        }
        D_800F2110[i] = (Rec *)(m * 8 + (u32)D_800CCF20);
    }

    t = D_800CCF88;
    p1 = &t[1];
    do {
        flag = 0;
        i = 0;
        pa = t;
        off = 8;
        pb = p1;
        for (; i < sizeof(D_800CCF88) / sizeof(Rec) - 2; i++) {
            if (cmp(pa, pb)) {
                flag = 1;
                tmp = *pa;
                *pa = *(Rec *)(off + (u32)t);
                *(Rec *)(off + (u32)t) = tmp;
            }
            pa++;
            off += 8;
            pb++;
        }
    } while (flag);
    for (i = 0; i < 5; i++) {
        for (m = 0; m < sizeof(D_800CCF88) / sizeof(Rec); m++) {
            if (D_800CCF88[m].key[0] <= i + 2) {
                break;
            }
        }
        D_800F2128[i] = (Rec *)(m * 8 + (u32)D_800CCF88);
    }
}
