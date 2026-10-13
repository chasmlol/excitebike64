#include "ctx.h"

extern s32 D_800F0A24;
extern s32 D_800F0A38;

#define STEP(i)                       \
    do {                              \
        c = D_800F0A24;               \
        v = D_800F0A38;               \
        c = c - 1;                    \
        D_800F0A24 = c;               \
        if (c == 0) {                 \
            D_800F0A38 = v + 1;       \
            D_800F0A24 = 8;           \
        }                             \
        i++;                          \
    } while (i < 8)

void func_8005FC68(void)
{
    s32 i;
    s32 j;
    s32 c;
    s32 v;
    s32 w;

    i = 0;
    do {
        v = D_800F0A38;
        c = D_800F0A24;
        c = c - 1;
        D_800F0A24 = c;
        if (c == 0) {
            w = v + 1;
            D_800F0A38 = w;
            D_800F0A24 = 8;
        }
        i++;
    } while (i < 8);
    i = 0;
    do {
        v = D_800F0A38;
        c = D_800F0A24;
        c = c - 1;
        D_800F0A24 = c;
        if (c == 0) {
            w = v + 1;
            D_800F0A38 = w;
            D_800F0A24 = 8;
        }
        i++;
    } while (i < 8);
    i = 0;
    do {
        v = D_800F0A38;
        c = D_800F0A24;
        c = c - 1;
        D_800F0A24 = c;
        if (c == 0) {
            w = v + 1;
            D_800F0A38 = w;
            D_800F0A24 = 8;
        }
        i++;
    } while (i < 8);
    i = 0;
    do {
        v = D_800F0A38;
        c = D_800F0A24;
        c = c - 1;
        D_800F0A24 = c;
        if (c == 0) {
            w = v + 1;
            D_800F0A38 = w;
            D_800F0A24 = 8;
        }
        i++;
    } while (i < 8);
    j = 0;
    do {
        i = 0;
        do {
            v = D_800F0A38;
            c = D_800F0A24;
            c = c - 1;
            D_800F0A24 = c;
            if (c == 0) {
                w = v + 1;
                D_800F0A38 = w;
                D_800F0A24 = 8;
            }
            i++;
        } while (i < 8);
        j++;
    } while (j < 6);
    j = 0;
    do {
        i = 0;
        do {
            v = D_800F0A38;
            c = D_800F0A24;
            c = c - 1;
            D_800F0A24 = c;
            if (c == 0) {
                w = v + 1;
                D_800F0A38 = w;
                D_800F0A24 = 8;
            }
            i++;
        } while (i < 8);
        j++;
    } while (j < 6);
}
