#include "game.h"

void cont_finish_read(void)
{
    s32 buf[2];
    s32 i;
    s32 v;

    if (gContReadPending != 0) {
        osRecvMesg(&gSiMesgQueue, buf, 1);
        for (i = 0; i < 8; i++) {
            osRecvMesg(&gSiMesgQueue, buf, 0);
        }
        for (i = 0; i < 4; i++) {
            if (gRumbleResetRequest != 0) {
                osMotorInit(&gSiMesgQueue, &gRumblePfs[i], i);
                __osMotorAccess(&gRumblePfs[i], 0);
                gRumbleTimers[i] = 0;
                gRumbleActiveMask &= ~(1 << i);
            }
            if (gRumblePakMask & (1 << i)) {
                if (gRumbleActiveMask & (1 << i)) {
                    v = gRumbleTimers[i];
                    if (v == 0) {
                        __osMotorAccess(&gRumblePfs[i], 0);
                        gRumbleTimers[i] = 0;
                        gRumbleActiveMask &= ~(1 << i);
                    }
                } else {
                    if (gRumbleTimers[i] != 0) {
                        __osMotorAccess(&gRumblePfs[i], 1);
                        gRumbleActiveMask |= 1 << i;
                    }
                }
                if (gRumbleTimers[i] != 0) {
                    gRumbleTimers[i]--;
                }
            }
        }
        gRumbleResetRequest = 0;
        gContReadPending = 0;
        if (D_800C2920 != 0) {
            gZBuffer = 0x80010000;
        }
    }
}

typedef struct { u16 id; u8 a; u8 b; u8 flag; u8 pad; } Sel;
typedef struct { u16 id; u8 a; u8 b; } Out;

extern Sel sContPadsRaw[];
extern Out gContPads[];

void cont_read_pads(void)
{
    u16 i;
    u16 cnt;
    u32 started;
    u8 bit;
    u8 nv;
    u8 old;
    Sel *e;

    if (gContReadPending != 0) {
        return;
    }
    cnt = 0;
    osContGetReadData(sContPadsRaw);
    started = 0;
    sContStickX = sContPadsRaw[i].a;
    sContStickY = sContPadsRaw[i].b;
    i = 0;
    do {
        old = gContMask;
        bit = (u8)(1 << i);
        nv = old | bit;
        gContMask = nv;
        e = &sContPadsRaw[i];
        if (e->flag != 0) {
            gContMask = nv & ~bit;
        } else {
            if (started == 0) {
                gContFirst = (u8)i;
                started = 1;
            }
            sContButtons[i] = e->id;
            gContPads[i].id = e->id;
            gContPads[i].a = e->a;
            gContPads[i].b = e->b;
            cnt = cnt + 1;
            if (cnt == 4) {
                return;
            }
        }
        i = i + 1;
    } while (i < 4);
}

int cont_init(void)
{
    int r;

    osCreateMesgQueue(&gSiMesgQueue, sSiMesgBuf, 4);
    osSetEventMesg(5, &gSiMesgQueue, 1);
    r = osContInit(&gSiMesgQueue, gContMask_2, gContStatus);
    if (r != 0) {
        return r;
    }
    pak_probe_all();
    gContReadPending = 0;
    return 0;
}

void start_controller_read(void) {
    if (gContReadPending == 0) {
        osContStartReadData(&gSiMesgQueue);
        gContReadPending = 1;
    }
}

void rumble_start(u32 a0, s32 a1)
{
    if ((gRumblePakMask & (1u << a0)) != 0) {
        gRumbleTimers[a0] = a1 + 1;
    }
}

void rumble_stop_all(void)
{
    s32 i;
    u32 bit;

    cont_finish_read();
    for (i = 0; i < 4; i++) {
        if ((gRumblePakMask & (1u << i)) != 0) {
            bit = 1u << i;
            osMotorInit(&gSiMesgQueue, gRumblePfs_2 + i * 0x68, i);
            __osMotorAccess(gRumblePfs_2 + i * 0x68, 0);
            gRumbleTimers[i] = 0;
            gRumbleActiveMask = gRumbleActiveMask & ~bit;
        }
    }
}

void eeprom_write_verify(s32 p1, u8 *p2, s32 p3)
{
    u8 loc[8];
    s32 i;
    s32 j;
    s32 k;
    s32 base;
    s32 w;

    cont_finish_read();
    osWritebackDCache(p2, p3 << 3);
    i = 0;
    if (p3 > 0) {
        do {
            w = p1 + i;
            base = i * 8;
            do {
                osEepromWrite(&gSiMesgQueue, (u8)w, p2 + base);
                for (j = 0; j < 8; j++) {
                    loc[j] = p2[base + j] ^ 0xAA;
                }
                osEepromRead(&gSiMesgQueue, (u8)w, loc);
                osInvalDCache(0, 0x400000);
                for (k = 0; k < 8; k++) {
                    if (loc[k] != p2[base + k]) {
                        break;
                    }
                }
            } while (k != 8);
            i++;
        } while (i < p3);
    }
}
