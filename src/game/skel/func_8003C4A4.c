#include "ctx.h"

typedef struct {
    s32 parent;
    s32 pos[3];
    s32 rot[9];
    s32 wpos[3];
    s32 wmtx[16];
} Node;

void func_80011C1C(s32 *a, s32 *b, s32 *c);
void func_80012DC8(s32 *a, s32 *b, s32 *c);

void func_8003C4A4(Node *n, s32 count)
{
    s32 i;
    Node *par;
    s32 v[3];

    n[0].wpos[0] = n[0].pos[0];
    n[0].wpos[1] = n[0].pos[1];
    n[0].wpos[2] = n[0].pos[2];
    for (i = 1; i < count; i++) {
        par = &n[n[i].parent];
        func_80011C1C(n[i].rot, par->wmtx, n[i].wmtx);
        func_80012DC8(n[i].pos, par->wmtx, n[i].wpos);
        n[i].wpos[0] += par->wpos[0];
        n[i].wpos[1] += par->wpos[1];
        n[i].wpos[2] += par->wpos[2];
    }
}
