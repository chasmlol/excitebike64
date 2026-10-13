#include "ctx.h"

void func_8008EDE4(Vtx_t *in, s32 n, s32 rot, Vtx_t *out, s32 dx, s32 dz, s32 flag)
{
    s32 x, z, nx, nz;

    for (; n != 0; n--) {
        switch (rot) {
        case 0:
            x = in->ob[0];
            z = in->ob[2];
            nx = in->cn[0];
            nz = in->cn[2];
            break;
        case 1:
            x = in->ob[2];
            nx = in->cn[2];
            z = -in->ob[0];
            nz = -in->cn[0];
            break;
        case 2:
            x = -in->ob[0];
            z = -in->ob[2];
            nx = -in->cn[0];
            nz = -in->cn[2];
            break;
        case 3:
            z = in->ob[0];
            nz = in->cn[0];
            x = -in->ob[2];
            nx = -in->cn[2];
            break;
        }
        out->ob[0] = dx + x + 0xfa;
        out->ob[1] = in->ob[1];
        out->ob[2] = dz + z + 0xfa;
        out->tc[0] = in->tc[0];
        out->tc[1] = in->tc[1];
        out->cn[0] = nx;
        out->cn[1] = in->cn[1];
        out->cn[2] = nz;
        if (flag != 0) {
            out->flag = 1;
        } else {
            out->flag = 0;
        }
        if (x == -0xfa || x == 0xfa || z == -0xfa || z == 0xfa) {
            out->cn[3] = 2;
        } else {
            out->cn[3] = 1;
        }
        out++;
        in++;
    }
}
