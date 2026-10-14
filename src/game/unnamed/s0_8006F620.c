#include "game.h"

/* Stunts: the stick sequences of the tricks, starting and animating a trick or animation of gRiderAnims, and the
   points. Stick directions (Rider.stickHist): 1 trick button released, 2 centred, 3-6 pushed. */

/* A trick's stick sequence: the directions, newest first, 0-terminated. The tables end with trick 0xFF. */
typedef struct { u8 trick; u8 seq[7]; } Rec_43;

extern Rec_43 *sStuntSeqByDir[];        /* the first entry for each newest direction 2-6 */
extern Rec_43 *sStuntSeqByDirAlt[];

extern Rec_43 sStuntInputSeqs[13];
extern Rec_43 sStuntInputSeqsAlt[8];    /* with R held */

/* True when a's sequence sorts before b's. */
static inline u8 cmp(Rec_43 *a, Rec_43 *b)
{
    s32 k;

    for (k = 0; k < 7; k++) {
        if (a->seq[k] > b->seq[k]) {
            return 0;
        }
        if (a->seq[k] < b->seq[k]) {
            return 1;
        }
    }
    return 0;
}

/* Sorts both sequence tables in decreasing order (the terminator stays last) and indexes them by the newest
   direction. */
void stunt_init_input_tables(void)
{
    u8 swapped;
    s32 i;
    u32 entry;
    s32 offset;
    Rec_43 *table, *cur, *next, *second;
    Rec_43 tmp;

    table = sStuntInputSeqs;
    second = &table[1];
    do {
        swapped = 0;
        i = 0;
        cur = table;
        offset = 8;
        next = second;
        for (; i < sizeof(sStuntInputSeqs) / sizeof(Rec_43) - 2; i++) {
            if (cmp(cur, next)) {
                swapped = 1;
                tmp = *cur;
                *cur = *(Rec_43 *)(offset + (u32)table);
                *(Rec_43 *)(offset + (u32)table) = tmp;
            }
            cur++;
            offset += 8;
            next++;
        }
    } while (swapped);
    for (i = 0; i < 5; i++) {
        for (entry = 0; entry < sizeof(sStuntInputSeqs) / sizeof(Rec_43); entry++) {
            if (sStuntInputSeqs[entry].seq[0] <= i + 2) {
                break;
            }
        }
        sStuntSeqByDir[i] = (Rec_43 *)(entry * 8 + (u32)sStuntInputSeqs); /* &sStuntInputSeqs[entry] */
    }

    table = sStuntInputSeqsAlt;
    second = &table[1];
    do {
        swapped = 0;
        i = 0;
        cur = table;
        offset = 8;
        next = second;
        for (; i < sizeof(sStuntInputSeqsAlt) / sizeof(Rec_43) - 2; i++) {
            if (cmp(cur, next)) {
                swapped = 1;
                tmp = *cur;
                *cur = *(Rec_43 *)(offset + (u32)table);
                *(Rec_43 *)(offset + (u32)table) = tmp;
            }
            cur++;
            offset += 8;
            next++;
        }
    } while (swapped);
    for (i = 0; i < 5; i++) {
        for (entry = 0; entry < sizeof(sStuntInputSeqsAlt) / sizeof(Rec_43); entry++) {
            if (sStuntInputSeqsAlt[entry].seq[0] <= i + 2) {
                break;
            }
        }
        sStuntSeqByDirAlt[i] = (Rec_43 *)(entry * 8 + (u32)sStuntInputSeqsAlt);
    }
}

/* Adds the stick's direction to the stick history and, when the trick button (C down) is pressed or the direction
   changes while it is held, queues the trick whose sequence the history ends with. */
