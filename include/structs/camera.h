/* The camera: modes, per-view state and history. */
#ifndef STRUCTS_CAMERA_H
#define STRUCTS_CAMERA_H

#include "common.h"

/* A camera placement: what a mode's update produces, the 16 samples of history per view and their running sum.
   Position in mm, angles in 1/0x10000 of a turn. */
typedef struct CamOut {
    /* 0x00 */ s32 x;
    /* 0x04 */ s32 y;
    /* 0x08 */ s32 z;
    /* 0x0C */ s32 pitch;
    /* 0x10 */ s32 roll;
    /* 0x14 */ s32 yaw;
} CamOut; /* size 0x18 */

/* An entry of gCamModes, indexed by the rider's camera mode (rider+0x5AA). */
typedef struct CamMode {
    /* 0x00 */ void (*init)(void);
    /* 0x04 */ void (*update)(void *rider, CamOut *out);
    /* 0x08 */ void (*exit)(void);
    /* 0x0C */ u16 flags;   /* 1 snap when entering, 2 snap when leaving, 8 skipped by the Camera button */
    /* 0x0E */ u16 unk_0E;
} CamMode; /* size 0x10 */

/* The spring the chase camera's pitch follows, one per view (sCamPitchSprings). */
typedef struct CamPitchSpring {
    /* 0x00 */ u8 unk_00[0x20];
    /* 0x20 */ f32 pos;
    /* 0x24 */ f32 vel;
} CamPitchSpring; /* size 0x28 */

/* The track's camera zones, used by replays, demos and the credits: a range of path nodes [startNode, endNode)
   with its camera. Flags of both kinds: 2 used only while gEvent+0x3C is set, 0x40 starts a node early. */

/* A follow zone orbits the rider: distance, yaw (from the rider's heading) and pitch go from the start values to
   the start plus the sweep across the zone. */
typedef struct CamFollowZone {
    /* 0x00 */ s32 dist0;
    /* 0x04 */ u16 yaw0;
    /* 0x06 */ u16 pitch0;
    /* 0x08 */ s32 dist1;
    /* 0x0C */ s32 yawSweep;
    /* 0x10 */ s32 pitchSweep;
    /* 0x14 */ s16 startNode;
    /* 0x16 */ s16 endNode;
    /* 0x18 */ u32 flags;
} CamFollowZone; /* size 0x1C */

/* A dolly zone: a camera at a point that looks at the rider. Flag 1 moves it to (x1, y1, z1) across the zone.
   Flags 4 and 0x10 move it by an offset of y1 mm instead (sCamZoneOffset, 0x20 backwards): with 4 the point is
   relative to the rider and the offset points from it to the rider; with 0x10 the offset follows fixed angles, x1
   the pitch and z1 the yaw, which the camera keeps. Flag 8 takes the point from the camera on entry. */
typedef struct CamDollyZone {
    /* 0x00 */ s32 x;
    /* 0x04 */ s32 y;
    /* 0x08 */ s32 z;
    /* 0x0C */ s32 x1;
    /* 0x10 */ s32 y1;
    /* 0x14 */ s32 z1;
    /* 0x18 */ u32 flags;
    /* 0x1C */ s32 startNode;
    /* 0x20 */ s32 endNode;
} CamDollyZone; /* size 0x24 */

/* The camera zone of a view (gCamZoneState). */
typedef struct CamZoneState {
    /* 0x00 */ u16 flags;   /* 1 the zone camera is on, 2 in a zone, 4 entered a new zone, 8 found a node early */
    /* 0x02 */ s16 type;    /* 0 dolly, 1 follow */
    /* 0x04 */ union {
                   CamDollyZone *dolly;
                   CamFollowZone *follow;
               } zone;
} CamZoneState; /* size 0x8 */

#endif
