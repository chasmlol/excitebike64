#include "game.h"

Blk_2 *shadow_clear_texture(Blk_2 *gdl, s32 unused, s32 n)
{
    s32 k = n;
    Blk_2 *p0 = gdl++;
    Blk_2 *p1 = gdl++;
    u32 c1 = 0xE3000A01;
    Blk_2 *p2 = gdl++;
    Blk_2 *p3 = gdl++;
    Blk_2 *p4, *p5;
    Blk_2 *p6;
    Blk_2 *q0, *q1, *q2;

    p0->w0 = 0xE7000000;
    p0->w1 = 0;
    p1->w1 = 0x300000;
    p1->w0 = c1;
    p2->w0 = 0xF7000000;
    p2->w1 = 0;
    p3->w0 = 0xED000000;
    p3->w1 = 0x80080;
    p4 = gdl++;
    p4->w0 = 0xE7000000;
    p4->w1 = 0;
    p5 = gdl++;
    p5->w0 = 0xFF68001F;
    p5->w1 = sShadowTextures + k * 0x400;
    p6 = gdl++;
    p6->w0 = 0xF607C07C;
    p6->w1 = 0;
    if (gRiders[k].crashState != 0) {
        q0 = gdl++;
        q0->w0 = 0xE7000000;
        q0->w1 = 0;
        q1 = gdl++;
        q1->w0 = 0xFF68001F;
        q1->w1 = sShadowTextures + (k + 8) * 0x400;
        q2 = gdl++;
        q2->w0 = 0xF607C07C;
        q2->w1 = 0;
    }
    {
        Blk_2 *e = gdl++;
        e->w0 = 0xE7000000;
        e->w1 = 0;
    }
    {
        Blk_2 *e = gdl++;
        e->w0 = 0xE3000A01;
        e->w1 = 0;
    }
    return gdl;
}

typedef struct {
    u8 pad0[0x220];
    Gfx *dl220;
    u8 pad224[0x22C - 0x224];
    void *seg22C;
    u8 pad230[0x234 - 0x230];
    void *seg234[2];
    void *mtx23C[(0xA80 - 0x23C) / 4];
} Plr_4;

extern Plr_4 gRiderGfx_9[];

Gfx *shadow_render_caster(s32 idx, Gfx *gdl, Mtx *m)
{
    f32 mf[4][4];
    s16 rot[64];
    Rider *blk;
    Gfx **tab;
    s32 i;

    gDPPipeSync(gdl++);
    gSPViewport(gdl++, &sShadowViewport);
    gDPSetScissor(gdl++, G_SC_NON_INTERLACE, 1, 1, 31, 31);
    gSPClipRatio(gdl++, FRUSTRATIO_6);
    blk = &gRiders[idx];
    guOrtho(mf, -15.0f, 15.0f, -15.0f, 15.0f, 50.0f, 4192.0f, 1.0f);
    mtx_from_angles_pos_scale(rot, -0x4000, 0, 0, 0, 0, 0, 4);
    mtx_mul(rot, (s16 *)mf, (s16 *)&sShadowProjMtx);
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
        gSPMatrix(gdl++, (u8 *)gRiderGfx_9[idx].mtx23C[gGfxFrameIndex] + i * 0x40, G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_PUSH);
        gSPDisplayList(gdl++, gBikeShadowDLs[i]);
        gSPPopMatrix(gdl++, G_MTX_MODELVIEW);
    }
    if (blk->crashState) {
        gDPSetColorImage(gdl++, G_IM_FMT_IA, G_IM_SIZ_8b, 32, sShadowTextures + (idx + 8) * 0x400);
    }
    gSPSegment(gdl++, 1, gRiderGfx_9[0].seg22C);
    gSPSegment(gdl++, 6, gRiderGfx_9[idx].seg234[gGfxFrameIndex]);
    if (!(gCheatFlags & 0x20)) {
        gSPDisplayList(gdl++, gRiderGfx_9[0].dl220);
    }
    gDPPipeSync(gdl++);
    gSPMatrix(gdl++, m + 16, G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);
    gSPSetGeometryMode(gdl++, gCullMode);
    return gdl;
}

