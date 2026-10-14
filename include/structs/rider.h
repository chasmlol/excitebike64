/* The riders (gRiders, 8 x Rider): bike and rider simulation state (bikenew.c and the files around it), and what
   hangs off it: the two rigid bodies, the wheels, the engine, the race progress and the skeletons. Positions are in
   metres (f32) for the physics and in millimetres (s32) for drawing; angles in s32/s16 are 0x10000 per turn. */
#ifndef STRUCTS_RIDER_H
#define STRUCTS_RIDER_H

#include "common.h"
#include "structs/surface.h"

struct Rider;

/* A bone of a skeleton (skel.c): resources of 0x38-byte entries become these. */
typedef struct Bone {
    /* 0x00 */ s32 parent;
    /* 0x04 */ Vec3i pos;           /* relative to the parent */
    /* 0x10 */ Mtx3 rot;            /* local rotation, 1.0 = 0x4000 */
    /* 0x34 */ Vec3i worldPos;
    /* 0x40 */ Mtx3 worldRot;
    /* 0x64 */ u16 angles[3];
    /* 0x6A */ s16 baseAngles[3];
    /* 0x70 */ u8 unk_70[0x10];
} Bone; /* size 0x80 */

typedef struct BikeSkeleton {
    /* 0x000 */ Bone bones[10];     /* gBikeSkelTemplate */
} BikeSkeleton; /* size 0x500 */

typedef struct RiderSkeleton {
    /* 0x000 */ Bone bones[15];     /* gRiderSkelTemplate */
} RiderSkeleton; /* size 0x780 */

/* Where the rider's hands and feet go (the bike's grips and pegs) and how the hips turn (rider_update_pose,
   rider_apply_ik). */
typedef struct RiderIkTarget {
    /* 0x00 */ Vec3i pos;
    /* 0x0C */ Mtx3 rot;
} RiderIkTarget; /* size 0x30 */

typedef struct RiderIkTargets {
    /* 0x00 */ RiderIkTarget limb[4]; /* left and right hand, left and right foot */
    /* 0xC0 */ Mtx3 hips;
    /* 0xE4 */ s32 armTwist;        /* 0x100 = full */
} RiderIkTargets; /* size 0xE8 */

/* A keyframe animation resource (rider_anims_load). */
typedef struct AnimClip {
    /* 0x00 */ s32 bones;           /* a frame holds (bones * 3 + 7) / 2 halfwords */
    /* 0x04 */ s32 count;           /* frames */
    /* 0x08 */ s32 unk_08;
    /* 0x0C */ s32 unk_0C;
    /* 0x10 */ void *data;          /* the frames below, set when loaded */
    /* 0x14 */ u8 frames[4];        /* [count] */
} AnimClip;

/* A rider animation or trick (gRiderAnims, indexed by Rider.anim). */
typedef struct RiderAnim {
    /* 0x00 */ u32 flags;
    /* 0x04 */ s32 *bodyRes;        /* resource of the body clip */
    /* 0x08 */ s32 *rootRes;        /* resource of the root clip (bike offset and rotation), or NULL */
    /* 0x0C */ AnimClip *body;      /* 26 halfwords per frame */
    /* 0x10 */ AnimClip *root;      /* 12 bytes per frame */
    /* 0x14 */ void *inputGlyphs;   /* the button combination, as glyphs */
    /* 0x18 */ char *name;
    /* 0x1C */ s32 riderMask;       /* the riders who can do it */
    /* 0x20 */ s32 points;
    /* 0x24 */ s32 unk_24;
    /* 0x28 */ s32 holdFrames;
    /* 0x2C */ s32 unk_2C;
    /* 0x30 */ s32 unk_30;
    /* 0x34 */ s32 speed;
    /* 0x38 */ s32 announce;        /* announcer clip, -1 none */
} RiderAnim; /* size 0x3C */

/* A sphere of a body's bounding-sphere tree (collide.c), built from CollideSphereDesc entries into a 16000-byte
   pool. The tests use the body's up and forward rows only. */
typedef struct CollideNode {
    /* 0x00 */ u8 cached;           /* worldPos is up to date */
    /* 0x01 */ u8 unk_01[3];
    /* 0x04 */ Vec3f pos;           /* in the body's frame */
    /* 0x10 */ Vec3f worldPos;
    /* 0x1C */ f32 radius;
    /* 0x20 */ struct CollideNode *next;
    /* 0x24 */ struct CollideNode *child;
} CollideNode; /* size 0x28 */

/* A sphere tree description (sBikeSphereTreeDesc, sRiderSphereDesc): offsets to the first child and the next
   sibling in entries, 0 for none. */
