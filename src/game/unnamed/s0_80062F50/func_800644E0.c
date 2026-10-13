#include "ctx.h"

typedef struct Player {
    u8 pad0[0x48];
    f32 x;          /* 0x48 */
    f32 y;          /* 0x4C */
    f32 z;          /* 0x50 */
    u8 pad54[0x42C - 0x54];
    f32 lane;       /* 0x42C */
    u8 pad430[0x628 - 0x430];
    s16 node;       /* 0x628 */
    s16 pad62A;
    f32 prog;       /* 0x62C */
    u8 pad630[0x638 - 0x630];
    f32 f638;       /* 0x638 */
    u8 pad63C[0x6E8 - 0x63C];
    u8 human;       /* 0x6E8 */
    u8 pad6E9[0x7A5 - 0x6E9];
    u8 f7A5;        /* 0x7A5 */
    u8 f7A6;        /* 0x7A6 */
    u8 pad7A7[0x7E0 - 0x7A7];
} Player;

typedef struct {
    u8 pad0[0x46];
    s16 next;       /* 0x46 */
    u8 pad48[0x58 - 0x48];
    f32 f58;
    f32 f5C;
    u8 pad60[0x6C - 0x60];
} TrackNode;

extern Player *D_800F0BE0[8];
extern f32 D_800F0C08[];
extern Player D_800E6700[];
extern TrackNode *D_800EE498;
extern f32 D_800EE48C;

#define NEAR(r, x, top, done)                           {                                                       Player *q_;                                         f32 v_, k_;                                     top:                                                    q_ = *l;                                            if (q_->human != 0) { v_ = q_->lane; k_ = 0.999f; } else { v_ = q_->f638; k_ = 0.999f; } v_ = v_ - (x);                                      l++;                                                if (v_ < k_ && (k_ = -k_) < v_) {                              r = 1;                                              goto done;                                      }                                                   if (*l != NULL) {                                       goto top;                                       }                                                   r = 0;                                          done:;                                              }

void func_800644E0(void)
{
    Player *list[8];
    Player *p;
    Player *q;
    Player **out;
    TrackNode *t;
    s32 i;
    s32 j;
    s32 n;
    s32 node;
    f32 lo;
    f32 hi;
    f32 d;
    f32 dx;
    f32 dz;
    f32 dist;
    f32 v;
    f32 c;
    f32 e;
    f32 best;
    f32 target;
    f32 len;
    f32 diff;
    u8 hit;
    f32 w;
    Player **l;
    f32 x;

    for (i = 0; i < 8; i++) {
        p = D_800F0BE0[i];
        if (p->human == 0 || p->f7A5 != 0) {
            continue;
        }
        if (p >= D_800E6700) {
            p->lane = D_800F0C08[p - D_800E6700];
        }
        lo = -20.0f;
        hi = 20.0f;
        node = p->node;
        j = 6;
        do {
            t = (TrackNode *)(node * sizeof(TrackNode) + (u32)D_800EE498);
            if (lo < -t->f58) {
                lo = -t->f58;
            }
            if (t->f5C < hi) {
                hi = t->f5C;
            }
            j--;
            node = t->next;
        } while (j != 0);
        if (hi < p->lane) {
            p->lane = hi;
        }
        if (p->lane < lo) {
            p->lane = lo;
        }
        n = 0;
        j = 0;
        p->f7A6 = 0;
        for (; j < 8; j++) {
            if (i == j) {
                continue;
            }
            q = D_800F0BE0[j];
            if (q->f7A5 != 0) {
                continue;
            }
            d = p->prog - q->prog + 1.0f;
            if (d < 0.0f) {
                d += D_800EE48C;
            }
            if (d > 5.0f) {
                continue;
            }
            dx = q->x - p->x;
            dz = q->z - p->z;
            dist = dx * dx + dz * dz;
            if (d * d + 100.0f < dist) {
                continue;
            }
            list[n] = q;
            n++;
            if (dist > 2.25f) {
                continue;
            }
            w = q->f638;
            diff = w - p->f638;
            if (d < 2.0f && dist < 2.25f) {
                if (diff > 0.0f) {
                    if (w < hi) {
                        hi = w;
                    }
                } else {
                    if (lo < w) {
                        lo = w;
                    }
                }
            }
        }
        list[n] = NULL;
        if (n != 0) {
            l = list;
            x = p->lane;
            NEAR(hit, x, n1, d1);
            if (hit) {
                best = 2.0f;
                for (n--; n >= 0; n--) {
                    q = list[n];
                    if (q->human != 0) {
                        v = q->lane;
                    } else {
                        v = q->f638;
                    }
                    c = v - 1.0f;
                    e = p->lane - c;
                    if ((e > 0.0f ? e : -e) < best && c < hi && lo < c) {
                        l = list;
                        NEAR(hit, c, n2, d2);
                        if (!hit) {
                            target = c;
                            best = p->lane - target;
                            if (!(best > 0.0f)) {
                                best = -best;
                            }
                        }
                    }
                    c = v + 1.0f;
                    e = p->lane - c;
                    if ((e > 0.0f ? e : -e) < best && c < hi && lo < c) {
                        l = list;
                        NEAR(hit, c, n3, d3);
                        if (!hit) {
                            target = c;
                            best = p->lane - target;
                            if (!(best > 0.0f)) {
                                best = -best;
                            }
                        }
                    }
                }
                if (best < 2.0f) {
                    p->lane = target;
                }
                continue;
            }
        }
        if (p < D_800E6700) {
            if (p->lane > 0.0f) {
                p->lane -= 0.02f;
            } else {
                p->lane += 0.02f;
            }
        }
    }
}
