#include "game.h"

/* Shadow slots: 0-7 the riders on their bikes, 8-15 the thrown riders (slot rider + 8). Each has a 32x32 IA8 texture
   (0x400 bytes, sShadowTextures) and, per frame buffer, 72 vertices of ground mesh (sShadowVtx). */

/* Clears rider riderIdx's shadow texture to 0 with fill rectangles, and the thrown rider's one when crashed. */
Blk_2 *shadow_clear_texture(Blk_2 *gdl, s32 unused, s32 riderIdx)
{
    s32 idx = riderIdx;
    Blk_2 *sync = gdl++;
    Blk_2 *fillCycle = gdl++;
    u32 fillCycleW0 = 0xE3000A01;
    Blk_2 *fillColor = gdl++;
    Blk_2 *scissor = gdl++;
    Blk_2 *sync2, *image;
    Blk_2 *rect;
    Blk_2 *bodySync, *bodyImage, *bodyRect;

    sync->w0 = 0xE7000000;           /* gDPPipeSync */
    sync->w1 = 0;
    fillCycle->w1 = 0x300000;        /* gDPSetCycleType(G_CYC_FILL) */
    fillCycle->w0 = fillCycleW0;
    fillColor->w0 = 0xF7000000;      /* gDPSetFillColor(0) */
    fillColor->w1 = 0;
    scissor->w0 = 0xED000000;        /* gDPSetScissor(0, 0, 32, 32) */
    scissor->w1 = 0x80080;
    sync2 = gdl++;
    sync2->w0 = 0xE7000000;
    sync2->w1 = 0;
    image = gdl++;
    image->w0 = 0xFF68001F;          /* gDPSetColorImage(IA, 8b, 32, texture) */
    image->w1 = sShadowTextures + idx * 0x400;
    rect = gdl++;
    rect->w0 = 0xF607C07C;           /* gDPFillRectangle(0, 0, 31, 31) */
    rect->w1 = 0;
    if (gRiders[idx].crashState != 0) {
        bodySync = gdl++;
        bodySync->w0 = 0xE7000000;
        bodySync->w1 = 0;
        bodyImage = gdl++;
        bodyImage->w0 = 0xFF68001F;
        bodyImage->w1 = sShadowTextures + (idx + RIDER_COUNT) * 0x400;
        bodyRect = gdl++;
        bodyRect->w0 = 0xF607C07C;
        bodyRect->w1 = 0;
    }
    {
        Blk_2 *e = gdl++;
        e->w0 = 0xE7000000;
        e->w1 = 0;
    }
    {
        Blk_2 *e = gdl++;
        e->w0 = 0xE3000A01;          /* gDPSetCycleType(G_CYC_1CYCLE) */
        e->w1 = 0;
    }
    return gdl;
}

/* gRiderGfx entries (0xA80 bytes per rider), as the shadow code sees them */
typedef struct {
    u8 pad0[0x220];
    Gfx *shadowDl;              /* rider 0's: the rider's low-detail model, vertices in segment 1, bones in 6 */
    u8 pad224[0x22C - 0x224];
    void *loVtx;                /* the low-detail model's vertices */
    u8 pad230[0x234 - 0x230];
    void *boneMtx[2];           /* the rider's bone matrices, per frame buffer */
    void *bikeMtx[(0xA80 - 0x23C) / 4]; /* [0..1]: the six bike parts' matrices, per frame buffer */
} Plr_4;

extern Plr_4 gRiderGfx_9[];

/* Draws rider idx from above, white on clear, into its shadow texture: the bike parts, then the rider (into the
   thrown rider's texture when crashed; not with the Invisible Rider cheat). dynMtx[16] is the view's modelview. */
