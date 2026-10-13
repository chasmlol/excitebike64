#include "ctx.h"

typedef struct Spr {
    s32 w0;
    s32 w1;
    s32 w2;
    s32 w3;
    s16 h10;
    s16 rot;
    s16 *P;
} Spr;

s32 func_8009B258(Spr *p, s32 k)
{
    s16 *P = p->P;
    s16 *Q = P + k;
    s32 a = Q[2];

    switch ((a + p->rot) & 3) {
    case 0:
        return 0;
    case 2:
        return 0;
    case 3:
        switch (a) {
        case 2:
            return P[0] - Q[4] - 2;
        case 0:
        case 3:
            return Q[4];
        case 1:
            return P[1] - Q[4] - 2;
        }
    case 1: {
        s16 *R = p->P;
        s16 *S = R + k;

        switch (S[2]) {
        case 2:
            return S[4];
        case 0:
            return R[0] - S[4] - 2;
        case 3:
            return R[1] - S[4] - 2;
        case 1:
            return S[4];
        }
    }
    }
}
