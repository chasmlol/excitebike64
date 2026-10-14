#include "game.h"

/* Controllers, Rumble Paks and EEPROM, all over one SI message queue (gSiMesgQueue). */

/* Finishes the controller read start_controller_read began (if one is pending): waits for the SI and drains the
   queue, then runs the Rumble Paks: after gRumbleResetRequest every motor is initialized again and stopped; a motor
   starts when its timer is set and stops when the timer reaches 0; the timers count down here, once per read. */
void cont_finish_read(void)
{
    s32 msgs[2];
    s32 i;
    s32 timer;

    if (gContReadPending != 0) {
        osRecvMesg(&gSiMesgQueue, msgs, OS_MESG_BLOCK);
        for (i = 0; i < 8; i++) {
            osRecvMesg(&gSiMesgQueue, msgs, OS_MESG_NOBLOCK);
        }
        for (i = 0; i < MAXCONTROLLERS; i++) {
            if (gRumbleResetRequest != 0) {
                osMotorInit(&gSiMesgQueue, &gRumblePfs[i], i);
                __osMotorAccess(&gRumblePfs[i], 0);
                gRumbleTimers[i] = 0;
                gRumbleActiveMask &= ~(1 << i);
            }
            if (gRumblePakMask & (1 << i)) {
                if (gRumbleActiveMask & (1 << i)) {
                    timer = gRumbleTimers[i];
                    if (timer == 0) {
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
        if (D_800C2920 != 0) {      /* never set */
            gZBuffer = 0x80010000;
        }
    }
}

/* libultra's OSContPad, as osContGetReadData fills sContPadsRaw. */
typedef struct {
    u16 button;
    u8 stickX;
    u8 stickY;
    u8 error;
    u8 pad;
} ContPadRaw;

/* gContPads: the last read of each connected controller. */
typedef struct {
    u16 button;
    u8 stickX;
    u8 stickY;
} ContPad;

extern ContPadRaw sContPadsRaw[];
extern ContPad gContPads[];

/* Takes the data of the finished controller read: gContMask gets a bit per controller that answered, gContFirst the
   first of them, and gContPads and sContButtons their buttons and stick. */
void cont_read_pads(void)
{
    u16 i;
    u16 count;
    u32 foundFirst;
    u8 bit;
    u8 maskWithBit;
    u8 mask;
    ContPadRaw *pad;

    if (gContReadPending != 0) {
        return;
    }
    count = 0;
    osContGetReadData(sContPadsRaw);
    foundFirst = 0;
    /* A bug: i is read here before it is set. */
    sContStickX = sContPadsRaw[i].stickX;
    sContStickY = sContPadsRaw[i].stickY;
    i = 0;
    do {
        mask = gContMask;
        bit = (u8)(1 << i);
        maskWithBit = mask | bit;
        gContMask = maskWithBit;
        pad = &sContPadsRaw[i];
        if (pad->error != 0) {
            gContMask = maskWithBit & ~bit;
        } else {
            if (foundFirst == 0) {
                gContFirst = (u8)i;
                foundFirst = 1;
            }
            sContButtons[i] = pad->button;
            gContPads[i].button = pad->button;
            gContPads[i].stickX = pad->stickX;
            gContPads[i].stickY = pad->stickY;
            count = count + 1;
            if (count == MAXCONTROLLERS) {
                return;
            }
        }
        i = i + 1;
    } while (i < MAXCONTROLLERS);
}

/* Creates the SI queue, initializes the controllers (gContMask, gContStatus) and probes their paks
   (pak_probe_all). Returns osContInit's error, or 0. */
int cont_init(void)
{
    int err;

    osCreateMesgQueue(&gSiMesgQueue, sSiMesgBuf, 4);
    osSetEventMesg(OS_EVENT_SI, &gSiMesgQueue, 1);
    err = osContInit(&gSiMesgQueue, gContMask_2, gContStatus);
    if (err != 0) {
        return err;
    }
    pak_probe_all();
    gContReadPending = 0;
    return 0;
}

/* Starts reading the controllers unless a read is already pending; cont_finish_read completes it. */
void start_controller_read(void) {
    if (gContReadPending == 0) {
        osContStartReadData(&gSiMesgQueue);
        gContReadPending = 1;
    }
}

/* Runs the Rumble Pak of a controller for the given number of reads (RUMBLE_OFF stops it), if it has one. */
void rumble_start(u32 cont, s32 frames)
{
    if ((gRumblePakMask & (1u << cont)) != 0) {
        gRumbleTimers[cont] = frames + 1;
    }
}

/* Waits for the pending controller read and stops every Rumble Pak at once. */
void rumble_stop_all(void)
{
    s32 i;
    u32 bit;

    cont_finish_read();
    for (i = 0; i < MAXCONTROLLERS; i++) {
        if ((gRumblePakMask & (1u << i)) != 0) {
            bit = 1u << i;
            /* &gRumblePfs[i]: 0x68 is sizeof(OSPfs) */
            osMotorInit(&gSiMesgQueue, gRumblePfs_2 + i * 0x68, i);
            __osMotorAccess(gRumblePfs_2 + i * 0x68, 0);
            gRumbleTimers[i] = 0;
            gRumbleActiveMask = gRumbleActiveMask & ~bit;
        }
    }
}

/* Writes numBlocks 8-byte EEPROM blocks from src, starting at block firstBlock, writing each block again until
   reading it back gives the same bytes. The read buffer is spoiled first (xor 0xAA) so a failed read cannot pass. */
void eeprom_write_verify(s32 firstBlock, u8 *src, s32 numBlocks)
{
    u8 readBack[EEPROM_BLOCK_SIZE];
    s32 i;
    s32 j;
    s32 k;
    s32 offset;
    s32 block;

    cont_finish_read();
    osWritebackDCache(src, numBlocks << 3);    /* numBlocks * EEPROM_BLOCK_SIZE */
    i = 0;
    if (numBlocks > 0) {
        do {
            block = firstBlock + i;
            offset = i * EEPROM_BLOCK_SIZE;
            do {
                osEepromWrite(&gSiMesgQueue, (u8)block, src + offset);
                for (j = 0; j < EEPROM_BLOCK_SIZE; j++) {
                    readBack[j] = src[offset + j] ^ 0xAA;
                }
                osEepromRead(&gSiMesgQueue, (u8)block, readBack);
                osInvalDCache(0, 0x400000);     /* the first 4 MB of RDRAM */
                for (k = 0; k < EEPROM_BLOCK_SIZE; k++) {
                    if (readBack[k] != src[offset + k]) {
                        break;
                    }
                }
            } while (k != EEPROM_BLOCK_SIZE);
            i++;
        } while (i < numBlocks);
    }
}