typedef struct CollideSphereDesc {
    /* 0x00 */ s32 child;
    /* 0x04 */ s32 next;
    /* 0x08 */ f32 x;
    /* 0x0C */ f32 y;
    /* 0x10 */ f32 z;
    /* 0x14 */ f32 radius;
} CollideSphereDesc; /* size 0x18 */

/* A node of a ponytail's spring chain (ponytail.c): four per rider, anchored on the rider's head bone. */
typedef struct PonytailNode {
    /* 0x00 */ Vec3f pos;
    /* 0x0C */ Vec3f chainPos;      /* pos along the bone chain, for the bone matrices */
    /* 0x18 */ Vec3f vel;
    /* 0x24 */ Vec3f acc;
    /* 0x30 */ s32 unk_30;
    /* 0x34 */ s32 unk_34;
    /* 0x38 */ struct PonytailNode *prev;
    /* 0x3C */ struct PonytailNode *next;
    /* 0x40 */ f32 prevLen;         /* rest length to prev */
    /* 0x44 */ f32 nextLen;         /* rest length to next */
} PonytailNode; /* size 0x48 */

/* A rigid body: the bike, the thrown rider, the soccer ball (phys_body_step). */
typedef struct RigidBody {
    /* 0x000 */ Vec3f pos;
    /* 0x00C */ Vec3f vel;
    /* 0x018 */ Vec3f acc;
    /* 0x024 */ Vec3f smoothPos;    /* pos, extrapolated between the double steps of far CPU riders */
    /* 0x030 */ f32 quat[4];        /* w, x, y, z */
    /* 0x040 */ Vec3f angVel;
    /* 0x04C */ Vec3f angAcc;
    /* 0x058 */ Vec3f right;        /* rotation matrix rows (quat_to_mtx3f) */
    /* 0x064 */ Vec3f up;
    /* 0x070 */ Vec3f fwd;
    /* 0x07C */ CollideNode *tree;  /* bounding-sphere tree */
    /* 0x080 */ Vec3f contacts[20]; /* contact points: 11 for the bike, 8 for the rider */
    /* 0x170 */ f32 unk_170;
    /* 0x174 */ f32 mass;
    /* 0x178 */ f32 inertia;
    /* 0x17C */ u8 type;            /* 0 human bike, 1 CPU bike, 2 crashed bike, 3 rider, 4 ball */
    /* 0x17D */ u8 flags;
    /* 0x17E */ s8 hitTimer;        /* set after a collision, counts down */
    /* 0x17F */ u8 unk_17F;
    /* 0x180 */ struct Rider *owner;
} RigidBody; /* size 0x184 */

/* A wheel and its suspension (rear at 0x454, front at 0x4F8). */
typedef struct Wheel {
    /* 0x00 */ s32 groundArea;      /* gGroundArea of the last ground search */
    /* 0x04 */ s32 airTicks;        /* > 0: ticks in the air; <= 0: on the ground */
    /* 0x08 */ Vec3f force;         /* tire force */
    /* 0x14 */ f32 latForce;
    /* 0x18 */ f32 lonForce;        /* drive force minus brake torque */
    /* 0x1C */ f32 springForce;
    /* 0x20 */ f32 unk_20;
    /* 0x24 */ f32 suspLength;
    /* 0x28 */ f32 groundDist;      /* along the bike's up axis */
    /* 0x2C */ f32 suspDelta;
    /* 0x30 */ f32 bump;            /* bike_wheel_surface_bumps */
    /* 0x34 */ f32 unk_34;
    /* 0x38 */ f32 bumpNoise;
    /* 0x3C */ f32 unk_3C;
    /* 0x40 */ Vec3f contact;       /* contact point */
    /* 0x4C */ Vec3f contactOffset; /* contact point - bike position */
    /* 0x58 */ Vec3f contactDelta;  /* contact point - last contact point */
    /* 0x64 */ s16 brakeRamp;       /* 0..6 while braking */
    /* 0x66 */ s16 unk_66;
    /* 0x68 */ f32 brakeTorque;
    /* 0x6C */ Vec3f normal;        /* ground normal */
    /* 0x78 */ Vec3f latAxis;
    /* 0x84 */ Vec3f lonAxis;
    /* 0x90 */ Surface *surface;    /* gSurfaceTable entry */
    /* 0x94 */ s32 groundFlags;     /* gGroundSurface */
    /* 0x98 */ f32 spin;            /* wheel speed */
    /* 0x9C */ f32 slip;
    /* 0xA0 */ f32 grip;
} Wheel; /* size 0xA4 */

