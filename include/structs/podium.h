/* podium.c: the end-of-season ceremony plays back recorded clips: the camera path and, per rider, the poses of the
   bike and rider and a prop. */
#ifndef STRUCTS_PODIUM_H
#define STRUCTS_PODIUM_H

#include "common.h"

#define PODIUM_NUM_PLACES       3
#define PODIUM_HEAD_BONE        2       /* the helmet sits on this bone during the first pose clip */
#define PODIUM_POSE_HEADER      0x14    /* a pose clip's frames follow this much header */
#define PODIUM_CLIP_HEADER      0x1C    /* ... a camera or prop clip's */

/* A clip resource. podium_init points frames past the header. */
typedef struct PodiumClip {
    /* 0x00 */ s32 unk_00;
    /* 0x04 */ s32 numFrames;
    /* 0x08 */ s32 unk_08[2];
    /* 0x10 */ void *frames;        /* PodiumCamFrame, PodiumPoseFrame or PodiumPropFrame */
} PodiumClip;

/* sPodiumCameraClips: played one after the other, up to an entry with no resource. */
typedef struct PodiumCameraClip {
    /* 0x00 */ s32 *resId;
    /* 0x04 */ PodiumClip *clip;
} PodiumCameraClip; /* size 0x8 */

/* sPodiumRiderClips[place]: the pose clips one after the other; the prop clips (the helmet, taken off) follow the
   first pose clip. Rider n stands on place n. */
typedef struct PodiumRiderClip {
    /* 0x00 */ s32 *poseResId;
    /* 0x04 */ s32 *propResId;
    /* 0x08 */ PodiumClip *pose;
    /* 0x0C */ PodiumClip *prop;
} PodiumRiderClip; /* size 0x10 */

typedef struct PodiumCamFrame {
    /* 0x00 */ s16 x, y, z;
    /* 0x06 */ s16 pitch, roll, yaw;
} PodiumCamFrame; /* size 0xC */

typedef struct PodiumPoseFrame {
    /* 0x00 */ s16 x, y, z;
    /* 0x06 */ Rgb rot[15];         /* the angles of the 15 bones (>> 8) */
    /* 0x33 */ u8 pad;
} PodiumPoseFrame; /* size 0x34 */

typedef struct PodiumPropFrame {
    /* 0x00 */ s16 x, y, z;
    /* 0x06 */ u8 rot[3];
    /* 0x09 */ u8 pad[3];
} PodiumPropFrame; /* size 0xC */

#endif
