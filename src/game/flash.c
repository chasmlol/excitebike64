#include "game.h"

/* Loads the stands' quads of the track and assigns each the track section nearest to its centre. resSet is
   unused (track_res_get reads the current track's set). */
void flash_init(s32 resSet)
{
    FlashQuadList *list;
    FlashQuadList *quads;
    s32 i;
    u32 j;
    s32 cx, cz, dx, dz;
    s32 bestDist;
    s32 bestSection;

    sFlashTris = list = (FlashQuadList *)track_res_get(gGidCamFlash);
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
            if (dx < bestDist || j == 0) {
                bestDist = dx;
                bestSection = j;
            }
        }
        quads->e[i].section = bestSection;
    }
end:
    sFlashCandidates = mem_alloc_dbg(sFlashTris->count << 2, 0, sStrFlashC, 0x6F);
    sFlashTimer = 0;
}

/* While the flash timer runs (one view only), sets off one flash per frame at a random point of a stand quad
   whose section is visible. */
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
                f32 pos[4];
                s32 vel[16]; /* only 3 used: the size keeps the original stack frame */
                i++;
                j = (s32)random_byte() % sFlashCandidateCount;
                q = sFlashCandidates[j];
                /* a random point of the quad: a and b in 1/256ths of its two edges from v[0] */
                a = (s32)random_byte() % 255;
                b = (s32)random_byte() % 255;
                pos[0] = q->v[0][0];
                pos[1] = q->v[0][1];
                pos[2] = q->v[0][2];
                pos[0] += ((f32)q->v[1][0] - (f32)q->v[0][0]) * (f32)a * 0.00390625f;
                pos[1] += ((f32)q->v[1][1] - (f32)q->v[0][1]) * (f32)a * 0.00390625f;
                pos[2] += ((f32)q->v[1][2] - (f32)q->v[0][2]) * (f32)a * 0.00390625f;
                pos[0] += ((f32)q->v[3][0] - (f32)q->v[0][0]) * (f32)b * 0.00390625f;
                pos[1] += ((f32)q->v[3][1] - (f32)q->v[0][1]) * (f32)b * 0.00390625f;
                pos[2] += ((f32)q->v[3][2] - (f32)q->v[0][2]) * (f32)b * 0.00390625f;
                /* 16 mm units to metres */
                pos[0] *= 0.016f;
                pos[1] *= 0.016f;
                pos[2] *= 0.016f;
                vel[0] = 0;
                vel[1] = 0;
                vel[2] = 0;
                particle_spawn(PARTICLE_SPARKLE, 0, pos, vel, &gParticleWhite);
            } while (i < 1);
        }
    }
}

/* Frees the list of visible quads. */
void flash_free(void)
{
    if (sFlashTris != 0) {
        mem_free(sFlashCandidates);
    }
}

/* Sets off flashes for the next `frames` frames (a human rider landed a jump). */
void flash_start(s32 frames)
{
    sFlashTimer = frames;
}