void stunt_read_input(Rider *rider)
{
    u32 input;
    s32 x, y, dir;
    u8 changed;
    s32 k;
    s32 i;
    u8 **tab;
    u8 *entry;

    if ((rider->prevInput & INPUT_C_DOWN) && !(rider->input & INPUT_C_DOWN)) {
        rider->queuedTrick = ANIM_NONE;
        rider->stickHist[rider->stickHistIdx] = 1;
        return;
    }
    input = rider->input;
    y = input >> INPUT_STICK_Y_SHIFT;
    if (input & INPUT_STICK_DOWN) y = -y;
    x = ((s32)input >> INPUT_STICK_X_SHIFT) & 0xF;
    if (input & INPUT_STICK_RIGHT) x = -x;
    if (abs(x) < 8 && abs(y) < 8) {
        dir = 2;
    } else {
        dir = 3;
        if (y < x) dir = 5;
        if (-y < x) dir++;
    }
    changed = 0;
    if (rider->stickHist[rider->stickHistIdx] == dir && rider->stickHistCount < FRAMES(240)) {
        rider->stickHistCount++;
    } else {
        changed = 1;
        rider->stickHistIdx = (rider->stickHistIdx - 1) & 7;
        rider->stickHist[rider->stickHistIdx] = dir;
        rider->stickHistCount = 0;
    }
    if ((rider->input & INPUT_C_DOWN) && (!(rider->prevInput & INPUT_C_DOWN) || changed)) {
        /* entries sorted in decreasing order: stop at the first one past the history */
        if (rider->input & INPUT_R) {
            entry = sStuntSeqByDirAlt[dir - 2];
        } else {
            entry = sStuntSeqByDir[dir - 2];
        }
        do {
            for (i = 0; i < 7; i++) {
                if (entry[i + 1] == 0 &&
                    (((gRiderAnims[entry[0]].riderMask >> gGame.riders[rider - gRiders]) & 1) ||
                     (gCheatFlags & CHEAT_ALL_STUNTS))) {
                    rider->trickFrame = gRaceFrame;
                    rider->queuedTrick = entry[0];
                }
                if ((entry + i)[1] != rider->stickHist[(rider->stickHistIdx + i) & 7]) break;
            }
            if ((entry + i)[1] < rider->stickHist[(rider->stickHistIdx + i) & 7]) return;
            entry += 8;
        } while (i != 0);
    }
}

/* A rider's stunts for a frame: reads the stick, banks a trick's points after landing, starts a queued trick,
   plays the air turns, and animates the trick (one still going on the ground below 5 m/s crashes). */