Gfx *shadow_render_caster(s32 idx, Gfx *gdl, Mtx *dynMtx)
{
    f32 ortho[4][4];
    s16 lookDown[64];
    Rider *rider;
    Gfx **tab;
    s32 i;

    gDPPipeSync(gdl++);
    gSPViewport(gdl++, &sShadowViewport);
    gDPSetScissor(gdl++, G_SC_NON_INTERLACE, 1, 1, 31, 31);
    gSPClipRatio(gdl++, FRUSTRATIO_6);
    rider = &gRiders[idx];
    guOrtho(ortho, -15.0f, 15.0f, -15.0f, 15.0f, 50.0f, 4192.0f, 1.0f);
    mtx_from_angles_pos_scale(lookDown, -0x4000, 0, 0, 0, 0, 0, 4);
    mtx_mul(lookDown, (s16 *)ortho, (s16 *)&sShadowProjMtx);
    mtx_from_angles_pos_scale(&sShadowViewMtx, 0, 0, 0, 0, 0, 0, 0x100);
    gSPClearGeometryMode(gdl++, G_CULL_BOTH);
    gSPTexture(gdl++, 0, 0, 0, 0, G_OFF);
    gDPSetCombineMode(gdl++, G_CC_PRIMITIVE, G_CC_PRIMITIVE);
    gDPSetRenderMode(gdl++, 0x0C084000, 0);
    gDPSetPrimColor(gdl++, 0, 0, 255, 255, 255, 255);
    gSPMatrix(gdl++, &sShadowProjMtx, G_MTX_PROJECTION | G_MTX_LOAD | G_MTX_NOPUSH);
    gSPMatrix(gdl++, &sShadowViewMtx, G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);
    gDPSetColorImage(gdl++, G_IM_FMT_IA, G_IM_SIZ_8b, 32, sShadowTextures + idx * 0x400);

    for (i = 0; i < 6; i++) {
        gSPMatrix(gdl++, (u8 *)gRiderGfx_9[idx].bikeMtx[gGfxFrameIndex] + i * 0x40, G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_PUSH);
        gSPDisplayList(gdl++, gBikeShadowDLs[i]);
        gSPPopMatrix(gdl++, G_MTX_MODELVIEW);
    }
    if (rider->crashState) {
        gDPSetColorImage(gdl++, G_IM_FMT_IA, G_IM_SIZ_8b, 32, sShadowTextures + (idx + RIDER_COUNT) * 0x400);
    }
    gSPSegment(gdl++, 1, gRiderGfx_9[0].loVtx);
    gSPSegment(gdl++, 6, gRiderGfx_9[idx].boneMtx[gGfxFrameIndex]);
    if (!(gCheatFlags & CHEAT_INVISIBLE_RIDER)) {
        gSPDisplayList(gdl++, gRiderGfx_9[0].shadowDl);
    }
    gDPPipeSync(gdl++);
    gSPMatrix(gdl++, dynMtx + 16, G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);
    gSPSetGeometryMode(gdl++, gCullMode);
    return gdl;
}

/* Draws slot's ground mesh (shadow_build_mesh) with its shadow texture, in black at the rider's alpha (38%, full
   during a lightning flash). */
Gfx *shadow_draw(s32 slot, s32 unused, Gfx *gdl, s32 view)
{
    s32 i;

    if (gGroundTriCount == 0 || sShadowTriCount == 0 || sShadowVtxCount == 0) {
        return gdl;
    }
    gDPPipeSync(gdl++);
    gDPSetRenderMode(gdl++, 0x404A50, 0);
    gDPSetCombine(gdl++, 0xFFB3FF, 0xFF65FEFF);
    if (gLightningFlash == 0) {
        gDPSetPrimColor(gdl++, 0, 0, 0, 0, 0, gRiderAlpha[slot % RIDER_COUNT] * 97 / 256);
    } else {
        gDPSetPrimColor(gdl++, 0, 0, 0, 0, 0, gRiderAlpha[slot % RIDER_COUNT]);
    }
    gDPSetBlendColor(gdl++, 0, 0, 0, 1);
    gDPSetAlphaCompare(gdl++, G_AC_THRESHOLD);
    gDPSetTextureLUT(gdl++, G_TT_NONE);
    gDPLoadTextureBlock(gdl++, sShadowTextures + (slot << 10), G_IM_FMT_IA, G_IM_SIZ_8b, 32, 32, 0,
                        G_TX_CLAMP, G_TX_CLAMP, 6, 6, G_TX_NOLOD, G_TX_NOLOD);
    gSPVertex(gdl++, &sShadowVtx[gGfxFrameIndex * 1152 + slot * 72], sShadowVtxCount, 0);
    for (i = 0; i < sShadowTriCount - 1; i += 2) {
        gSP2Triangles(gdl++, sShadowTriIndices[i * 3], sShadowTriIndices[i * 3 + 1], sShadowTriIndices[i * 3 + 2], 0,
                      sShadowTriIndices[i * 3 + 3], sShadowTriIndices[i * 3 + 4], sShadowTriIndices[i * 3 + 5], 0);
    }
    if (sShadowTriCount & 1) {
        gSP1Triangle(gdl++, sShadowTriIndices[i * 3], sShadowTriIndices[i * 3 + 1], sShadowTriIndices[i * 3 + 2], 0);
    }
    gDPPipeSync(gdl++);
    gDPSetTextureLUT(gdl++, G_TT_RGBA16);
    gDPSetAlphaCompare(gdl++, G_AC_NONE);
    gDPPipeSync(gdl++);
    return gdl;
}

