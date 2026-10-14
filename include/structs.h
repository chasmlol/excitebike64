/* Types several source files of the game use. */
#ifndef STRUCTS_H
#define STRUCTS_H

#include "common.h"

/* Declared here for the prototypes and globals; most are defined in the source file that uses them. */
typedef struct Dyn Dyn;
typedef struct MtxS_2 MtxS_2;
typedef struct Res_3 Res_3;
typedef struct List_6 List_6;
typedef struct Ent_35 Ent_35;
typedef struct Pt_4 Pt_4;
typedef struct Obj_40 Obj_40;
typedef struct Obj_43 Obj_43;
typedef struct Spawn Spawn;
typedef struct Obj_55 Obj_55;
typedef struct Obj_58 Obj_58;
typedef struct Blk_29 Blk_29;

typedef struct { u8 r, g, b; } Rgb;
typedef struct { s32 pad0; u32 *tbl; } ResFile;
typedef struct { s32 w[4]; } Blk;
typedef struct { u8 pad[0xA0]; } Rec;
typedef struct { s32 a; s32 b; } Pair;

typedef struct {
    s32 w[16];
} Blk64;

typedef struct {
    u8 pad[0x68];
} Rec68;

typedef struct { s32 x; s32 y; } Pair_2;

typedef struct {
    s32 x;
    s32 y;
    s32 w;
    s32 h;
} Rect;

typedef struct { s16 x; s16 y; } Pos;

typedef struct {
    s32 x, y, z;
} Vec3i;

typedef struct {
    u8 pad[0x2C];
    s32 f2C;
} Cfg;

typedef struct { u8 w, h, pad, pal; u8 *addr; } Img;
typedef struct { s32 x, y, z, w; } V4;

typedef struct Vec {
    s32 x, y, z;
} Vec;

typedef struct { s32 m[9]; } Mtx3;

typedef struct Rec_2 {
    u8 pad[0xA0];
} Rec_2;

typedef struct MtxS {
    s16 i[4][4];
    u16 f[4][4];
} MtxS;

typedef struct {
    u8 pad[0x1C];
} Ent1C;

typedef struct { u32 w[4]; } Pad16;

typedef struct {
    s32 key;
    s32 val;
} Pair_4;

typedef struct {
    f32 x, z;
} Start;

typedef struct { f32 x, y, z; } V3;

typedef struct { s32 a, b, c; } T3;

typedef struct {
    f32 x, y, z, w;
} Quat;

typedef struct { s32 a, b, c, d; } Quad;

typedef struct { u8 pad[0x2C]; s32 mode; } Game_2;

typedef struct {
    u32 w0, w1;
} Blk_2;

typedef struct { f32 q[4]; } Quat_2;

typedef struct { u8 b0, b1, b2, b3; } Pts;
typedef struct { u8 w; u8 h; u8 pad[2]; void *img; } Img_2;

typedef struct {
    u8 pad0[0x50];
    s32 w50;
} Track;

typedef struct { u16 a, b; } Pal_2;

typedef struct {
    s32 f0;
    u32 pal;            /* 0x04 */
    u8 img[1];          /* 0x08 */
} Pic;

typedef struct Block {
    struct Block *next;
    s32 unk4;
    s32 size;
    s32 check;
} Block;

typedef struct Q_3 {
    s32 *p0;
    s32 f4;
    s32 f8;
    s32 f0c;
    s32 f10;
    s32 *f14;
} Q_3;

/* The input word, controllers, scheduler, heap, DMA, replays and maths. */
#include "structs/system.h"

/* The structures of the track, its models and the effects drawn on it. */
#include "structs/model.h"
#include "structs/track.h"
#include "structs/camera.h"
#include "structs/particle.h"
#include "structs/snow.h"
#include "structs/decal.h"
#include "structs/sun.h"
#include "structs/desert.h"
#include "structs/shatter.h"
#include "structs/palmod.h"
#include "structs/map.h"
#include "structs/flash.h"
#include "structs/arned.h"
#include "structs/resman.h"

/* The racing line and the track editor. */
#include "structs/path.h"
#include "structs/editor.h"

/* The front end: fonts and text objects, the screens, the GUI kit, the race's texts, the Controller Pak screen. */
#include "structs/font.h"
#include "structs/shell.h"
#include "structs/gui.h"
#include "structs/hud.h"
#include "structs/pak.h"

/* Sound, the NES emulator and the files of the ROM. */
#include "structs/nes.h"
#include "structs/softmix.h"
#include "structs/sfx.h"
#include "structs/tunes.h"
#include "structs/snd.h"
#include "structs/file.h"

/* The game's setup, the race, the season and the podium. */
#include "structs/race.h"
#include "structs/game.h"
#include "structs/hacks.h"
#include "structs/podium.h"

/* The riders: the bike and rider simulation, the skeletons and animations, the ground surfaces under the wheels. */
#include "structs/surface.h"
#include "structs/rider.h"

#endif
