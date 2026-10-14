/* The system layer: the game's input word, controllers, the scheduler, the heap, DMA, replays and maths. */
#ifndef STRUCTS_SYSTEM_H
#define STRUCTS_SYSTEM_H

#include "common.h"

/* The game's input word (gPadInput, Rider.input), which main_convert_pad_input builds from an OSContPad. The stick
   sets a direction bit and its magnitude (0-15) in the top two nibbles. */
#define INPUT_STICK_UP      0x00000001
#define INPUT_STICK_DOWN    0x00000002
#define INPUT_STICK_LEFT    0x00000004
#define INPUT_STICK_RIGHT   0x00000008
#define INPUT_A             0x00000010
#define INPUT_B             0x00000020
#define INPUT_START         0x00000040
#define INPUT_C_UP          0x00000080
#define INPUT_C_DOWN        0x00000100
#define INPUT_C_LEFT        0x00000200
#define INPUT_C_RIGHT       0x00000400
#define INPUT_D_UP          0x00000800
#define INPUT_D_DOWN        0x00001000
#define INPUT_D_LEFT        0x00002000
#define INPUT_D_RIGHT       0x00004000
#define INPUT_L             0x00008000
#define INPUT_R             0x00010000
#define INPUT_Z             0x00020000
#define INPUT_STICK_X_SHIFT 24          /* magnitude of the stick's x, 0-15 */
#define INPUT_STICK_Y_SHIFT 28          /* magnitude of the stick's y, 0-15 */

/* Game modes (gGame.mode). 3, 4 and 11 are not identified. */
#define GAME_MODE_EXHIBITION    0
#define GAME_MODE_SEASON        1
#define GAME_MODE_SOCCER        2
#define GAME_MODE_DESERT        5
#define GAME_MODE_EXCITE_3D     6
#define GAME_MODE_HILL_CLIMB    7
#define GAME_MODE_TIME_TRIAL    8
#define GAME_MODE_TUTORIAL      9
#define GAME_MODE_STUNT_COURSE  10
#define GAME_MODE_NES           12      /* Original Excitebike */
#define GAME_MODE_TEST_RIDE     13      /* the editor's test ride */
#define GAME_MODE_TEST_RIDE_2   14

/* Screen fades (gGame.fadeState; gGame.fadeLevel 0-255 is how far along). */
#define FADE_OUT    0
#define FADE_SWAP   1       /* faded out: the screen changes */
#define FADE_IN     2
#define FADE_DONE   3

/* Threads started by game_boot and idle_thread (unnamed/s1_80073200.c): ids and priorities. */
#define THREAD_ID_IDLE          1
#define THREAD_ID_MAIN          3
#define THREAD_PRI_IDLE_START   8       /* then OS_PRIORITY_IDLE once the main thread runs */
#define THREAD_PRI_MAIN         10

/* The scheduler (unnamed/sched.c). Messages on its own event queue (sSchedSysEventQueue): */
#define SCHED_MSG_RETRACE       1       /* VI retrace (osViSetEvent) */
#define SCHED_MSG_RDP_DONE      2       /* OS_EVENT_DP: the RDP finished the task, show its framebuffer */
#define SCHED_MSG_PRENMI        3       /* OS_EVENT_PRENMI: the reset button was pressed */
#define SCHED_MSG_FAULT         4       /* OS_EVENT_FAULT, OS_EVENT_CPU_BREAK: show the main thread's registers */
#define SCHED_MAX_CLIENTS       4       /* queues told of each retrace (sched_add_client) */
/* Messages the clients receive (an s16 the message points at): */
#define SCHED_CLIENT_RETRACE    1       /* sSchedRetraceMsg, each retrace */
#define SCHED_CLIENT_GFX_DONE   2       /* the message of a graphics task, once its framebuffer is shown */

/* VI modes (gViMode, gViModeRequest); sViModeWidths and sViModeHeights give the sizes. */
#define VI_MODE_320X240         0
#define VI_MODE_512X240         1
#define VI_MODE_640X480         2       /* interlaced: libultra's Hpf1 mode unchanged */
#define FB_320X240_SIZE         0x25800 /* bytes of a 320x240 framebuffer of 16-bit pixels */
#define FAULT_FB_WIDTH          0x140   /* the register dump draws in VI_MODE_320X240 */

/* Controllers, Rumble Paks and EEPROM over gSiMesgQueue (unnamed/s0_8000CD00.c). gRumbleTimers count frames down
   while a motor runs. */
#define RUMBLE_OFF              (-1)    /* a rumble_start duration that stops the motor */

/* Maths (unnamed/s0_80011490.c). Angles are 0x10000 per turn (s16 or s32) and the tables take an angle >> 4:
   gCosTable (0x1000 entries) is the cosine with 1.0 = 0x4000, and 0x400 entries (a quarter turn) back the sine.
   mtx3i_* matrices are s32[9], row by row, in 2.14 fixed point (1.0 = 0x4000); mtx3f_* the same in f32; mtx_* work
   on the N64 Mtx (16.16: the s16 integer halves, then the u16 fractions). Quaternions are f32 (w, x, y, z). */
