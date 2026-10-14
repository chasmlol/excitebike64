/* The camera: modes, per-view state and history. */
#ifndef STRUCTS_CAMERA_H
#define STRUCTS_CAMERA_H

#include "common.h"

struct Rider;

/* gCamSegFlags: the flags of the racing-line node under each view's camera. */
#define CAM_SEG_COVERED 0x40 /* a covered area: rain and snow stop */

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

/* Camera modes: Rider.camMode, the index into gCamModes. */
#define CAM_MODE_SOCCER        0  /* sweeps around the ball after a goal, else the chase camera */
#define CAM_MODE_CHASE_NEAR    1  /* 1-3: the modes the Camera button cycles through */
#define CAM_MODE_CHASE_MID     2
#define CAM_MODE_CHASE_FAR     3
#define CAM_MODE_SIDE_VIEW     4  /* Excite 3D */
#define CAM_MODE_HIGH_SIDE     5
#define CAM_MODE_HILL_CLIMB    6
#define CAM_MODE_STUNT_PIP     7  /* the stunt replay in the inset view */
#define CAM_MODE_CHASE_FAR_ALT 8
#define CAM_MODE_FALL_VIEW     9  /* watching the rider fall off a cliff */
#define CAM_MODE_CELEBRATION   10 /* circling the rider after the finish */
#define CAM_MODE_FIRST_PERSON  11
#define CAM_NUM_MODES          12

/* CamMode.flags */
#define CAM_FLAG_SNAP_IN  1 /* snap to the new mode's camera when entering it */
#define CAM_FLAG_SNAP_OUT 2 /* snap when leaving the mode */
#define CAM_FLAG_NO_CYCLE 8 /* the Camera button skips it */

#define CAM_HISTORY 16 /* samples of the camera history that cam_smooth_to averages */

/* An entry of gCamModes, indexed by the rider's camera mode (Rider.camMode). */
typedef struct CamMode {
    /* 0x00 */ void (*init)(void);
    /* 0x04 */ void (*update)(struct Rider *rider, CamOut *out);
    /* 0x08 */ void (*exit)(void);
    /* 0x0C */ u16 flags;   /* CAM_FLAG_* */
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

/* Flags of both kinds of zone */
#define CAM_ZONE_DEMO_ONLY 0x02 /* used only while gEvent.noPlayers is set */
#define CAM_ZONE_EARLY     0x40 /* also found from the alternative node (cam_find_zone's second search) */
/* Dolly zone flags */
#define CAM_DOLLY_MOVE     0x01 /* moves to (x1, y1, z1) across the zone */
#define CAM_DOLLY_RELATIVE 0x04 /* the point is relative to the rider */
#define CAM_DOLLY_FROM_CAM 0x08 /* the point is taken from the camera on entry */
#define CAM_DOLLY_FIXED    0x10 /* fixed angles: x1 the pitch, z1 the yaw */
#define CAM_DOLLY_BACKWARD 0x20 /* the offset runs backwards */

/* CamZoneState.flags */
#define CAM_ZONE_STATE_ON    1 /* the zone camera is on */
#define CAM_ZONE_STATE_IN    2 /* in a zone */
#define CAM_ZONE_STATE_NEW   4 /* entered a new zone */
#define CAM_ZONE_STATE_EARLY 8 /* found from the alternative node */

/* The camera zone of a view (gCamZoneState). */
typedef struct CamZoneState {
    /* 0x00 */ u16 flags;   /* CAM_ZONE_STATE_* */
    /* 0x02 */ s16 type;    /* 0 dolly, 1 follow */
    /* 0x04 */ union {
                   CamDollyZone *dolly;
                   CamFollowZone *follow;
               } zone;
} CamZoneState; /* size 0x8 */

#endif