void stunt_update(Rider *rider)
{
    s32 isNew;
    s32 k;
    s32 score;
    s32 ticksLeft;
    s32 end;
    s8 trick;
    s8 queued;
    s8 anim;

    stunt_read_input(rider);
    if (rider->trickDoneTimer != 0) {
        if (--rider->trickDoneTimer == 0) {
            if ((rider->airFrames < 0 || FRAME_RATE / 2.0f < rider->landingTime) && gEvent.allFinished == 0) {
                isNew = 0;
                score = rider->stuntScore;
                rider->stuntScore = 0;
                rider->stuntTotal += score;
                trick = rider->trick;
                if (trick < ANIM_TRICK_COUNT) {
                    for (k = 0; k < rider->trickListLen; k++) {
                        if (trick == rider->trickList[k]) break;
                    }
                    if (k == rider->trickListLen) {
                        isNew = 1;
                        rider->trickList[k] = rider->trick;
                        rider->trickListLen = k + isNew;
                    }
                    for (k = 4; k >= 0; k--) {
                        rider->trickHist[k + 1] = rider->trickHist[k];
                    }
                    rider->trickHist[0] = rider->trick;
                }
                if (isNew && (gCheatFlags & CHEAT_STUNT_BONUS)) {
                    rider->engine.heat = 0.0f;
                }
            } else {
                rider->trickDoneTimer = 1; /* wait for the landing */
            }
        }
    }
    anim = rider->anim;
    if (anim < 0) {
        queued = rider->queuedTrick;
        if (queued >= 0) {
            if (gRaceFrame - rider->trickFrame < FRAMES(60)) {
                stunt_start(rider, queued);
            } else {
                rider->queuedTrick = ANIM_NONE;
            }
        }
        return;
    }
    if (rider->anim == ANIM_AIR_TURN || rider->anim == ANIM_AIR_TURN_2) {
        /* to the last frame and back, back at the first frame when landing */
        if (rider->airFrames > 0) {
            ticksLeft = (s32)(rider->landingTime * (f32)FRAME_RATE);
            if (rider->animSpeed > 0 && ticksLeft >= 16) {
                rider->animFrame += rider->animSpeed;
                end = gRiderAnims[rider->anim].body->count << 8;
                if (rider->animFrame >= end) {
                    rider->animFrame = end - 0x100;
                    rider->animSpeed = -0x100;
                }
            } else {
                rider->animSpeed = -0x100;
                ticksLeft = (s32)(rider->landingTime * (f32)FRAME_RATE);
                if (ticksLeft < 15) {
                    if (ticksLeft < 2) {
                    stop:
                        rider->anim = ANIM_NONE;
                    } else {
                        rider->animFrame -= rider->animFrame / ticksLeft;
                    }
                }
            }
        } else {
            goto stop;
        }
        return;
    }
    if (rider->airFrames < 0 && rider->speed < 5.0f && anim < ANIM_TRICK_COUNT) {
        if (rider->drawRot[1] > 0) {
            bike_crash_begin(rider, ANIM_CRASH_RIGHT);
        } else {
            bike_crash_begin(rider, ANIM_CRASH_LEFT);
        }
        return;
    }
    if (stunt_animate(rider, (rider->input >> 8) & 1) != 0) { /* INPUT_C_DOWN held */
        u8 done = rider->anim;
        rider->trickDoneTimer = 0x20;
        rider->anim = ANIM_NONE;
        rider->trick = done;
    }
    if (rider->crashState == CRASH_STATE_RIDING && rider->anim < ANIM_TRICK_COUNT && rider->trick < ANIM_TRICK_COUNT) {
        rider->pipTimer = FRAMES(15);
    }
}

/* Starts a trick or animation of gRiderAnims (a trick in the air only if not landing within half a second, a crash
   at any time). A trick adds its points to stuntScore: the previous one, still to be banked, makes a combo. */
void stunt_start(Rider *rider, s32 anim)
{
    s32 speed;
    s32 d, k, j, count, points, crowd; s8 cur;
    u8 curAnim;
    s32 riderIdx;

    if (rider->airFrames > 0) {
        if (anim < ANIM_CRASH_BACK) {
            if (rider->landingTime < 0.5f) {
                return;
            }
            if (rider->pendingCrash != 0) {
                return;
            }
        }
    } else if (!(gRiderAnims[anim].flags & ANIM_FLAG_ON_GROUND)) {
        return;
    }
    rider->powerSlide = POWER_SLIDE_NONE;
    rider_pose_snapshot(rider->riderSkel->bones, RIDER_BONE_COUNT);
    if (anim == ANIM_KOBE) { /* only right after the take-off */
        if (rider->airFrames < 3) {
            return;
        }
        if (rider->airFrames >= 5) {
            rider->queuedTrick = ANIM_NONE;
            return;
        }
    }
    rider->anim = anim;
    rider->animFrame = 0;
    speed = gRiderAnims[anim].playSpeed;
    rider->queuedTrick = ANIM_NONE;
    rider->animHold = 0;
    rider->fistPump = 0;
    rider->holdTime = 0;
    rider->heldFrames = 0;
    rider->airTurnYaw = 0;
    rider->animSpeed = speed;
    if (anim >= ANIM_TRICK_COUNT) {
        return;
    }
    if (gRiderAnims[anim].announce == -1) {
        return;
    }
    if (rider->trickDoneTimer != 0) {
        d = rider->trick;
        rider->trickDoneTimer = 0;
        if (d < ANIM_TRICK_COUNT) {
            count = rider->trickListLen;
            for (k = 0; k < count; k++) {
                if (d == rider->trickList[k]) {
                    break;
                }
            }
            if (k == rider->trickListLen) {
                rider->trickList[k] = rider->trick;
                rider->trickListLen = k + 1;
            }
            for (k = 4; k >= 0; k--) {
                rider->trickHist[k + 1] = rider->trickHist[k];
            }
            rider->trickHist[0] = rider->trick;
        }
    }
    /* points + 2 per frame held, less for a repeat: 4/8 for the last trick again, then 5/8, 6/8, 7/8 */
    curAnim = (u8)rider->anim;
    d = 0;
    if (curAnim >= ANIM_TRICK_COUNT) {
        rider->stuntScore = 0;
    } else {
        cur = (s8)curAnim;
        points = gRiderAnims[cur].points;
        points += rider->heldFrames * 2;
        for (; d < 4; d++) {
            if (cur == rider->trickHist[d]) {
                break;
            }
        }
        points = points * (d + 4);
        points = points / 8;
        rider->stuntScore += points;
    }
    riderIdx = ((s32)rider - (s32)gRiders) * -0x41041041 >> 5; /* rider - gRiders, written out */
    if (riderIdx != gGhostSlot || gGhostRace == 0) {
        announce_trick(riderIdx, gRiderAnims[anim].announce);
    }
    /* the crowd: silent for a trick done twice in the last four, louder above 1000 points */
    crowd = 0;
    for (j = 0; j < 4; j++) {
        if ((s8)rider->anim == rider->trickHist[j]) {
            crowd++;
        }
    }
    if (crowd < 2) {
        if (gRiderAnims[anim].points < 0x3E9) {
            crowd = 1;
        } else {
            crowd = 2;
        }
    } else {
        crowd = 0;
    }
    tunes_queue_crowd(crowd);
}

