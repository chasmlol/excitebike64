#include "game.h"

typedef struct { u8 r0; u8 key[7]; } Rec_43;

extern Rec_43 *sStuntSeqByDir[];
extern Rec_43 *sStuntSeqByDirAlt[];

extern Rec_43 sStuntInputSeqs[13];
extern Rec_43 sStuntInputSeqsAlt[8];

static inline u8 cmp(Rec_43 *a, Rec_43 *b)
{
    s32 k;

    for (k = 0; k < 7; k++) {
        if (a->key[k] > b->key[k]) {
            return 0;
        }
        if (a->key[k] < b->key[k]) {
            return 1;
        }
    }
    return 0;
}

void stunt_init_input_tables(void)
{
    u8 flag;
    s32 i;
    u32 m;
    s32 off;
    Rec_43 *t, *pa, *pb, *p1;
    Rec_43 tmp;

    t = sStuntInputSeqs;
    p1 = &t[1];
    do {
        flag = 0;
        i = 0;
        pa = t;
        off = 8;
        pb = p1;
        for (; i < sizeof(sStuntInputSeqs) / sizeof(Rec_43) - 2; i++) {
            if (cmp(pa, pb)) {
                flag = 1;
                tmp = *pa;
                *pa = *(Rec_43 *)(off + (u32)t);
                *(Rec_43 *)(off + (u32)t) = tmp;
            }
            pa++;
            off += 8;
            pb++;
        }
    } while (flag);
    for (i = 0; i < 5; i++) {
        for (m = 0; m < sizeof(sStuntInputSeqs) / sizeof(Rec_43); m++) {
            if (sStuntInputSeqs[m].key[0] <= i + 2) {
                break;
            }
        }
        sStuntSeqByDir[i] = (Rec_43 *)(m * 8 + (u32)sStuntInputSeqs);
    }

    t = sStuntInputSeqsAlt;
    p1 = &t[1];
    do {
        flag = 0;
        i = 0;
        pa = t;
        off = 8;
        pb = p1;
        for (; i < sizeof(sStuntInputSeqsAlt) / sizeof(Rec_43) - 2; i++) {
            if (cmp(pa, pb)) {
                flag = 1;
                tmp = *pa;
                *pa = *(Rec_43 *)(off + (u32)t);
                *(Rec_43 *)(off + (u32)t) = tmp;
            }
            pa++;
            off += 8;
            pb++;
        }
    } while (flag);
    for (i = 0; i < 5; i++) {
        for (m = 0; m < sizeof(sStuntInputSeqsAlt) / sizeof(Rec_43); m++) {
            if (sStuntInputSeqsAlt[m].key[0] <= i + 2) {
                break;
            }
        }
        sStuntSeqByDirAlt[i] = (Rec_43 *)(m * 8 + (u32)sStuntInputSeqsAlt);
    }
}

void stunt_read_input(Rider *e)
{
    u32 w;
    s32 x, y, dir;
    u8 flag;
    s32 k;
    s32 i;
    u8 **tab;
    u8 *p;

    if ((e->prevInput & 0x100) && !(e->input & 0x100)) {
        e->queuedTrick = -1;
        e->stickHist[e->stickHistIdx] = 1;
        return;
    }
    w = e->input;
    y = w >> 28;
    if (w & 2) y = -y;
    x = ((s32)w >> 24) & 0xF;
    if (w & 8) x = -x;
    if (abs(x) < 8 && abs(y) < 8) {
        dir = 2;
    } else {
        dir = 3;
        if (y < x) dir = 5;
        if (-y < x) dir++;
    }
    flag = 0;
    if (e->stickHist[e->stickHistIdx] == dir && e->stickHistCount < 0xF0) {
        e->stickHistCount++;
    } else {
        flag = 1;
        e->stickHistIdx = (e->stickHistIdx - 1) & 7;
        e->stickHist[e->stickHistIdx] = dir;
        e->stickHistCount = 0;
    }
    if ((e->input & 0x100) && (!(e->prevInput & 0x100) || flag)) {
        if (e->input & 0x10000) {
            p = sStuntSeqByDirAlt[dir - 2];
        } else {
            p = sStuntSeqByDir[dir - 2];
        }
        do {
            for (i = 0; i < 7; i++) {
                if (p[i + 1] == 0 &&
                    (((gRiderAnims[p[0]].riderMask >> gGame.riders[e - gRiders]) & 1) ||
                     (gCheatFlags & 0x200))) {
                    e->trickFrame = gRaceFrame;
                    e->queuedTrick = p[0];
                }
                if ((p + i)[1] != e->stickHist[(e->stickHistIdx + i) & 7]) break;
            }
            if ((p + i)[1] < e->stickHist[(e->stickHistIdx + i) & 7]) return;
            p += 8;
        } while (i != 0);
    }
}


