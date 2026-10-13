#include "ctx.h"

typedef struct Blk {
    s32 w0;
    s32 w1;
    s32 w2;
    s32 w3;
    s16 h10;
    s16 k12;
    s16 *P;
} Blk;

s32 func_8009B11C(Blk *b, s32 s)
{
    s16 *P = b->P;
    s16 *Q = &P[s];
    s32 t = Q[2];

    switch ((t + b->k12) & 3) {
    case 1:
    case 3:
        return 0;
    case 0:
        switch (t) {
        case 0:
        case 3:
            return Q[4];
        case 2:
            return P[0] - Q[4] - 2;
        case 1:
            return P[1] - Q[4] - 2;
        }
    case 2: {
        s16 *R = b->P;
        s16 *Q2 = &R[s];

        switch (Q2[2]) {
        case 0:
            return R[0] - Q2[4] - 2;
        case 1:
            return Q2[4];
        case 2:
            return Q2[4];
        case 3:
            return R[1] - Q2[4] - 2;
        }
    }
    }
}