/* Advances the rider's trick or animation; returns 1 when it is over. A trick plays forward, holds its last frames
   while held, then plays back by the landing; a Kobe landed more than 22.5 degrees off its heading crashes. */
s32 stunt_animate(Rider *rider, s32 held)
{
    s32 anim;
    s32 end;
    RiderAnim *info;
    s32 step;
    u32 flags;

    anim = rider->anim;
    if (anim < 0) {
        goto ret0;
    }
    info = &gRiderAnims[anim];
    end = info->body->count << 8;
    flags = info->flags;

    if (anim == ANIM_KOBE) {
        s32 ticksLeft = rider->landingTime1 * (f32)FRAME_RATE;
        s32 remaining;
        if (ticksLeft == 0) {
            ticksLeft = 1;
        }
        remaining = end - rider->animFrame - 0x100;
        step = remaining / ticksLeft;
        if (step > 0x80) {
            step = 0x80;
        }
        rider->animFrame += step;
        if (ticksLeft != 1 && rider->airFrames > 0) {
            goto ret0;
        }
        {
            s32 yawDiff = (s16)((u16)rider->drawRot[2] - (u16)rider->heading);
            if (yawDiff < 0) {
                yawDiff = -yawDiff;
            }
            if (yawDiff > 0x1000) {
                bike_queue_crash_2(rider, -ANIM_CRASH_RIGHT);
            }
        }
        return 1;
    }

    if (rider->holdTime != 0) {
        if (rider->holdTime < info->holdFrames || held != 0) {
            u16 prevHold = rider->holdTime;
            s32 phase;
            s32 back;
            rider->holdTime = prevHold + (u16)rider->animSpeed;
            rider->heldFrames = rider->heldFrames + 1;
            phase = ((u16)rider->holdTime >> 8) & 7;
            if (phase & 4) {
                phase = 8 - phase;
            }
            back = (phase << 8) + 0x100;
            rider->animFrame = end - back;
            goto ret0;
        }
        rider->animSpeed = -info->speed;
        rider->holdTime = 0;
        goto ret0;
    } else {
        s32 speed = rider->animSpeed;
        if (speed > 0) {
            s32 frame = rider->animFrame + speed;
            rider->animFrame = frame;
            if (frame < end) {
                goto ret0;
            }
            if (flags & ANIM_FLAG_LOOP) {
                rider->animFrame = 0;
                return 1;
            }
            if ((flags & ANIM_FLAG_HOLD) && held != 0) {
                s32 holdSpeed = info->holdSpeed;
                rider->holdTime = 1;
                rider->heldFrames = 1;
                rider->animHold = 1;
                rider->animSpeed = holdSpeed / 2;
                goto ret0;
            }
            if (!(flags & ANIM_FLAG_REVERSE)) {
                goto ret0;
            }
            rider->holdTime = 0;
            rider->animSpeed = -info->speed;
            goto ret0;
        }
        {
            s32 delta;
            s32 frame;
            if (rider->airFrames > 0) {
                s32 ticksLeft = rider->landingTime1 * (f32)FRAME_RATE;
                s32 toLand;
                if (ticksLeft == 0) {
                    ticksLeft = 1;
                }
                toLand = -rider->animFrame / ticksLeft;
                if (toLand < speed * 2) {
                    toLand = speed * 2;
                }
                if (rider->queuedTrick < 0) {
                    s32 half = speed / 2;
                    delta = (half < toLand) ? half : toLand;
                } else {
                    delta = speed;
                }
            } else {
                delta = speed;
            }
            frame = rider->animFrame + delta;
            rider->animFrame = frame;
            if (frame > 0) {
                goto ret0;
            }
            rider->animFrame = 0;
            return 1;
        }
    }
ret0:
    return 0;
}

