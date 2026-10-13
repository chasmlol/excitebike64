#include "ctx.h"

int func_8002E920(int, int, int);

float func_800228A0(float *p)
{
    float x = p[0];
    int r = func_8002E920((int)(x * 1000.0f), (int)(p[1] * 1000.0f), (int)(p[2] * 1000.0f));
    return (float)r / 1000.0f;
}
