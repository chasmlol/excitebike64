/* Excitebike 64 (N64): declarations shared by every C file of the decompilation. */
#ifndef CTX_H
#define CTX_H

/* libultra: types (u8..s64, f32, f64), OS structs (OSThread, OSMesgQueue, OSPfs...), and the F3DEX2 display
   list macros (gDPPipeSync, gDPSetPrimColor, gDPFillRectangle, gSPVertex, ...). The game's display lists use F3DEX2 command
   bytes (0xDE display list, 0xD7 texture, 0xDF end), so the build defines F3DEX_GBI_2. */
#include <ultra64.h>

#ifndef NULL
#define NULL 0
#endif
#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

typedef struct {
    f32 x, y, z;
} Vec3f;

#endif
