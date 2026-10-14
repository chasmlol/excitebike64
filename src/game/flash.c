#include "game.h"


void flash_init(s32 arg0)
{
    FlashQuadList *list;
    FlashQuadList *quads;
    s32 i;
    u32 j;
    s32 cx, cz, dx, dz;
    s32 best;
    s32 bi;

    sFlashTris = list = (FlashQuadList *)track_res_get(D_800D1590);
    if (list == NULL) {
        return;
    }
    i = 0;
    if (i >= list->count) goto end;
    quads = list;
    for (; i < quads->count; i++) {
        /* the last z is v[3]'s y */
        cx = (quads->e[i].v[0][0] + quads->e[i].v[1][0] + quads->e[i].v[2][0] + quads->e[i].v[3][0]) / 4;
        cz = (quads->e[i].v[0][2] + quads->e[i].v[1][2] + quads->e[i].v[2][2] + quads->e[i].v[3][1]) / 4;
        for (j = 0; j < gTrackSections->count; j++) {
            dx = (gTrackSections->e[j].x0 + gTrackSections->e[j].x1) / 2 - cx;
            dz = (gTrackSections->e[j].z0 + gTrackSections->e[j].z1) / 2 - cz;
            dx /= 4;
            dz /= 4;
            dx = dx * dx + dz * dz;
            if (dx < best || j == 0) {
                best = dx;
                bi = j;
            }
        }
        quads->e[i].section = bi;
    }
end:
    sFlashCandidates = mem_alloc_dbg(sFlashTris->count << 2, 0, D_800D8E20, 0x6F);
    sFlashTimer = 0;
}

void flash_update(void)
{
    FlashQuadList *list = sFlashTris;
    s32 i;
    s32 j;
    FlashQuadList *quads;

    if (list != 0 && sFlashTimer != 0 && gNumViews == 1) {
        sFlashTimer = sFlashTimer - 1;
        sFlashCandidateCount = 0;
        i = 0;
        if (list->count != 0) {
            quads = list;
            do {
                j = quads->e[i].section;
                if (gTrackSections->e[j].visible & 1) {
                    sFlashCandidates[sFlashCandidateCount++] = &quads->e[i];
                }
                i++;
            } while (i < quads->count);
        }
        if (sFlashCandidateCount != 0) {
            i = 0;
            do {
                FlashQuad *q;
                s32 a, b;
                f32 v[4];
                s32 w[16];
                i++;
                j = (s32)random_byte() % sFlashCandidateCount;
                q = sFlashCandidates[j];
                a = (s32)random_byte() % 255;
                b = (s32)random_byte() % 255;
                v[0] = q->v[0][0];
                v[1] = q->v[0][1];
                v[2] = q->v[0][2];
                v[0] += ((f32)q->v[1][0] - (f32)q->v[0][0]) * (f32)a * 0.00390625f;
                v[1] += ((f32)q->v[1][1] - (f32)q->v[0][1]) * (f32)a * 0.00390625f;
                v[2] += ((f32)q->v[1][2] - (f32)q->v[0][2]) * (f32)a * 0.00390625f;
                v[0] += ((f32)q->v[3][0] - (f32)q->v[0][0]) * (f32)b * 0.00390625f;
                v[1] += ((f32)q->v[3][1] - (f32)q->v[0][1]) * (f32)b * 0.00390625f;
                v[2] += ((f32)q->v[3][2] - (f32)q->v[0][2]) * (f32)b * 0.00390625f;
                v[0] *= 0.016f;
                v[1] *= 0.016f;
                v[2] *= 0.016f;
                w[0] = 0;
                w[1] = 0;
                w[2] = 0;
                particle_spawn(5, 0, v, w, &gParticleWhite);
            } while (i < 1);
        }
    }
}

void flash_free(void)
{
    if (sFlashTris != 0) {
        mem_free(sFlashCandidates);
    }
}

void flash_start(s32 x)
{
    sFlashTimer = x;
}