/* The engine (s0_800358C0.c): rpm, gearbox and turbo heat. */
typedef struct RiderEngine {
    /* 0x00 */ f32 rpm;
    /* 0x04 */ f32 heat;            /* turbo temperature, 0..12000 */
    /* 0x08 */ f32 torque;
    /* 0x0C */ s32 shiftFrame;      /* gRaceFrame of the last gear change */
    /* 0x10 */ u8 overheated;
    /* 0x11 */ u8 gear;
    /* 0x12 */ u8 throttle;
    /* 0x13 */ u8 unk_13;
} RiderEngine; /* size 0x14 */

/* Laps, places and times (event.c). */
typedef struct RiderRace {
    /* 0x00 */ s16 lap;             /* laps completed */
    /* 0x02 */ s16 place;           /* 0 first; the grid lane at the start */
    /* 0x04 */ s16 prevPlace;
    /* 0x06 */ s16 placeAnim;
    /* 0x08 */ s16 finishPlace;     /* -1 until finished */
    /* 0x0A */ s16 lapMsgTimer;
    /* 0x0C */ s16 gapMsgTimer;
    /* 0x0E */ s16 unk_0E;
    /* 0x10 */ s32 gap;             /* frames to the rider ahead or behind */
    /* 0x14 */ s32 lapTimes[6];
    /* 0x2C */ s32 finishTime;
} RiderRace; /* size 0x30 */