void stunt_update(Rider *o)
{
    s32 found;
    s32 k;
    s32 t;
    s32 v;
    s32 lim;
    s8 d2;
    s8 d3;
    s8 s;

    stunt_read_input(o);
    if (o->unk_3FB != 0) {
        if (--o->unk_3FB == 0) {
            if ((o->airFrames < 0 || 30.0f < o->landingTime) && gEvent.allFinished == 0) {
                found = 0;
                t = o->stuntScore;
                o->stuntScore = 0;
                o->raceKey += t;
                d2 = o->trick;
                if (d2 < 16) {
                    for (k = 0; k < o->trickListLen; k++) {
                        if (d2 == o->trickList[k]) break;
                    }
                    if (k == o->trickListLen) {
                        found = 1;
                        o->trickList[k] = o->trick;
                        o->trickListLen = k + found;
                    }
                    for (k = 4; k >= 0; k--) {
                        o->trickHist[k + 1] = o->trickHist[k];
                    }
                    o->trickHist[0] = o->trick;
                }
                if (found && (gCheatFlags & 0x10)) {
                    o->engine.heat = 0.0f;
                }
            } else {
                o->unk_3FB = 1;
            }
        }
    }
    s = o->anim;
    if (s < 0) {
        d3 = o->queuedTrick;
        if (d3 >= 0) {
            if (gRaceFrame - o->trickFrame < 60) {
                stunt_start(o, d3);
            } else {
                o->queuedTrick = -1;
            }
        }
        return;
    }
    if (o->anim == 16 || o->anim == 17) {
        if (o->airFrames > 0) {
            v = (s32)(o->landingTime * 60.0f);
            if (o->animSpeed > 0 && v >= 16) {
                o->animFrame += o->animSpeed;
                lim = gRiderAnims[o->anim].body->count << 8;
                if (o->animFrame >= lim) {
                    o->animFrame = lim - 0x100;
                    o->animSpeed = -0x100;
                }
            } else {
                o->animSpeed = -0x100;
                v = (s32)(o->landingTime * 60.0f);
                if (v < 15) {
                    if (v < 2) {
                    neg:
                        o->anim = -1;
                    } else {
                        o->animFrame -= o->animFrame / v;
                    }
                }
            }
        } else {
            goto neg;
        }
        return;
    }
    if (o->airFrames < 0 && o->speed < 5.0f && s < 16) {
        if (o->drawRot[1] > 0) {
            bike_crash_begin(o, 0x22);
        } else {
            bike_crash_begin(o, 0x1F);
        }
        return;
    }
    if (stunt_animate(o, (o->input >> 8) & 1) != 0) {
        u8 pv = o->anim;
        o->unk_3FB = 0x20;
        o->anim = -1;
        o->trick = pv;
    }
    if (o->crashState == 0 && o->anim < 16 && o->trick < 16) {
        o->pipTimer = 15;
    }
}


void stunt_start(Rider *o, s32 n)
{
    s32 u;
    s32 d, k, j, cnt, v, c2; s8 a2;
    u8 c;
    s32 id;

    if (o->airFrames > 0) {
        if (n < 0x19) {
            if (o->landingTime < 0.5f) {
                return;
            }
            if (o->pendingCrash != 0) {
                return;
            }
        }
    } else if (!(gRiderAnims[n].flags & 0x10)) {
        return;
    }
    o->powerSlide = 0;
    rider_pose_snapshot(o->riderSkel->bones, 0xF);
    if (n == 0xF) {
        if (o->airFrames < 3) {
            return;
        }
        if (o->airFrames >= 5) {
            o->queuedTrick = -1;
            return;
        }
    }
    o->anim = n;
    o->animFrame = 0;
    u = gRiderAnims[n].unk_2C;
    o->queuedTrick = -1;
    o->animHold = 0;
    o->fistPump = 0;
    o->trickCount = 0;
    o->unk_3BA = 0;
    o->unk_622 = 0;
    o->animSpeed = u;
    if (n >= 0x10) {
        return;
    }
    if (gRiderAnims[n].announce == -1) {
        return;
    }
    if (o->unk_3FB != 0) {
        d = o->trick;
        o->unk_3FB = 0;
        if (d < 0x10) {
            cnt = o->trickListLen;
            for (k = 0; k < cnt; k++) {
                if (d == o->trickList[k]) {
                    break;
                }
            }
            if (k == o->trickListLen) {
                o->trickList[k] = o->trick;
                o->trickListLen = k + 1;
            }
            for (k = 4; k >= 0; k--) {
                o->trickHist[k + 1] = o->trickHist[k];
            }
            o->trickHist[0] = o->trick;
        }
    }
    c = (u8)o->anim;
    d = 0;
    if (c >= 0x10) {
        o->stuntScore = 0;
    } else {
        a2 = (s8)c;
        v = gRiderAnims[a2].points;
        v += o->unk_3BA * 2;
        for (; d < 4; d++) {
            if (a2 == o->trickHist[d]) {
                break;
            }
        }
        v = v * (d + 4);
        v = v / 8;
        o->stuntScore += v;
    }
    id = ((s32)o - (s32)gRiders) * -0x41041041 >> 5;
    if (id != gGhostSlot || gGhostRace == 0) {
        announce_trick(id, gRiderAnims[n].announce);
    }
    c2 = 0;
    for (j = 0; j < 4; j++) {
        if ((s8)o->anim == o->trickHist[j]) {
            c2++;
        }
    }
    if (c2 < 2) {
        if (gRiderAnims[n].points < 0x3E9) {
            c2 = 1;
        } else {
            c2 = 2;
        }
    } else {
        c2 = 0;
    }
    tunes_queue_crowd(c2);
}


