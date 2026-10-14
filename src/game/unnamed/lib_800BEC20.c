#include "game.h"

typedef struct Slot_18 {
    s32 w0, w1, w2, w3, w4, w5, w6, w7;
    u8 w8[4];
    s32 w9, w10, w11, w12, w13, w14, w15, w16, w17, w18;
} Slot_18;

s32 dma_start(s32 a0, s32 a1, s32 a2, s32 a3, s32 a4)
{
    s32 i;
    s32 off;
    s32 t;
    Slot_18 *base;
    Slot_18 *p;
    Slot_18 *e;

    dma_update();
    if (gDmaIdleCallback != 0) {
        gDmaIdleCallback();
    }
    i = 0;
    base = sDmaRequests;
    p = base;
    for (; i < 8; i++) {
        if (p->w0 == 0) {
            break;
        }
        p++;
    }
    if (i == 8) {
        return -2;
    }
    off = i * 76;
    e = (Slot_18 *)(off + (s32)base);
    e->w15 = a3;
    e->w16 = a2;
    e->w17 = a4;
    e->w14 = a0;
    e->w18 = a1;
    e->w8[2] = 0;
    t = (s32)&base->w1;
    e->w9 = off + t;
    e->w0 = 2;
    e->w10 = e->w15;
    e->w11 = e->w16;
    e->w12 = e->w17;
    return i;
}

typedef struct {
    s32 state;
    u8 f04[0x1C];
    u8 f20[2];
    u8 flag22;
    u8 pad23;
    void *p24;
    s32 f28, f2C, f30, f34;
    s32 f38, f3C, f40, f44, f48;
} Ent_107;

typedef void (*FnPtr)(s32, s32, s32);

extern Ent_107 sDmaRequests_2[];
extern FnPtr sDmaCopyFunc;

void dma_update(void)
{
    s32 i;

    sDmaPendingCount = 0;
    sDmaStartedCount = 0;
    for (i = 0; i < 8; i++) {
        if (sDmaRequests_2[i].state == 1) {
            if (osRecvMesg(sDmaRequests_2[i].f04, 0, 0) == 0) {
                sDmaRequests_2[i].state = 3;
            } else {
                sDmaPendingCount++;
                return;
            }
        } else if (sDmaRequests_2[i].state == 2) {
            if (sDmaCopyFunc == 0) {
                sDmaRequests_2[i].p24 = sDmaRequests_2[i].f04;
                sDmaStartedCount++;
                sDmaRequests_2[i].flag22 = 0;
                sDmaRequests_2[i].f28 = sDmaRequests_2[i].f3C;
                sDmaRequests_2[i].f2C = sDmaRequests_2[i].f40;
                sDmaRequests_2[i].f30 = sDmaRequests_2[i].f44;
                main_on_dma_start();
                if (dma_epi_start(sDmaRequests_2[i].f38, sDmaRequests_2[i].f20, sDmaRequests_2[i].f48) == 0) {
                    sDmaRequests_2[i].state = 1;
                    return;
                }
            } else {
                sDmaCopyFunc(sDmaRequests_2[i].f40, sDmaRequests_2[i].f3C, sDmaRequests_2[i].f44);
                sDmaRequests_2[i].state = 3;
            }
        }
    }
}

s32 dma_init(void)
{
    s32 i;
    u8 *b;
    s32 *z;
    u8 *q;
    u8 *p;

    osCreatePiManager(0x96, &sPiCmdQueue, sPiCmdMsgs, 8);
    i = 0;
    b = D_80100724;
    z = (s32 *)(b - 4);
    q = b + 0x18;
    p = b;
    do {
        osCreateMesgQueue(p, q, 1);
        *z = 0;
        z = (s32 *)((u8 *)z + 0x4C);
        q += 0x4C;
        i++;
        p += 0x4C;
    } while (i < 8);
    gCartRomHandle = dma_cart_rom_init();
    sSramHandle = (s32)sram_init();
    return 0;
}

s32 dma_get_status(s32 a0) {
    if (gDmaIdleCallback != NULL) {
        gDmaIdleCallback();
    }
    dma_update();
    return sDmaRequests[a0 * 19];
}

void dma_release(s32 a0)
{
    sDmaRequests[a0 * 0x13] = 0;
}

typedef struct {
    s32 state;
    u8 pad[0x48];
} Ent_108;

extern Ent_108 sDmaRequests_3[];

void dma_wait(s32 x) {
    do {
        if (gDmaIdleCallback != NULL) {
            gDmaIdleCallback();
        }
        dma_update();
    } while (sDmaRequests_3[x].state != 3 && sDmaRequests_3[x].state != 0);
    sDmaRequests_3[x].state = 0;
}
