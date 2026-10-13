#include "ctx.h"

typedef struct Node {
    u32 w0;
    struct Node *next;
    s32 id;
    s32 f0c;
    s32 f10;
    s32 f14;
    s32 f18;
    s32 f1c;
    s32 f20;
    s32 f24;
    s32 f28;
    s32 f2c;
    volatile s32 flags30;
    s32 f34;
    s32 f38;
    s32 f3c;
    s32 f40;
    u32 f44;
    s32 f48;
    s32 flags4c;
} Node;

typedef struct {
    s32 w0;
    s32 f4;
    s32 f8;
    s32 fc;
    s32 w10;
    s32 w14;
} Ent24;

typedef struct {
    u8 pad[0x28];
    void (*f28)(s32, s32);
    void (*f2c)(s32, s32);
    void (*f30)(s32, s32);
    void (*f34)(s32, s32);
    void (*f38)(s32, u32);
    s32 (*f3c)(s32);
    void (*f40)(s32, s32);
    s32 (*f44)(s32);
    s32 (*f48)(void);
} SndApi;

extern SndApi *D_800FE860;
extern s32 D_800D33F0;
extern Node *D_800D33E8;
extern s32 D_800D33F8;
extern s32 D_800FA840;
extern Node **D_800FA854;
extern Ent24 D_800FA860[];

void func_800B7BE0(void)
{
    Node *n;
    s32 i;
    s32 now;
    s32 dt;
    s32 cnt;
    s32 v;
    u32 pos;
    u32 cur;

    if (D_800D33F0 != 0) {
        now = D_800FE860->f48();
        n = D_800D33E8;
        D_800D33F8 = 0;
        while (n != NULL) {
            i = now - n->f14;
            n->f14 = now;
            D_800D33F8++;
            if ((n->flags30 & 1) && n->id == -1) {
                v = n->flags30 & 2;
                pos = n->f44 + i * n->f48;
                n->f44 = pos;
                if (v) {
                    s32 len;
                    len = (D_800FA860[n->f18].fc - D_800FA860[n->f18].f8) << 8;
                    while (n->f44 >= (u32)(D_800FA860[n->f18].fc << 8) && len != 0) {
                        n->f44 -= len;
                    }
                } else {
                    s32 len;
                    len = (D_800FA860[n->f18].fc - D_800FA860[n->f18].f8) << 8;
                    if (n->f44 >= (u32)(D_800FA860[n->f18].f4 << 8)) {
                        if (D_800FA860[n->f18].f8 != D_800FA860[n->f18].fc) {
                            n->flags30 |= 2;
                            cur = n->f44;
                            while (cur >= (u32)(D_800FA860[n->f18].fc << 8)) {
                                cur -= len;
                                n->f44 = cur;
                            }
                        } else {
                            n->flags4c |= 1;
                        }
                    }
                }
            }
            n = n->next;
        }

        for (n = D_800D33E8; n != NULL; n = n->next) {
            if (n->f34 != 0) {
                n->f38 += n->f34;
                v = n->f38 / 0x10000;
                n->f24 = v;
                if (v < 0) {
                    n->f24 = 0;
                    n->flags4c |= 1;
                } else {
                    if (v >= 0x80) {
                        n->f24 = 0x7F;
                    }
                    if (n->id != -1) {
                        D_800FE860->f30(n->id, n->f24);
                    }
                }
            }
        }

        for (i = 0; i < D_800FA840; i++) {
            n = D_800FA854[i];
            if (n != NULL) {
                v = D_800FE860->f44(i);
                n->flags30 = v;
                if (!(v & 1)) {
                    n->flags4c |= 1;
                }
            }
        }

        for (n = D_800D33E8; n != NULL; n = n->next) {
            if (n->flags4c & 1) {
                if (n->id != -1) {
                    D_800FE860->f40(n->id, 0);
                    D_800FA854[n->id] = NULL;
                    n->id = -1;
                }
                n->flags4c |= 2;
            }
        }

        i = 0;
        for (n = D_800D33E8; n != NULL; n = n->next) {
            n->f0c = 0;
            if (n->flags4c & 1) {
            } else if (n->flags4c & 2) {
            } else if (i < D_800FA840) {
                n->f0c = 1;
                i++;
            }
        }

        for (i = 0; i < D_800FA840; i++) {
            if (D_800FA854[i] != NULL && D_800FA854[i]->f0c == 0) {
                D_800FA854[i]->f44 = D_800FE860->f3c(i) << 8;
                D_800FA854[i]->id = -1;
                D_800FA854[i] = NULL;
                D_800FE860->f40(i, 0);
            }
        }

        for (n = D_800D33E8; n != NULL; n = n->next) {
            if (n->f0c != 0 && n->id == -1) {
                for (i = 0; i < D_800FA840; i++) {
                    if (D_800FA854[i] == NULL) {
                        break;
                    }
                }
                if (i != D_800FA840) {
                    D_800FA854[i] = n;
                    n->id = i;
                    n->flags30 = 1;
                    D_800FE860->f28(i, n->f18);
                    D_800FE860->f2c(i, n->f1c);
                    D_800FE860->f30(i, n->f24);
                    D_800FE860->f34(i, n->f28);
                    D_800FE860->f38(i, n->f44 >> 8);
                    D_800FE860->f40(i, n->flags30);
                }
            }
        }
    }
}