/* Draws slot's shadow texture on screen at (50, 50), a debug view. */
Gfx *shadow_draw_debug_texture(s32 slot, s32 unused, Gfx *gdl)
{
    Gfx *g0;
    Gfx *g1;
    Gfx *g2;
    Gfx *g3;
    Gfx *g4;
    Gfx *g5;
    Gfx *g6;
    Gfx *g7;
    Gfx *g8;
    Gfx *g9;
    Gfx *g10;
    Gfx *g11;
    Gfx *g12;
    Gfx *g13;
    Gfx *g14;
    Gfx *g15;
    Gfx *g16;
    Gfx *g17;
    u32 c;

    /* pipe sync, render mode, combine mode, no TLUT, no perspective, texture on */
    g0 = gdl++;
    g1 = gdl++;
    c = 0xE200001C;
    ((u32 *)g0)[0] = 0xE7000000;
    ((u32 *)g0)[1] = 0;
    ((u32 *)g1)[0] = c;
    ((u32 *)g1)[1] = 0x0C084000;
    g2 = gdl++;
    ((u32 *)g2)[0] = 0xFCFFFFFF;
    ((u32 *)g2)[1] = 0xFFFCF87C;
    g3 = gdl++;
    ((u32 *)g3)[0] = 0xE3001001;
    ((u32 *)g3)[1] = 0;
    g4 = gdl++;
    ((u32 *)g4)[0] = 0xE3000C00;
    ((u32 *)g4)[1] = 0;
    g5 = gdl++;
    ((u32 *)g5)[0] = 0xD7000002;
    ((u32 *)g5)[1] = 0x80008000;
    /* gDPLoadTextureBlock(texture, G_IM_FMT_IA, G_IM_SIZ_8b, 32, 32, ...) */
    g6 = gdl++;
    ((u32 *)g6)[0] = 0xFD700000;
    ((u32 *)g6)[1] = (u32)(sShadowTextures + slot * 0x400);
    g7 = gdl++;
    ((u32 *)g7)[0] = 0xF5700000;
    ((u32 *)g7)[1] = 0x07098260;
    g8 = gdl++;
    ((u32 *)g8)[0] = 0xE6000000;
    ((u32 *)g8)[1] = 0;
    g9 = gdl++;
    ((u32 *)g9)[0] = 0xF3000000;
    ((u32 *)g9)[1] = 0x071FF200;
    g10 = gdl++;
    ((u32 *)g10)[0] = 0xE7000000;
    ((u32 *)g10)[1] = 0;
    g11 = gdl++;
    ((u32 *)g11)[0] = 0xF5680800;
    ((u32 *)g11)[1] = 0x00098260;
    g12 = gdl++;
    ((u32 *)g12)[0] = 0xF2000000;
    ((u32 *)g12)[1] = 0x0007C07C;
    /* a texture rectangle (50, 50)-(113, 113), then perspective back on */
    g13 = gdl++;
    ((u32 *)g13)[0] = 0xE41C41C4;
    ((u32 *)g13)[1] = 0x000C80C8;
    g14 = gdl++;
    ((u32 *)g14)[0] = 0xE1000000;
    ((u32 *)g14)[1] = 0;
    g15 = gdl++;
    ((u32 *)g15)[0] = 0xF1000000;
    ((u32 *)g15)[1] = 0x04000400;
    g16 = gdl++;
    ((u32 *)g16)[0] = 0xE7000000;
    ((u32 *)g16)[1] = 0;
    g17 = gdl++;
    ((u32 *)g17)[0] = 0xE3000C00;
    ((u32 *)g17)[1] = 0x00080000;
    return gdl;
}