s32 stunt_animate(Rider *p, s32 k)
{
    s32 i;
    s32 top;
    RiderAnim *e;
    s32 v;
    u32 fl;

    i = p->anim;
    if (i < 0) {
        goto ret0;
    }
    e = &gRiderAnims[i];
    top = e->body->count << 8;
    fl = e->flags;

    if (i == 15) {
        s32 d = p->landingTime1 * 60.0f;
        s32 n;
        if (d == 0) {
            d = 1;
        }
        n = top - p->animFrame - 0x100;
        v = n / d;
        if (v > 0x80) {
            v = 0x80;
        }
        p->animFrame += v;
        if (d != 1 && p->airFrames > 0) {
            goto ret0;
        }
        {
            s32 df = (s16)((u16)p->drawRot[2] - (u16)p->heading);
            if (df < 0) {
                df = -df;
            }
            if (df > 0x1000) {
                bike_queue_crash_2(p, -0x22);
            }
        }
        return 1;
    }

    if (p->trickCount != 0) {
        if (p->trickCount < e->holdFrames || k != 0) {
            u16 o = p->trickCount;
            s32 x;
            s32 y;
            p->trickCount = o + (u16)p->animSpeed;
            p->unk_3BA = p->unk_3BA + 1;
            x = ((u16)p->trickCount >> 8) & 7;
            if (x & 4) {
                x = 8 - x;
            }
            y = (x << 8) + 0x100;
            p->animFrame = top - y;
            goto ret0;
        }
        p->animSpeed = -e->speed;
        p->trickCount = 0;
        goto ret0;
    } else {
        s32 b = p->animSpeed;
        if (b > 0) {
            s32 n = p->animFrame + b;
            p->animFrame = n;
            if (n < top) {
                goto ret0;
            }
            if (fl & 2) {
                p->animFrame = 0;
                return 1;
            }
            if ((fl & 8) && k != 0) {
                s32 q = e->unk_30;
                p->trickCount = 1;
                p->unk_3BA = 1;
                p->animHold = 1;
                p->animSpeed = q / 2;
                goto ret0;
            }
            if (!(fl & 1)) {
                goto ret0;
            }
            p->trickCount = 0;
            p->animSpeed = -e->speed;
            goto ret0;
        }
        {
            s32 vv;
            s32 w;
            if (p->airFrames > 0) {
                s32 d = p->landingTime1 * 60.0f;
                s32 qq;
                if (d == 0) {
                    d = 1;
                }
                qq = -p->animFrame / d;
                if (qq < b * 2) {
                    qq = b * 2;
                }
                if (p->queuedTrick < 0) {
                    s32 h = b / 2;
                    vv = (h < qq) ? h : qq;
                } else {
                    vv = b;
                }
            } else {
                vv = b;
            }
            w = p->animFrame + vv;
            p->animFrame = w;
            if (w > 0) {
                goto ret0;
            }
            p->animFrame = 0;
            return 1;
        }
    }
ret0:
    return 0;
}

struct Obj_58 {
    u8 pad0[0x3BA];
    s16 f3ba;
    u8 pad1[0x3C4 - 0x3BC];
    s32 f3c4;
    u8 pad2[0x3D0 - 0x3C8];
    u8 f3d0;
    u8 pad3[0x3E4 - 0x3D1];
    u8 f3e4[4];
};


void stunt_add_points(Obj_58 *o) {
    s32 base;
    s8 idx;
    s32 k;
    s32 t;
    if (o->f3d0 >= 16) {
        o->f3c4 = 0;
        return;
    }
    idx = (s8)o->f3d0;
    base = gRiderAnims[idx].points;
    base += o->f3ba * 2;
    for (k = 0; k < 4; k++) {
        if (idx == o->f3e4[k]) {
            break;
        }
    }
    base = base * (k + 4);
    t = base;
    if (base < 0) {
        t = base + 7;
    }
    base = t >> 3;
    o->f3c4 += base;
}

s32 stunt_seq_less(u8 *a, u8 *b) {
    s32 i;

    for (i = 0; i < 7; i++) {
        u8 *pa = a + i;
        u8 *pb = b + i;

        if (pa[1] > pb[1]) {
            return 0;
        }
        if (pb[1] > pa[1]) {
            return 1;
        }
    }
    return 0;
}

void stunt_on_takeoff(Rider *b)
{
}

s32 stunt_record_done(Rider *p)
{
    s32 ret = 0;
    s32 i;

    if (p->trick < 16) {
        for (i = 0; i < p->trickListLen; i++) {
            if (p->trick == p->trickList[i]) {
                break;
            }
        }
        if (i == p->trickListLen) {
            ret = 1;
            p->trickList[i] = p->trick;
            p->trickListLen = i + 1;
        }
        for (i = 4; i >= 0; i--) { p->trickHist[i + 1] = p->trickHist[i]; }
        p->trickHist[0] = p->trick;
    }
    return ret;
}