Gfx *shadow_draw(s32 id, s32 unused, Gfx *gdl, s32 arg3) {
    s32 i;

    if (gGroundTriCount == 0 || sShadowTriCount == 0 || sShadowVtxCount == 0) {
        return gdl;
    }
    gDPPipeSync(gdl++);
    gDPSetRenderMode(gdl++, 0x404A50, 0);
    gDPSetCombine(gdl++, 0xFFB3FF, 0xFF65FEFF);
    if (gLightningFlash == 0) {
        gDPSetPrimColor(gdl++, 0, 0, 0, 0, 0, gRiderAlpha[id % 8] * 97 / 256);
    } else {
        gDPSetPrimColor(gdl++, 0, 0, 0, 0, 0, gRiderAlpha[id % 8]);
    }
    gDPSetBlendColor(gdl++, 0, 0, 0, 1);
    gDPSetAlphaCompare(gdl++, G_AC_THRESHOLD);
    gDPSetTextureLUT(gdl++, G_TT_NONE);
    gDPLoadTextureBlock(gdl++, sShadowTextures + (id << 10), G_IM_FMT_IA, G_IM_SIZ_8b, 32, 32, 0,
                        G_TX_CLAMP, G_TX_CLAMP, 6, 6, G_TX_NOLOD, G_TX_NOLOD);
    gSPVertex(gdl++, &sShadowVtx[gGfxFrameIndex * 1152 + id * 72], sShadowVtxCount, 0);
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

Gfx *shadow_draw_debug_texture(s32 param_1, s32 param_2, Gfx *gdl)
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
    g6 = gdl++;
    ((u32 *)g6)[0] = 0xFD700000;
    ((u32 *)g6)[1] = (u32)(sShadowTextures + param_1 * 0x400);
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

void shadow_build_mesh(s32 n)
{
    s32 x, y, z;
    s32 i, j, k, cnt;
    Vtx *v;
    u32 vb;
    Vec3i *src;

    if (n < 8) {
        x = gRiders[n].drawPos.x;
        y = gRiders[n].drawPos.y;
        z = gRiders[n].drawPos.z;
    } else {
        x = gRiders[n % 8].drawBodyPos.x;
        y = gRiders[n % 8].drawBodyPos.y;
        z = gRiders[n % 8].drawBodyPos.z;
    }
    height_collect_ground_tris(x - 1000, z - 1000, x + 1000, z + 1000, gRiders[n].drawPos.y + 1000);
    if (gGroundTriCount == 0) return;
    sShadowVtxCount = 0;
    sShadowTriCount = 0;
    k = gGfxFrameIndex * 0x480;
    k += n * 0x48;
    vb = (u32)sShadowVtx + k * 16;
    for (i = 0; i < gGroundTriCount; i++) {
        for (j = 0; j < 3; j++) {
            gGroundTris[i].v[j].x -= x;
            gGroundTris[i].v[j].y -= y;
            gGroundTris[i].v[j].z -= z;
        }
        shadow_clip_triangle(&gGroundTris[i], 1000);
        cnt = sShadowTriCount - 2;
        if (cnt + sClipOutCount >= 0x48) return;
        if (sShadowVtxCount + sClipOutCount >= 0x20) return;
        j = sShadowVtxCount;
        for (k = 0; k < sClipOutCount; k++) {
            v = (Vtx *)(sShadowVtxCount * 16 + vb);
            v->v.ob[0] = sClipPolyA[k].x;
            v->v.ob[1] = sClipPolyA[k].y;
            v->v.ob[2] = sClipPolyA[k].z;
            v->v.tc[0] = ((sClipPolyA[k].x + 1000) << 11) / 2000;
            sShadowVtxCount++;
            v->v.tc[1] = ((-sClipPolyA[k].z + 1000) << 11) / 2000;
        }
        for (k = 2; k < sClipOutCount; k++) {
            sShadowTriIndices[sShadowTriCount * 3] = j;
            sShadowTriIndices[sShadowTriCount * 3 + 1] = j + (k - 1);
            sShadowTriIndices[sShadowTriCount * 3 + 2] = j + k;
            sShadowTriCount++;
        }
    }
}

void shadow_clip_triangle(GroundTri *a, s32 lim)
{
    Vec *o;
    Vec *p;
    Vec *q;
    s32 i;
    s32 t;


    sClipPolyA_2[0] = a->v[0];
    sClipPolyA_2[1] = a->v[1];
    sClipPolyA_2[2] = a->v[2];
    sClipInCount = 3;
    o = sClipPolyB;
    i = 0;
    sClipOutCount = 0;
    for (; i < sClipInCount; i++) {
        p = &sClipPolyA_2[i];
        q = &sClipPolyA_2[(i + 1) % sClipInCount];
        if (p->x <= lim) {
            *o++ = *p;
            sClipOutCount++;
        }
        if ((p->x <= lim && q->x > lim) || (p->x > lim && q->x <= lim)) {
            t = ((lim - p->x) << 8) / (q->x - p->x);
            o->x = lim;
            o->y = p->y + t * (q->y - p->y) / 256;
            o->z = p->z + t * (q->z - p->z) / 256;
            o++;
            sClipOutCount++;
        }
    }

    sClipInCount = sClipOutCount;
    o = sClipPolyA_2;
    i = 0;
    sClipOutCount = 0;
        for (; i < sClipInCount; i++) {
            p = &sClipPolyB[i];
            q = &sClipPolyB[(i + 1) % sClipInCount];
            if (p->x >= -lim) {
                *o++ = *p;
                sClipOutCount++;
            }
            if ((p->x >= -lim && q->x < -lim) || (p->x < -lim && q->x >= -lim)) {
                t = ((-lim - p->x) << 8) / (q->x - p->x);
                o->x = -lim;
                o->y = p->y + t * (q->y - p->y) / 256;
                o->z = p->z + t * (q->z - p->z) / 256;
                o++;
                sClipOutCount++;
            }
        }

    sClipInCount = sClipOutCount;
    o = sClipPolyB;
    i = 0;
    sClipOutCount = 0;
        for (; i < sClipInCount; i++) {
            p = &sClipPolyA_2[i];
            q = &sClipPolyA_2[(i + 1) % sClipInCount];
            if (p->z <= lim) {
                *o++ = *p;
                sClipOutCount++;
            }
            if ((p->z <= lim && q->z > lim) || (p->z > lim && q->z <= lim)) {
                t = ((lim - p->z) << 8) / (q->z - p->z);
                o->z = lim;
                o->y = p->y + t * (q->y - p->y) / 256;
                o->x = p->x + t * (q->x - p->x) / 256;
                o++;
                sClipOutCount++;
            }
        }

    sClipInCount = sClipOutCount;
    o = sClipPolyA_2;
    i = 0;
    sClipOutCount = 0;
        for (; i < sClipInCount; i++) {
            p = &sClipPolyB[i];
            q = &sClipPolyB[(i + 1) % sClipInCount];
            if (p->z >= -lim) {
                *o++ = *p;
                sClipOutCount++;
            }
            if ((p->z >= -lim && q->z < -lim) || (p->z < -lim && q->z >= -lim)) {
                t = ((-lim - p->z) << 8) / (q->z - p->z);
                o->z = -lim;
                o->y = p->y + t * (q->y - p->y) / 256;
                o->x = p->x + t * (q->x - p->x) / 256;
                o++;
                sClipOutCount++;
            }
        }
}

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

void shadow_free(void)
{
    mem_free(sShadowVtx);
    mem_free(sShadowTexAlloc);
}