/* Builds slot's shadow mesh: the ground triangles within a metre of the rider (slot >= 8: the thrown rider),
   clipped to a 2 m square mapped onto the shadow texture. */
void shadow_build_mesh(s32 slot)
{
    s32 cx, cy, cz;
    s32 i, j, k, tris;
    Vtx *vtx;
    u32 vtxBase;
    Vec3i *src;

    if (slot < RIDER_COUNT) {
        cx = gRiders[slot].drawPos.x;
        cy = gRiders[slot].drawPos.y;
        cz = gRiders[slot].drawPos.z;
    } else {
        cx = gRiders[slot % RIDER_COUNT].drawBodyPos.x;
        cy = gRiders[slot % RIDER_COUNT].drawBodyPos.y;
        cz = gRiders[slot % RIDER_COUNT].drawBodyPos.z;
    }
    /* gRiders[slot] reads past gRiders for slot >= 8 (slot % 8 above): a bug of the original */
    height_collect_ground_tris(cx - 1000, cz - 1000, cx + 1000, cz + 1000, gRiders[slot].drawPos.y + 1000);
    if (gGroundTriCount == 0) return;
    sShadowVtxCount = 0;
    sShadowTriCount = 0;
    k = gGfxFrameIndex * 0x480;
    k += slot * 0x48;
    vtxBase = (u32)sShadowVtx + k * 16;
    for (i = 0; i < gGroundTriCount; i++) {
        for (j = 0; j < 3; j++) {
            gGroundTris[i].v[j].x -= cx;
            gGroundTris[i].v[j].y -= cy;
            gGroundTris[i].v[j].z -= cz;
        }
        shadow_clip_triangle(&gGroundTris[i], 1000);
        /* the clipped polygon adds sClipOutCount - 2 triangles, as a fan */
        tris = sShadowTriCount - 2;
        if (tris + sClipOutCount >= 0x48) return;
        if (sShadowVtxCount + sClipOutCount >= 0x20) return;
        j = sShadowVtxCount;
        for (k = 0; k < sClipOutCount; k++) {
            vtx = (Vtx *)(sShadowVtxCount * 16 + vtxBase);
            vtx->v.ob[0] = sClipPolyA[k].x;
            vtx->v.ob[1] = sClipPolyA[k].y;
            vtx->v.ob[2] = sClipPolyA[k].z;
            vtx->v.tc[0] = ((sClipPolyA[k].x + 1000) << 11) / 2000;
            sShadowVtxCount++;
            vtx->v.tc[1] = ((-sClipPolyA[k].z + 1000) << 11) / 2000;
        }
        for (k = 2; k < sClipOutCount; k++) {
            sShadowTriIndices[sShadowTriCount * 3] = j;
            sShadowTriIndices[sShadowTriCount * 3 + 1] = j + (k - 1);
            sShadowTriIndices[sShadowTriCount * 3 + 2] = j + k;
            sShadowTriCount++;
        }
    }
}

/* Clips a triangle to the square |x| <= half, |z| <= half, one side at a time (Sutherland-Hodgman) between
   sClipPolyA_2 and sClipPolyB; the result is sClipPolyA_2 (sClipPolyA), sClipOutCount vertices. */
