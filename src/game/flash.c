#include "game.h"

typedef struct {
    u16 count;
    u16 best;
    s16 v[3][3];
    s16 x3, z3;
} Rec_41;

extern Rec_41 *sFlashTris;

typedef struct {
    u32 count;
    s32 pad4;
    s32 x0, z0, x1, z1;
    s32 pad18, pad1C;
} Ent_71;

extern Ent_71 *gTrackSections_9;

void flash_init(s32 arg0)
{
    Rec_41 *list;
    Rec_41 *r;
    Rec_41 *recs;
    s32 i;
    u32 j;
    s32 cx, cz, dx, dz, d;
    s32 best;
    s32 bi;

    sFlashTris = list = (Rec_41 *)track_res_get(D_800D1590);
    if (list == NULL) {
        return;
    }
    i = 0;
    if (i >= list->count) goto end;
    recs = list;
    for (; i < recs->count; i++) {
        r = &recs[i];
        cx = (r->v[0][0] + r->v[1][0] + r->v[2][0] + r->x3) / 4;
        cz = (r->v[0][2] + r->v[1][2] + r->v[2][2] + r->z3) / 4;
        for (j = 0; j < gTrackSections_9->count; j++) {
            dx = (gTrackSections_9[j].x0 + gTrackSections_9[j].x1) / 2 - cx;
            dz = (gTrackSections_9[j].z0 + gTrackSections_9[j].z1) / 2 - cz;
            dx /= 4;
            dz /= 4;
            dx = dx * dx + dz * dz;
            if (dx < best || j == 0) {
                best = dx;
                bi = j;
            }
        }
        r->best = bi;
    }
end:
    sFlashCandidates = mem_alloc_dbg(sFlashTris->count << 2, 0, D_800D8E20, 0x6F);
    sFlashTimer = 0;
}

typedef struct {
    u16 idx;
    s16 x0, y0, z0;
    s16 x1, y1, z1;
    s16 pad[3];
    s16 x2, y2, z2;
} BikeTri;

typedef struct { u16 n; BikeTri t[1]; } Tbl_5;

extern BikeTri **sFlashCandidates_2;

void flash_update(void)
{
    Tbl_5 *tbl = sFlashTris;
    s32 i;
    s32 j;
    s32 o;
    Tbl_5 *b;

    if (tbl != 0 && sFlashTimer != 0 && gNumViews == 1) {
        sFlashTimer = sFlashTimer - 1;
        sFlashCandidateCount = 0;
        i = 0;
        if (tbl->n != 0) {
            b = tbl;
            do {
                j = b->t[i].idx;
                o = j * 32;
                if (((ObjInfo *)((s32)gTrackSections_10 + o))->flags & 1) {
                    sFlashCandidates_2[sFlashCandidateCount++] = &b->t[i];
                }
                i++;
            } while (i < b->n);
        }
        if (sFlashCandidateCount != 0) {
            i = 0;
            do {
                BikeTri *s;
                s32 a, b;
                f32 v[4];
                s32 w[16];
                i++;
                j = (s32)random_byte() % sFlashCandidateCount;
                s = sFlashCandidates_2[j];
                a = (s32)random_byte() % 255;
                b = (s32)random_byte() % 255;
                v[0] = s->x0;
                v[1] = s->y0;
                v[2] = s->z0;
                v[0] += ((f32)s->x1 - (f32)s->x0) * (f32)a * 0.00390625f;
                v[1] += ((f32)s->y1 - (f32)s->y0) * (f32)a * 0.00390625f;
                v[2] += ((f32)s->z1 - (f32)s->z0) * (f32)a * 0.00390625f;
                v[0] += ((f32)s->x2 - (f32)s->x0) * (f32)b * 0.00390625f;
                v[1] += ((f32)s->y2 - (f32)s->y0) * (f32)b * 0.00390625f;
                v[2] += ((f32)s->z2 - (f32)s->z0) * (f32)b * 0.00390625f;
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