/* The fields of Rider used below. */
struct Obj_58 {
    u8 pad0[0x3BA];
    s16 heldFrames;
    u8 pad1[0x3C4 - 0x3BC];
    s32 stuntScore;
    u8 pad2[0x3D0 - 0x3C8];
    u8 anim;
    u8 pad3[0x3E4 - 0x3D1];
    u8 trickHist[4];
};

/* Adds the current trick's points to stuntScore, as stunt_start does (the division by 8 written out). Not called. */
void stunt_add_points(Obj_58 *rider)
{
    s32 points;
    s8 trick;
    s32 k;
    s32 rounded;
    if (rider->anim >= ANIM_TRICK_COUNT) {
        rider->stuntScore = 0;
        return;
    }
    trick = (s8)rider->anim;
    points = gRiderAnims[trick].points;
    points += rider->heldFrames * 2;
    for (k = 0; k < 4; k++) {
        if (trick == rider->trickHist[k]) {
            break;
        }
    }
    points = points * (k + 4);
    rounded = points;
    if (points < 0) {
        rounded = points + 7;
    }
    points = rounded >> 3;
    rider->stuntScore += points;
}

/* cmp on two sequence table entries: true when a's sequence sorts before b's. Not called. */
s32 stunt_seq_less(u8 *a, u8 *b)
{
    s32 i;

    for (i = 0; i < 7; i++) {
        u8 *seqA = a + i;
        u8 *seqB = b + i;

        if (seqA[1] > seqB[1]) {
            return 0;
        }
        if (seqB[1] > seqA[1]) {
            return 1;
        }
    }
    return 0;
}

/* Empty; called at each take-off. */
void stunt_on_takeoff(Rider *rider)
{
}

/* Records the last trick in the trick history and, if new (returns 1), in the list of different tricks. Not called:
   stunt_update and stunt_start have it written out. */
s32 stunt_record_done(Rider *rider)
{
    s32 isNew = 0;
    s32 i;

    if (rider->trick < ANIM_TRICK_COUNT) {
        for (i = 0; i < rider->trickListLen; i++) {
            if (rider->trick == rider->trickList[i]) {
                break;
            }
        }
        if (i == rider->trickListLen) {
            isNew = 1;
            rider->trickList[i] = rider->trick;
            rider->trickListLen = i + 1;
        }
        for (i = 4; i >= 0; i--) {
            rider->trickHist[i + 1] = rider->trickHist[i];
        }
        rider->trickHist[0] = rider->trick;
    }
    return isNew;
}
