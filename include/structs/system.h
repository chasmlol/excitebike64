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

#endif
