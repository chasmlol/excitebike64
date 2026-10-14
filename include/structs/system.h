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

#endif