typedef struct Rider {
    /* 0x000 */ Vec3i drawPos;      /* bike, mm */
    /* 0x00C */ Vec3i drawBodyPos;  /* thrown rider, mm */
    /* 0x018 */ s32 drawRot[3];     /* from drawQuat */
    /* 0x024 */ s32 rot[3];         /* from the bike body's quaternion */
    /* 0x030 */ s32 drawSteer;
    /* 0x034 */ s16 heading;
    /* 0x036 */ s16 yawLag;         /* cam_calc_bike_yaw */
    /* 0x038 */ s32 drawSuspFront;  /* mm */
    /* 0x03C */ s32 drawSuspRear;
    /* 0x040 */ u16 frontSpinAngle;
    /* 0x042 */ u16 rearSpinAngle;
    /* 0x044 */ u8 crashState;      /* 0 riding, 1 crashed, 2 rider clear of the bike */
    /* 0x045 */ s8 pendingCrash;    /* > 0 crash on landing, < 0 now: 0x19 back, 0x1C forward, 0x1F/0x22 side */
    /* 0x046 */ u8 unk_046[2];
    /* 0x048 */ RigidBody bike;
    /* 0x1CC */ RigidBody body;     /* the rider, once thrown off */
    /* 0x350 */ s32 sparkTimer;
    /* 0x354 */ Vec3f sparkDir;
    /* 0x360 */ u8 unk_360[0xC];
    /* 0x36C */ u8 crashPhase;
    /* 0x36D */ u8 crashDir;        /* (crash type - 0x19) / 3 */
    /* 0x36E */ u8 unk_36E[2];
    /* 0x370 */ f32 drawQuat[4];
    /* 0x380 */ f32 impactSpeed;
    /* 0x384 */ f32 slideSpeed;
    /* 0x388 */ f32 bumpSeed;       /* math_sin_rand */
    /* 0x38C */ f32 trickQuat[4];
    /* 0x39C */ Vec3f trickOffset;
    /* 0x3A8 */ f32 trickBlend;
    /* 0x3AC */ s32 animFrame;      /* 8.8 fixed point */
    /* 0x3B0 */ s32 animSpeed;
    /* 0x3B4 */ s32 animHold;
    /* 0x3B8 */ s16 trickCount;
    /* 0x3BA */ s16 unk_3BA;
    /* 0x3BC */ s16 headYaw;
    /* 0x3BE */ s16 unk_3BE;
    /* 0x3C0 */ s16 headTimer;
    /* 0x3C2 */ s16 unk_3C2;
    /* 0x3C4 */ s32 stuntScore;
    /* 0x3C8 */ s32 raceKey;        /* race order key */
    /* 0x3CC */ s32 prevAnimFrame;
    /* 0x3D0 */ s8 anim;            /* current trick or animation, -1 none */
    /* 0x3D1 */ s8 prevAnim;
    /* 0x3D2 */ s8 trick;
    /* 0x3D3 */ s8 queuedTrick;     /* stunt_read_input */
    /* 0x3D4 */ s8 stickHist[8];
    /* 0x3DC */ u8 unk_3DC[8];
    /* 0x3E4 */ u8 trickHist[6];
    /* 0x3EA */ u8 trickListLen;
    /* 0x3EB */ s8 trickList[16];
    /* 0x3FB */ u8 unk_3FB;
    /* 0x3FC */ u8 stickHistIdx;
    /* 0x3FD */ u8 stickHistCount;
    /* 0x3FE */ s8 pipTimer;        /* stunt replay inset view */
    /* 0x3FF */ u8 finishState;
    /* 0x400 */ u16 celebrationTimer; /* cam_mode_celebration */
    /* 0x402 */ u8 checkpoint;      /* desert */
    /* 0x403 */ u8 unk_403;
    /* 0x404 */ f32 checkpointDist;
    /* 0x408 */ s32 trickFrame;
    /* 0x40C */ u8 unk_40C[4];
    /* 0x410 */ s16 crestCrouch;    /* grows while a crest is ahead */
    /* 0x412 */ s16 unk_412;
    /* 0x414 */ s16 wheelieBalance;
    /* 0x416 */ s16 wheelieTimer;
    /* 0x418 */ s8 crestAhead;      /* path_find_crest_ahead */
    /* 0x419 */ u8 leftFootOut;
    /* 0x41A */ u8 rightFootOut;
    /* 0x41B */ u8 powerSlide;
    /* 0x41C */ u8 hardTurn;
    /* 0x41D */ u8 noCollide;       /* ghost, respawn */
    /* 0x41E */ u8 unk_41E[2];
    /* 0x420 */ f32 speed;
    /* 0x424 */ f32 fwdSpeed;
    /* 0x428 */ f32 crashSpeedCap;
    /* 0x42C */ f32 laneOffset;
    /* 0x430 */ Vec3f localAngVel;
    /* 0x43C */ Vec3f crashVel;     /* velocity at the crash */
    /* 0x448 */ f32 driveForce;     /* rear wheel */
    /* 0x44C */ f32 timeScale;
    /* 0x450 */ f32 aiSkill;        /* race_init_ai_skills */
    /* 0x454 */ Wheel rearWheel;
    /* 0x4F8 */ Wheel frontWheel;
    /* 0x59C */ u8 unk_59C[4];
    /* 0x5A0 */ f32 steer;          /* radians */
    /* 0x5A4 */ u8 unk_5A4[4];
    /* 0x5A8 */ s16 savedCamMode;
    /* 0x5AA */ s16 camMode;        /* gCamModes */
    /* 0x5AC */ s32 throttleTwist;  /* right wrist */
    /* 0x5B0 */ u32 input;          /* pad word: A 0x10, B 0x20, Start 0x40, L 0x8000, R 0x10000, Z 0x20000, stick x
                                       bits 24-27 (sign 8), y bits 28-31 (sign 2) */
    /* 0x5B4 */ u32 prevInput;
    /* 0x5B8 */ s32 turboPressFrame;
    /* 0x5BC */ s32 turboReleaseFrame;
    /* 0x5C0 */ s32 rHeld;          /* frames R held (> 0) or released (< 0) */
    /* 0x5C4 */ s32 aiFrame;
    /* 0x5C8 */ s32 hardLandFrame;
    /* 0x5CC */ u8 landingAssist;
    /* 0x5CD */ u8 roughLanding;
    /* 0x5CE */ u8 unk_5CE;
    /* 0x5CF */ u8 dirtSpray;
    /* 0x5D0 */ s8 stickX;
    /* 0x5D1 */ u8 unk_5D1;
    /* 0x5D2 */ s8 stickY;
    /* 0x5D3 */ u8 unk_5D3;
    /* 0x5D4 */ s32 inHistIdx;      /* rider_update_input_history */
    /* 0x5D8 */ s32 inHist[8];
    /* 0x5F8 */ s32 inHistAvg;
    /* 0x5FC */ s8 stickHist8Idx;
    /* 0x5FD */ s8 stickHist8[8];
    /* 0x605 */ u8 unk_605[8];
    /* 0x60D */ s8 stickHist8Avg;
    /* 0x60E */ s8 stickHist16Idx;
    /* 0x60F */ s8 stickHist16[16];
    /* 0x61F */ u8 unk_61F;
    /* 0x620 */ u8 blinkCount;
    /* 0x621 */ u8 blinkTimer;
    /* 0x622 */ s16 unk_622;
    /* 0x624 */ s16 crashTimer;
    /* 0x626 */ s16 prevSegment;
    /* 0x628 */ s16 segment;        /* gPathNodes */
    /* 0x62A */ s16 unk_62A;
    /* 0x62C */ f32 segmentDist;
    /* 0x630 */ f32 distToGo;
    /* 0x634 */ f32 progressSpeed;
    /* 0x638 */ f32 lateral;        /* offset from the racing line */
    /* 0x63C */ f32 segmentT;
    /* 0x640 */ u8 unk_640[4];
    /* 0x644 */ RiderEngine engine;
    /* 0x658 */ BikeSkeleton *bikeSkel;
    /* 0x65C */ RiderSkeleton *riderSkel;
    /* 0x660 */ RiderRace race;
    /* 0x690 */ u32 flags;          /* 0x8 out of the race, 0x10 finished: brake */
    /* 0x694 */ u8 unk_694[8];
    /* 0x69C */ s32 timeLimit;
    /* 0x6A0 */ u8 unk_6A0[0x12];
    /* 0x6B2 */ s16 bestSegment;
    /* 0x6B4 */ u8 unk_6B4[0x34];
    /* 0x6E8 */ u8 isCpu;           /* humans become CPU riders after the finish */
    /* 0x6E9 */ u8 reversing;
    /* 0x6EA */ u8 unk_6EA;
    /* 0x6EB */ u8 pathSearch;
    /* 0x6EC */ u8 landingPick;
    /* 0x6ED */ s8 lapAdjust;
    /* 0x6EE */ u8 wrongWay;
    /* 0x6EF */ u8 unk_6EF;
    /* 0x6F0 */ f32 takeoffVelY;
    /* 0x6F4 */ f32 landingTime;    /* landing prediction */
    /* 0x6F8 */ f32 landingTime1;
    /* 0x6FC */ Vec3f landingPos1;
    /* 0x708 */ Vec3f landingNormal1;
    /* 0x714 */ s16 landingSegment1;
    /* 0x716 */ s16 unk_716;
    /* 0x718 */ f32 landingTime2;
    /* 0x71C */ Vec3f landingPos2;
    /* 0x728 */ Vec3f landingNormal2;
    /* 0x734 */ s16 landingSegment2;
    /* 0x736 */ s16 unk_736;
    /* 0x738 */ f32 airPitch;
    /* 0x73C */ s32 airPitchInput;
    /* 0x740 */ s16 unk_740;
    /* 0x742 */ s16 unk_742;
    /* 0x744 */ s32 airFrames;      /* frames in the air, negative on the ground */
    /* 0x748 */ s32 jumpState;
    /* 0x74C */ Vec3f takeoffPos;
    /* 0x758 */ f32 jumpDist;
    /* 0x75C */ f32 prevJumpDist;
    /* 0x760 */ u8 unk_760[4];
    /* 0x764 */ u8 impactFlags;
    /* 0x765 */ u8 unk_765[3];
    /* 0x768 */ s32 landingImpact;
    /* 0x76C */ u8 unk_76C[8];
    /* 0x774 */ s32 landingFrame;
    /* 0x778 */ s32 unk_778;
    /* 0x77C */ s16 unk_77C;
    /* 0x77E */ s16 unk_77E;
    /* 0x780 */ s32 surfaceIdx;
    /* 0x784 */ s32 contactFrame[8]; /* gRaceFrame of the last contact with each other rider */
    /* 0x7A4 */ u8 frozen;
    /* 0x7A5 */ u8 inactive;
    /* 0x7A6 */ u8 laneBlocked;     /* ai_update_lanes */
    /* 0x7A7 */ u8 standing;        /* index in gRiderStandings */
    /* 0x7A8 */ s8 fistPump;        /* frame of the fist-pump animation, 0 none */
    /* 0x7A9 */ u8 gridSlot;
    /* 0x7AA */ s8 laneChange;      /* Excite 3D lane change frames */
    /* 0x7AB */ u8 unk_7AB;
    /* 0x7AC */ s16 offTrackTimer;
    /* 0x7AE */ s16 unk_7AE;
    /* 0x7B0 */ s32 spawnFrame;
    /* 0x7B4 */ f32 weight;         /* handling stats (sRiderStatsTable): collision mass scale */
    /* 0x7B8 */ f32 landing;
    /* 0x7BC */ f32 handling;
    /* 0x7C0 */ f32 turbo;
    /* 0x7C4 */ f32 airTurn;
    /* 0x7C8 */ f32 airFloat;
    /* 0x7CC */ f32 unk_7CC;
    /* 0x7D0 */ f32 turnRadius;
    /* 0x7D4 */ f32 unk_7D4;
    /* 0x7D8 */ f32 power;
    /* 0x7DC */ u8 unk_7DC;
    /* 0x7DD */ u8 unk_7DD[3];
} Rider; /* size 0x7E0 */

#endif