#define ANGLE_90                0x4000
#define ANGLE_180               0x8000
#define ANGLE_270               0xC000
#define COS_IDX(i)              gCosTable[(i) & 0xFFF]              /* i: an angle >> 4 */
#define SIN_IDX(i)              gCosTable[((i) - 0x400) & 0xFFF]
#define FIX14_ONE               0x4000          /* 1.0 in 2.14 fixed point */
#define FIX14_ONE_F             16384.0f
#define FIX14_TO_FLOAT          6.1035156e-05f  /* 1.0f / 0x4000 */
#define MATH_PI_F               3.1415927f
#define MATH_HALF_PI_F          1.5707964f
/* The R250 generator (sR250Buffer): each new byte is the byte 250 back xor the byte 147 back. */
#define R250_SIZE               250
#define R250_LAG                147

/* The heap (unnamed/lib_800BC3D0.c): blocks of 16-byte units, each starting with a one-unit header {next, prev,
   size in units, check}. A used block's next is its owner (__FILE__), its prev (line << 16) | serial | MEM_USED. */
#define MEM_UNIT_SHIFT          4
#define MEM_UNIT_MASK           0xF
#define MEM_CHECK_SUM           0xB562439F  /* next + prev + size + check of every header */
#define MEM_USED                1           /* in a header's prev word */
#define MEM_SERIAL_MASK         0xFFFE      /* the allocation serial in a used header's prev word, counting by 2 */
#define MEM_ALLOC_TOP           1           /* mem_alloc_dbg flag: take the last block that fits, from its top */

/* PI DMA requests (unnamed/lib_800BEC20.c): sDmaRequests, slots of 19 words {state, a message queue and its
   message, the OSIoMesg, then dma_start's handle, RDRAM address, device address, size and direction}. */
#define DMA_SLOT_COUNT          8
#define DMA_SLOT_SIZE           0x4C
#define DMA_SLOT_WORDS          19
#define DMA_FREE                0
#define DMA_RUNNING             1
#define DMA_QUEUED              2       /* waits for dma_update to start it */
#define DMA_DONE                3

/* SRAM (unnamed/lib_800BF3C0.c): 32 KB at PI_DOM2_ADDR2, with these PI timings. */
#define SRAM_SIZE               0x8000
#define SRAM_LATENCY            5
#define SRAM_PULSE              0x0C
#define SRAM_PAGE_SIZE          0x0D
#define SRAM_REL_DURATION       2

/* Replays and Controller Pak ghosts (unnamed/replay.c): the magic byte, a 24-bit checksum of everything after the
   first four bytes (calc_checksum24), the 16-byte setup (players, CPU riders, track, laps, then a skin byte and a
   rider byte per racer), a 24-bit frame count, then for each block of 8 frames the bit-packed changes of each
   player's input. Numbers of several bytes are little-endian. */
#define REPLAY_MAGIC            0x7B
#define REPLAY_OFS_SETUP        4       /* the setup; the checksum covers the bytes from here on */
#define REPLAY_OFS_TRACK        6
#define REPLAY_OFS_LAPS         7
#define REPLAY_OFS_FRAMES       0x14    /* the frame count */
#define REPLAY_MAX_RACERS       6
#define REPLAY_BLOCK_FRAMES     8
#define REPLAY_GHOST_SIZE       0x2800  /* a time-trial ghost on the Controller Pak */
#define REPLAY_RIDER_GHOST      0x80    /* in a rider byte: this racer is the ghost (gGhostSlot) */
/* The buttons a replay keeps, as bits of its button byte (the bit number is what the stream stores). */
#define REPLAY_BTN_A            0x01
#define REPLAY_BTN_B            0x02
#define REPLAY_BTN_Z            0x04
#define REPLAY_BTN_C_DOWN       0x08
#define REPLAY_BTN_R            0x10
#define REPLAY_NUM_BUTTONS      5

/* The save image (gSaveData, SaveData). */
#define SAVE_MAGIC              0x5748FB3A
#define SAVE_CHECKSUM_XOR       0xFCB1121D  /* the checksum is the sum of the other words, xor this */
#define SAVE_WORDS              0x200       /* words the checksum covers (the first 0x800 bytes) */
/* save_write's sections: what has changed. */
#define SAVE_SECTION_NES_TRACK  0       /* the NES design track */
#define SAVE_SECTION_PROGRESS   1       /* season, custom cup and tutorial progress */
#define SAVE_SECTION_RECORDS    2       /* a track's records; the second argument is the track */
#define SAVE_SECTION_USER_TRACK 4
#define SAVE_SECTION_OPTIONS    5

#endif