void shadow_clip_triangle(GroundTri *tri, s32 half)
{
    Vec *out;
    Vec *a;
    Vec *b;
    s32 i;
    s32 t;

    sClipPolyA_2[0] = tri->v[0];
    sClipPolyA_2[1] = tri->v[1];
    sClipPolyA_2[2] = tri->v[2];
    sClipInCount = 3;
    out = sClipPolyB;
    i = 0;
    sClipOutCount = 0;
    for (; i < sClipInCount; i++) {
        a = &sClipPolyA_2[i];
        b = &sClipPolyA_2[(i + 1) % sClipInCount];
        if (a->x <= half) {
            *out++ = *a;
            sClipOutCount++;
        }
        if ((a->x <= half && b->x > half) || (a->x > half && b->x <= half)) {
            t = ((half - a->x) << 8) / (b->x - a->x);
            out->x = half;
            out->y = a->y + t * (b->y - a->y) / 256;
            out->z = a->z + t * (b->z - a->z) / 256;
            out++;
            sClipOutCount++;
        }
    }

    sClipInCount = sClipOutCount;
    out = sClipPolyA_2;
    i = 0;
    sClipOutCount = 0;
    for (; i < sClipInCount; i++) {
        a = &sClipPolyB[i];
        b = &sClipPolyB[(i + 1) % sClipInCount];
        if (a->x >= -half) {
            *out++ = *a;
            sClipOutCount++;
        }
        if ((a->x >= -half && b->x < -half) || (a->x < -half && b->x >= -half)) {
            t = ((-half - a->x) << 8) / (b->x - a->x);
            out->x = -half;
            out->y = a->y + t * (b->y - a->y) / 256;
            out->z = a->z + t * (b->z - a->z) / 256;
            out++;
            sClipOutCount++;
        }
    }

    sClipInCount = sClipOutCount;
    out = sClipPolyB;
    i = 0;
    sClipOutCount = 0;
    for (; i < sClipInCount; i++) {
        a = &sClipPolyA_2[i];
        b = &sClipPolyA_2[(i + 1) % sClipInCount];
        if (a->z <= half) {
            *out++ = *a;
            sClipOutCount++;
        }
        if ((a->z <= half && b->z > half) || (a->z > half && b->z <= half)) {
            t = ((half - a->z) << 8) / (b->z - a->z);
            out->z = half;
            out->y = a->y + t * (b->y - a->y) / 256;
            out->x = a->x + t * (b->x - a->x) / 256;
            out++;
            sClipOutCount++;
        }
    }

    sClipInCount = sClipOutCount;
    out = sClipPolyA_2;
    i = 0;
    sClipOutCount = 0;
    for (; i < sClipInCount; i++) {
        a = &sClipPolyB[i];
        b = &sClipPolyB[(i + 1) % sClipInCount];
        if (a->z >= -half) {
            *out++ = *a;
            sClipOutCount++;
        }
        if ((a->z >= -half && b->z < -half) || (a->z < -half && b->z >= -half)) {
            t = ((-half - a->z) << 8) / (b->z - a->z);
            out->z = -half;
            out->y = a->y + t * (b->y - a->y) / 256;
            out->x = a->x + t * (b->x - a->x) / 256;
            out++;
            sClipOutCount++;
        }
    }
}

/* Allocates the shadow vertices (two frame buffers of 16 slots of 72) and the 16 textures, 64-byte aligned. */
void shadow_init(void)
{
    u32 x;

    sShadowVtx = mem_alloc_dbg(0x9000, 0, sStrShadowC, 0x1F6);
    sShadowTexAlloc = mem_alloc_dbg(0x4040, 0, sStrShadowC, 0x1F9);
    mem_fill((u8 *)sShadowTexAlloc, 0, 0x4040);
    x = (u32)sShadowTexAlloc;
    sShadowTextures = x;
    while ((x & 0x3F) != 0) {
        sShadowTextures = sShadowTextures + 0x10;
        x = sShadowTextures;
    }
}

/* Frees the shadow buffers. */
void shadow_free(void)
{
    mem_free(sShadowVtx);
    mem_free(sShadowTexAlloc);
}
