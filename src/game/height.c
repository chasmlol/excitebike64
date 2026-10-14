#include "game.h"

/* A desert ground triangle from the height map (2000 mm cells). */
static inline void tri(s32 area, s32 index, s32 ax, s32 az, s32 bx, s32 bz, s32 cx, s32 cz) {
    gGroundTris[gGroundTriCount].area = area;
    gGroundTris[gGroundTriCount].tri = index;
    gGroundTris[gGroundTriCount].v[0].x = ax * DESERT_CELL_MM;
    gGroundTris[gGroundTriCount].v[0].y = gDesertHeightMap[((az & 0x1FF) << 9) | (ax & 0x1FF)] << 6;
    gGroundTris[gGroundTriCount].v[0].z = az * DESERT_CELL_MM;
    gGroundTris[gGroundTriCount].v[1].x = bx * DESERT_CELL_MM;
    gGroundTris[gGroundTriCount].v[1].y = gDesertHeightMap[((bz & 0x1FF) << 9) | (bx & 0x1FF)] << 6;
    gGroundTris[gGroundTriCount].v[1].z = bz * DESERT_CELL_MM;
    gGroundTris[gGroundTriCount].v[2].x = cx * DESERT_CELL_MM;
    gGroundTris[gGroundTriCount].v[2].y = gDesertHeightMap[((cz & 0x1FF) << 9) | (cx & 0x1FF)] << 6;
    gGroundTris[gGroundTriCount].v[2].z = cz * DESERT_CELL_MM;
    gGroundTriCount++;
}

/* Takes the sections as the height areas, bins them on a 16x16 grid of their total bounding box, and lists the
   surface polygons over each section. */
void height_init(TrackModelList *sections, TrackModelList *objects, WallMesh *walls, u32 *polys)
{
    u32 i;
    s32 n;
    s32 cell;
    s32 x, z;
    s32 j, k, total;
    u16 *list;
    TrackModel *sec;
    TrackModelList *entry; /* section n is r->e[0] */
    SurfacePoly *p;
    SurfacePoly **tbl;
    s16 *ids;

    sHeightAreas = sections;
    sWallMesh = walls;
    gGroundArea = -1;
    if (sections != NULL) {
        for (i = 0; i < sHeightAreas->count; i++) {
            sec = (TrackModel *)((u8 *)sHeightAreas + (i * 32 + 4)); /* &sHeightAreas->e[i] */
            if (sec->x0 < sHeightAreaMin[0] || i == 0) sHeightAreaMin[0] = sec->x0;
            if (sec->z0 < sHeightAreaMin[1] || i == 0) sHeightAreaMin[1] = sec->z0;
            if (sec->x1 > sHeightAreaMax[0] || i == 0) sHeightAreaMax[0] = sec->x1;
            if (sec->z1 > sHeightAreaMax[1] || i == 0) sHeightAreaMax[1] = sec->z1;
        }
        /* counted and listed in the Z buffer: 256 counts, then the lists */
        mem_fill((u8 *)gZBuffer_2, 0, 0x200);
        i = 0;
        total = 0;
        list = gZBuffer_2 + 0x100;
        for (k = 0; k < HEIGHT_GRID_SIZE; k++) {
            for (j = 0; j < HEIGHT_GRID_SIZE; j++) {
                for (n = 0; n < sHeightAreas->count; n++) {
                    entry = (TrackModelList *)((u8 *)sHeightAreas + n * 32);
                    x = entry->e[0].x0 << 4;
                    x >>= 4;
                    x = ((x - sHeightAreaMin[0]) << 4) / (sHeightAreaMax[0] - sHeightAreaMin[0]);
                    z = entry->e[0].z0 << 4;
                    z >>= 4;
                    if (x < 0) x = 0;
                    if (x >= 16) x = 15;
                    /* z from the x minimum: an original slip, made everywhere the grid is used */
                    z = ((z - sHeightAreaMin[0]) << 4) / (sHeightAreaMax[1] - sHeightAreaMin[1]);
                    if (z < 0) z = 0;
                    if (z >= 16) z = 15;
                    cell = z * 16 + x;
                    z = cell >> 4;
                    if ((cell & 0xF) > j) continue;
                    if (z > k) continue;
                    z = entry->e[0].x1 << 4;
                    z >>= 4;
                    z = ((z - sHeightAreaMin[0]) << 4) / (sHeightAreaMax[0] - sHeightAreaMin[0]);
                    x = entry->e[0].z1 << 4;
                    x >>= 4;
                    if (z < 0) z = 0;
                    if (z >= 16) z = 15;
                    x = ((x - sHeightAreaMin[0]) << 4) / (sHeightAreaMax[1] - sHeightAreaMin[1]);
                    if (x < 0) x = 0;
                    if (x >= 16) x = 15;
                    cell = x * 16 + z;
                    z = cell >> 4;
                    if ((cell & 0xF) < j) continue;
                    if (z < k) continue;
                    gZBuffer_2[i]++;
                    total++;
                    *list++ = n;
                }
                i++;
            }
        }
        sHeightGridData = mem_alloc_dbg((total + 0x100) * 2, 0, (s32)sStrHeightC, 0xDA);
        sHeightGrid = mem_alloc_dbg(HEIGHT_GRID_SIZE * HEIGHT_GRID_SIZE * sizeof(HeightCell *), 0, (s32)sStrHeightC, 0xDB);
        n = 0;
        list = gZBuffer_2 + 0x100;
        for (i = 0; (s32)i < HEIGHT_GRID_SIZE * HEIGHT_GRID_SIZE; i++) {
            sHeightGrid[i] = (HeightCell *)(sHeightGridData + n);
            sHeightGrid[i]->count = gZBuffer_2[i];
            memcpy(sHeightGrid[i]->ids, list, sHeightGrid[i]->count * 2);
            n++;
            n += sHeightGrid[i]->count;
            list += sHeightGrid[i]->count;
        }
    } else {
        sHeightGridData = NULL;
        sHeightGrid = NULL;
    }
    sSurfacePolys = NULL;
    sAreaSurfacePolys = NULL;
    if (polys != NULL) {
        sSurfacePolys = mem_alloc_dbg(*polys * 4, 0, (s32)sStrHeightC, 0xF6);
        i = 0;
        p = (SurfacePoly *)(polys + 1);
        z = *polys;
        if (z != 0) {
            do {
                sSurfacePolys[i] = p;
                p = (SurfacePoly *)((u8 *)p + (p->count * 4 + 8));
            } while (++i < *polys);
        }
        mem_fill((u8 *)gZBuffer_2, 0, sHeightAreas->count * 2);
        i = 0;
        list = gZBuffer_2 + sHeightAreas->count;
        for (; i < sHeightAreas->count; i++) {
            for (cell = 0; cell < *polys; cell++) {
                p = sSurfacePolys[cell];
                for (n = 0; n < p->count; n++) {
                    if (height_test_area(i, p->pts[n].x << 6, 0, p->pts[n].z << 6)) break;
                }
                if (n != p->count) {
                    *list++ = cell;
                    gZBuffer_2[i]++;
                }
            }
        }
        i = 0;
        sAreaSurfacePolys = mem_alloc_dbg(sHeightAreas->count * 4, 0, (s32)sStrHeightC, 0x11C);
        mem_fill((u8 *)sAreaSurfacePolys, 0, sHeightAreas->count * 4);
        list = gZBuffer_2 + sHeightAreas->count;
        for (; i < sHeightAreas->count; i++) {
            if (gZBuffer_2[i] != 0) {
                ids = mem_alloc_dbg((gZBuffer_2[i] + 1) * 2, 0, (s32)sStrHeightC, 0x123);
                sAreaSurfacePolys[i] = ids;
                memcpy(ids, list, gZBuffer_2[i] * 2);
                list += gZBuffer_2[i];
                sAreaSurfacePolys[i][gZBuffer_2[i]] = -1;
            }
        }
    }
}

/* Frees the height grid, the surface polygons and the wall edge flags. */
void height_free(void)
{
    u32 i;
    if (gWallEdgeFlags != 0) {
        mem_free(gWallEdgeFlags);
        gWallEdgeFlags = 0;
    }
    if (sHeightGrid != 0) {
        mem_free(sHeightGrid);
        sHeightGrid = 0;
    }
    if (sHeightGridData != 0) {
        mem_free(sHeightGridData);
        sHeightGridData = 0;
    }
    if (sSurfacePolys != 0) {
        mem_free(sSurfacePolys);
        sSurfacePolys = 0;
    }
    if (sAreaSurfacePolys != 0) {
        i = 0;
        if (sHeightAreas->count != 0) {
            do {
                if (sAreaSurfacePolys[i] != 0) {
                    mem_free(sAreaSurfacePolys[i]);
                }
                i++;
            } while (i < sHeightAreas->count);
        }
        mem_free(sAreaSurfacePolys);
        sAreaSurfacePolys = 0;
    }
}

/* Looks for the ground under (x, y, z) (mm) in height area id: the triangles of the grid cell under the point,
   starting from the last one found. A bridge triangle (0x400) counts only below y + 500 and the highest wins; the
   first other one ends the search. Returns 1 when found, with the result in gGroundHeight and the other gGround
   globals. */
s32 height_test_area(s32 area, s32 x, s32 y, s32 z)
{
    TrackModel *sec;
    CollGrid *grid2;
    s32 localX, localZ;
    s32 mask, maskWord, count;
    CollTri *tri;
    Vtx *verts;
    s32 x0, z0, x1, z1;
    s32 width, depth;
    s32 i, start;
    s32 coord[3][4];
    CollGrid *grid;
    s32 dxp, dzp;
    s32 nx, ny, nz, height;
    s32 k;
    s32 cross;

    sAreaTestCount++;
    sec = (TrackModel *)((u8 *)sHeightAreas + (area * 32 + 4));
    x0 = sec->x0;
    if (x < x0 * 16) return 0;
    z0 = sec->z0;
    if (z < z0 * 16) return 0;
    x1 = sec->x1;
    if (x1 * 16 < x) return 0;
    z1 = sec->z1;
    if (z1 * 16 < z) return 0;
    grid = sec->grid;
    if (grid == NULL) return 0;
    if (!(grid->flags & COLL_GRID_VALID)) fatal_halt(1);
    localX = x;
    grid2 = grid;
    localZ = z;
    if (grid2->flags & COLL_GRID_CENTERED) {
        localX = x - (x1 + x0) * 8;
        localZ = z - (z1 + z0) * 8;
    }
    width = (x1 - x0) * 16;
    if (width == 0) return 0;
    depth = (z1 - z0) * 16;
    if (depth == 0) return 0;
    /* the point's cell of the area's 8x8 grid: the triangles have a bit per cell */
    i = ((z - z0 * 16) * 8 / depth) * 8 + (x - x0 * 16) * 8 / width;
    verts = sec->model->vtx;
    maskWord = i / 16;
    mask = 1 << (i % 16);
    start = gGroundTri;
    count = grid2->count;
    if (start < 0) start = 0;
    if (start >= count) start = 0;
    i = start;
    do {
        tri = &grid2->tris[i];
        if (tri->cells.h[maskWord] & mask) {
            coord[0][0] = verts[tri->vtx[0]].v.ob[0] * 16;
            coord[0][1] = verts[tri->vtx[1]].v.ob[0] * 16;
            coord[0][2] = verts[tri->vtx[2]].v.ob[0] * 16;
            coord[2][0] = verts[tri->vtx[0]].v.ob[2] * 16;
            dzp = localZ - coord[2][0];
            coord[2][1] = verts[tri->vtx[1]].v.ob[2] * 16;
            dxp = localX - coord[0][0];
            coord[2][2] = verts[tri->vtx[2]].v.ob[2] * 16;
            /* inside: on the same side of the three edges */
            if ((cross = dzp * (coord[0][1] - coord[0][0]) - dxp * (coord[2][1] - coord[2][0])) >= 0 &&
                (cross = (localZ - coord[2][1]) * (coord[0][2] - coord[0][1]) - (localX - coord[0][1]) * (coord[2][2] - coord[2][1])) >= 0 &&
                (cross = (localZ - coord[2][2]) * (coord[0][0] - coord[0][2]) - (localX - coord[0][2]) * (coord[2][0] - coord[2][2])) >= 0) {
                coord[1][0] = verts[tri->vtx[0]].v.ob[1] * 16;
                coord[1][1] = verts[tri->vtx[1]].v.ob[1] * 16;
                coord[1][2] = verts[tri->vtx[2]].v.ob[1] * 16;
                coord[0][1] -= coord[0][0];
                coord[0][2] -= coord[0][0];
                coord[1][1] -= coord[1][0];
                coord[1][2] -= coord[1][0];
                coord[2][1] -= coord[2][0];
                coord[2][2] -= coord[2][0];
                coord[0][1] >>= 4;
                coord[1][1] >>= 4;
                coord[2][1] >>= 4;
                coord[0][2] >>= 4;
                coord[2][2] >>= 4;
                coord[1][2] >>= 4;
                nx = coord[2][1] * coord[1][2] - coord[1][1] * coord[2][2];
                ny = coord[0][1] * coord[2][2] - coord[2][1] * coord[0][2];
                nz = coord[1][1] * coord[0][2] - coord[0][1] * coord[1][2];
                if (ny != 0) {
                    cross = -nz;
                    height = (dxp * -nx + dzp * cross) / ny + coord[1][0];
                    if (tri->flags & COLL_TRI_BRIDGE) {
                        if (height <= y + 500 && height >= sGroundBestHeight) {
                            sGroundBestHeight = height;
                            gGroundFound = 1;
                            gGroundArea = area;
                            gGroundTri = i;
                            gGroundHeight = height;
                            gGroundSurface = tri->flags;
                        }
                    } else if (sGroundBestHeight == HEIGHT_NONE) {
                        gGroundFound = 1;
                        gGroundArea = area;
                        gGroundTri = i;
                        gGroundHeight = height;
                        gGroundSurface = tri->flags;
                        /* a surface polygon over the point overrides the triangle's surface */
                        if (sAreaSurfacePolys != NULL) {
                            if (sAreaSurfacePolys[area] == NULL) return 1;
                            k = 0;
                            while (sAreaSurfacePolys[area][k] != -1) {
                                if (point_in_poly_xz(sSurfacePolys[sAreaSurfacePolys[area][k]], x, z)) {
                                    gGroundSurface = sSurfacePolys[sAreaSurfacePolys[area][k]]->surface;
                                    break;
                                }
                                k++;
                            }
                        }
                        return 1;
                    }
                }
            }
        }
        i++;
        if (i >= count) i = 0;
    } while (i != start);
    return 0;
}

/* The height of the ground under (x, y, z) (mm): the last area found first, then the areas of the point's grid
   cell. Sets gGroundFound and the other gGround globals. */
s32 height_get_ground(s32 x, s32 y, s32 z)
{
    s32 cellX, cellZ, i, cell;

    sGroundBestHeight = HEIGHT_NONE;
    gGroundFound = 0;
    gGroundSurface = 0;
    sGroundQueryCount++;
    if (gCurTrack_5->mode == TRACK_ENV_DESERT) {
        return height_get_desert(x, z);
    }
    if (gGroundArea != -1 && height_test_area(gGroundArea, x, y, z) != 0) {
        return gGroundHeight;
    }
    cellX = x >> 4;
    cellX = (cellX - sHeightAreaMin[0]) * 16 / (sHeightAreaMax[0] - sHeightAreaMin[0]);
    cellZ = z >> 4;
    if (cellX < 0) cellX = 0;
    if (cellX > 15) cellX = 15;
    cellZ = (cellZ - sHeightAreaMin[0]) * 16 / (sHeightAreaMax[1] - sHeightAreaMin[1]);
    if (cellZ < 0) cellZ = 0;
    if (cellZ > 15) cellZ = 15;
    cell = cellZ * 16 + cellX;
    for (i = 0; i < sHeightGrid[cell]->count; i++) {
        if (sHeightGrid[cell]->ids[i] != gGroundArea && height_test_area(sHeightGrid[cell]->ids[i], x, y, z) != 0) {
            return gGroundHeight;
        }
    }
    if (sGroundBestHeight != HEIGHT_NONE) {
        return sGroundBestHeight;
    }
    return gGroundHeight;
}

/* The desert's height at (x, z) (mm), interpolated over the height-map triangle; gGroundArea is the tile
   (x | z << 16, in 4-cell blocks) and gGroundTri the triangle in it. */
s32 height_get_desert(s32 x, s32 z)
{
    struct {
        s32 q;
        s32 rx;
        s32 rz;
    } L;
    s32 *qPtr;
    s32 cellX, lt, tile, fracX, fracZ, h0, h1, h2;

    qPtr = &L.q;
    gGroundFound = 1;
    divmod_floor(x, 2000, qPtr, &L.rx);
    cellX = L.q;
    divmod_floor(z, 2000, qPtr, &L.rz);
    z = L.q;
    gGroundArea = ((cellX >> 2) & 0xffff) + ((z >> 2) << 16);
    tile = ((cellX & 3) + (z & 3) * 4) * 2;
    gGroundTri = tile;
    if (L.rx < 1999 - L.rz) {
        gGroundTri = tile + 1;
    }
    cellX &= 0x1ff;
    z &= 0x1ff;
    fracX = L.rx;
    fracZ = L.rz;
    if (fracX < 1999 - fracZ) {
        h0 = gDesertHeightMap[z * 0x200 + cellX];
        h1 = gDesertHeightMap[((z + 1) & 0x1ff) * 0x200 + cellX];
        h2 = gDesertHeightMap[z * 0x200 + ((cellX + 1) & 0x1ff)];
        h0 <<= 6;
        h1 <<= 6;
        h2 <<= 6;
        L.q = h0 + ((h1 - h0) * fracZ + (h2 - h0) * fracX) / 2000;
    } else {
        h0 = gDesertHeightMap[((z + 1) & 0x1ff) * 0x200 + ((cellX + 1) & 0x1ff)];
        h1 = gDesertHeightMap[((z + 1) & 0x1ff) * 0x200 + cellX];
        h2 = gDesertHeightMap[z * 0x200 + ((cellX + 1) & 0x1ff)];
        h0 <<= 6;
        h1 <<= 6;
        h2 <<= 6;
        L.q = h0 + ((h1 - h0) * (2000 - L.rx) + (h2 - h0) * (2000 - L.rz)) / 2000;
    }
    return L.q;
}

/* height_get_desert in metres. */
float height_get_desert_f(float x, float z)
{
    s32 ix, iz, h0, h1, h2;
    float rz, f0, f2;

    x *= 0.5f;
    z *= 0.5f;
    ix = x;
    gGroundFound = 1;
    if (ix < 0) {
        ix--;
    }
    iz = z;
    x -= ix;
    if (iz < 0) {
        iz--;
    }
    gGroundArea = ((ix >> 2) & 0xffff) + ((iz >> 2) << 16);
    z -= iz;
    rz = 1.0f - z;
    gGroundTri = ((ix & 3) + (iz & 3) * 4) * 2;
    if (x < rz) {
        gGroundTri = gGroundTri + 1;
    }
    ix &= 0x1ff;
    iz &= 0x1ff;
    if (x < rz) {
        float f1;
        h0 = gDesertHeightMap[iz * 0x200 + ix];
        h1 = gDesertHeightMap[((iz + 1) & 0x1ff) * 0x200 + ix];
        h0 <<= 6;
        h1 <<= 6;
        f0 = h0;
        f1 = h1;
        f1 -= f0;
        f2 = gDesertHeightMap[iz * 0x200 + ((ix + 1) & 0x1ff)] << 6;
        return (f0 + f1 * z + (f2 - f0) * x) / 1000.0f;
    } else {
        float f1, d;
        h0 = gDesertHeightMap[((iz + 1) & 0x1ff) * 0x200 + ((ix + 1) & 0x1ff)];
        h1 = gDesertHeightMap[((iz + 1) & 0x1ff) * 0x200 + ix];
        h0 <<= 6;
        h1 <<= 6;
        f0 = h0;
        f1 = h1;
        d = f1 - f0;
        f1 = 1.0f - x;
        f2 = gDesertHeightMap[iz * 0x200 + ((ix + 1) & 0x1ff)] << 6;
        return (f0 + d * f1 + (f2 - f0) * rz) / 1000.0f;
    }
}

/* The vertex colour of the ground found last (gGroundArea, gGroundTri) at (x, z), interpolated over the
   triangle: out = {r, g, b}. */
void height_get_ground_color(s32 x, s32 z, s32 *out)
{
    s32 coord[3][4];
    TrackModel *area;
    CollGrid *grid;
    CollTri *tri;
    Vtx *verts;
    s32 ee, k, q, tt, a, a2, b, d, num, centerX, centerZ, px, pz;

    if (gGroundArea < 0 || gGroundArea >= sHeightAreas->count) return;
    area = &sHeightAreas->e[gGroundArea];
    grid = area->grid;
    if (!(grid->flags & COLL_GRID_VALID)) return;
    tri = &grid->tris[gGroundTri];
    centerX = (area->x1 + area->x0) * 8;
    centerZ = (area->z1 + area->z0) * 8;
    if (grid->flags & COLL_GRID_CENTERED) {
        px = x - centerX;
        pz = z - centerZ;
    } else {
        px = x;
        pz = z;
    }
    verts = area->model->vtx;
    coord[0][0] = verts[tri->vtx[0]].v.ob[0];
    coord[0][1] = verts[tri->vtx[1]].v.ob[0];
    coord[0][2] = verts[tri->vtx[2]].v.ob[0];
    coord[2][0] = verts[tri->vtx[0]].v.ob[2];
    coord[2][1] = verts[tri->vtx[1]].v.ob[2];
    coord[2][2] = verts[tri->vtx[2]].v.ob[2];
    coord[0][1] -= coord[0][0];
    coord[2][1] -= coord[2][0];
    coord[0][2] -= coord[0][0];
    coord[2][2] -= coord[2][0];
    coord[0][0] <<= 4;
    coord[2][0] <<= 4;
    for (k = 0; k < 3; k++) {
        coord[1][0] = verts[tri->vtx[0]].v.cn[k];
        coord[1][1] = verts[tri->vtx[1]].v.cn[k];
        coord[1][2] = verts[tri->vtx[2]].v.cn[k];
        coord[1][1] -= coord[1][0];
        coord[1][2] -= coord[1][0];
        tt = coord[2][1] * coord[1][2];
        a = coord[1][1] * coord[2][2] - tt;
        b = px - coord[0][0];
        q = b * a;
        tt = coord[1][1] * coord[0][2];
        a2 = coord[0][1] * coord[1][2] - tt;
        d = coord[0][1] * coord[2][2];
        q += (pz - coord[2][0]) * a2;
        ee = coord[2][1] * coord[0][2];
        q = q / (d - ee);
        q /= 16; q += coord[1][0];
        if (q < 0) q = 0;
        if (q > 255) q = 255;
        out[k] = q;
    }
}

#define IDX(a, b) (((((a) & 0x1FF)) << 9) | ((b) & 0x1FF))

/* The vertices (mm) of ground triangle `tri` of height area `area` (on the desert: of the height map). */
s32 height_get_tri_verts(s32 area, s32 tri, Vec3i *v)
{
    s32 cell, row, cellX, cellZ, tileX4;
    TrackModel *sec;
    CollGrid *grid;
    Vtx *vt;
    s32 k;
    u32 flags; s32 j;
    u16 *words, *entry;

    if (*(s32 *)(gCurTrack + 0x2c) == TRACK_ENV_DESERT) {
        cell = tri / 2;
        tileX4 = (area << 16) >> 14;
        row = cell / 4;
        cellX = tileX4 + (cell - row * 4);
        cellZ = (area >> 16) * 4 + row;
        if (tri & 1) {
            v[0].x = cellX * 2000;
            v[0].z = cellZ * 2000;
            v[0].y = gDesertHeightMap[IDX(cellZ, cellX)] << 6;
            v[1].x = (cellX + 1) * 2000;
            v[1].z = cellZ * 2000;
            v[1].y = gDesertHeightMap[IDX(cellZ, cellX + 1)] << 6;
            v[2].x = cellX * 2000;
            v[2].z = (cellZ + 1) * 2000;
            v[2].y = gDesertHeightMap[IDX(cellZ + 1, cellX)] << 6;
        } else {
            v[0].x = (cellX + 1) * 2000;
            v[0].z = (cellZ + 1) * 2000;
            v[0].y = gDesertHeightMap[IDX(cellZ + 1, cellX + 1)] << 6;
            v[1].x = cellX * 2000;
            v[1].z = (cellZ + 1) * 2000;
            v[1].y = gDesertHeightMap[IDX(cellZ + 1, cellX)] << 6;
            v[2].x = (cellX + 1) * 2000;
            v[2].z = cellZ * 2000;
            v[2].y = gDesertHeightMap[IDX(cellZ, cellX + 1)] << 6;
        }
        return 0;
    }

    sec = (TrackModel *)((u8 *)sHeightAreas + (area * 0x20 + 4));
    grid = sec->grid;
    flags = grid->flags;
    if (!(flags & COLL_GRID_VALID)) {
        goto hang;
    }
    words = (u16 *)grid;
    vt = sec->model->vtx;
    if (flags & COLL_GRID_CENTERED) {
        cellX = (sec->x1 + sec->x0) * 8;
        cell = (sec->z1 + sec->z0) * 8;
    } else {
        cell = 0;
        cellX = 0;
    }
    entry = words + (tri * 8 + 2); /* m->tris[p2]: its vertex indices are pv[4..6] */
    for (area = 0; area < 3; area++) {
        v[area].x = vt[entry[area + 4]].v.ob[0] * 16 + cellX;
        v[area].y = vt[entry[area + 4]].v.ob[1] * 16;
        v[area].z = vt[entry[area + 4]].v.ob[2] * 16 + cell;
    }
    return 0;
hang:
    for (;;) {
    }
}

/* The unit normal of the ground triangle found last. */
void height_get_ground_normal(f32 *out)
{
    s32 v[9];
    f32 len;
    height_get_tri_verts(gGroundArea, gGroundTri, v);
    v[3] -= v[0];
    v[6] -= v[0];
    v[4] -= v[1];
    v[5] -= v[2];
    v[7] -= v[1];
    v[8] -= v[2];
    out[0] = (f32)v[5] * (f32)v[7] - (f32)v[4] * (f32)v[8];
    out[1] = (f32)v[3] * (f32)v[8] - (f32)v[5] * (f32)v[6];
    out[2] = (f32)v[4] * (f32)v[6] - (f32)v[3] * (f32)v[7];
    len = out[0] * out[0] + out[1] * out[1] + out[2] * out[2];
    len = sqrtf(len);
    out[0] = out[0] / len;
    out[1] = out[1] / len;
    out[2] = out[2] / len;
}

/* Collects into gGroundTris (at most GROUND_MAX_TRIS) the ground triangles in the box (x0, z0)-(x1, z1) (mm), leaving out
   bridge triangles not just below hy. */
void height_collect_ground_tris(s32 x0, s32 z0, s32 x1, s32 z1, s32 hy) {

    s32 box[4];
    union { u16 h[4]; u32 w[2]; } mask;
    s32 outcode[3];
    s32 slot;
    s32 hmin;
    s32 x, z, cellX, cellZ, cx, cz, cell, k, j, n;
    s32 ox, oz, dx, dz;
    s32 bx0, bz0, bx1, bz1, bit;
    u16 f;
    TrackModel *sec;
    CollGrid *grid;
    CollTri *ctri;
    Vtx *verts;
    u16 *vidx;
    u16 *maskHalves;
    GroundTri *out;
    s32 *p;

    x0 >>= 4;
    x1 >>= 4;
    z0 >>= 4;
    z1 >>= 4;
    gGroundTriCount = 0;
    hy >>= 4;
    if (gCurTrack_4->f2C == TRACK_ENV_DESERT) {
        /* the height-map cells (125 units of 16 mm) the box covers, two triangles each */
        divmod_floor(x0, 125, &box[0], &slot);
        divmod_floor(z0, 125, &box[2], &slot);
        divmod_floor(x1 + 124, 125, &box[1], &slot);
        divmod_floor(z1 + 124, 125, &box[3], &slot);
        for (k = box[2]; k < box[3]; k++) {
            for (n = box[0]; n < box[1]; n++) {
                tri(slot, n * 2 + k * 8, n, k, n + 1, k, n + 1, k + 1);
                if (gGroundTriCount == GROUND_MAX_TRIS) return;
                tri(slot, n * 2 + k * 8, n, k, n + 1, k + 1, n, k + 1);
                if (gGroundTriCount == GROUND_MAX_TRIS) return;
            }
        }
        return;
    }
    cellX = (x0 + x1) >> 1;
    cellZ = (z0 + z1) >> 1;
    cellX = ((cellX - sHeightAreaMin[0]) << 4) / (sHeightAreaMax[0] - sHeightAreaMin[0]);
    if (cellX < 0) cellX = 0;
    if (cellX >= 16) cellX = 15;
    cellZ = ((cellZ - sHeightAreaMin[0]) << 4) / (sHeightAreaMax[1] - sHeightAreaMin[1]);
    if (cellZ < 0) cellZ = 0;
    if (cellZ >= 16) cellZ = 15;
    cell = cellZ * 16 + cellX;
    for (slot = 0; slot < sHeightGrid[cell]->count; slot++) {
        sec = (TrackModel *)((u8 *)sHeightAreas + ((sHeightGrid[cell]->ids[slot] << 5) + 4));
        if (x1 < sec->x0) continue;
        if (z1 < sec->z0) continue;
        if (sec->x1 < x0) continue;
        if (sec->z1 < z0) continue;
        if (!(sec->grid->flags & COLL_GRID_VALID)) fatal_halt(1);
        grid = sec->grid;
        if (grid->flags & COLL_GRID_CENTERED) {
            cx = (sec->x1 + sec->x0) / 2;
            cz = (sec->z1 + sec->z0) / 2;
        } else {
            cx = cz = 0;
        }
        box[0] = x0 - cx;
        box[1] = x1 - cx;
        box[2] = z0 - cz;
        box[3] = z1 - cz;
        dx = sec->x1 - sec->x0;
        ox = sec->x0;
        bx0 = (x0 - ox) * 8 / dx;
        oz = sec->z0;
        dz = sec->z1 - oz;
        if (bx0 >= 8) bx0 = 7;
        if (bx0 < 0) bx0 = 0;
        bz0 = (z0 - oz) * 8 / dz;
        if (bz0 >= 8) bz0 = 7;
        if (bz0 < 0) bz0 = 0;
        bx1 = (x1 - ox) * 8 / dx;
        if (bx1 >= 8) bx1 = 7;
        if (bx1 < 0) bx1 = 0;
        bz1 = (z1 - oz) * 8 / dz;
        if (bz1 >= 8) bz1 = 7;
        if (bz1 < 0) bz1 = 0;
        /* the cells of the area's 8x8 grid the box covers */
        maskHalves = mask.h;
        for (n = bz0; n <= bz1; n++) {
            for (k = bx0; k <= bx1; k++) {
                bit = n * 8 + k;
                maskHalves[bit / 16] |= 1 << (bit % 16);
            }
        }
        for (n = 0; n < grid->count; n++) {
            ctri = &grid->tris[n];
            if (!((ctri->cells.w[0] & mask.w[0]) || (ctri->cells.w[1] & mask.w[1]))) continue;
            if (ctri->flags & COLL_TRI_SKIP) continue;
            vidx = ctri->vtx;
            verts = sec->model->vtx;
            /* left out when all three vertices are beyond the same side of the box */
            for (k = 0; k < 3; k++) {
                outcode[k] = 0;
                if (verts[vidx[k]].v.ob[0] < box[0]) outcode[k] = 1;
                if (box[1] < verts[vidx[k]].v.ob[0]) outcode[k] |= 2;
                if (verts[vidx[k]].v.ob[2] < box[2]) outcode[k] |= 4;
                if (box[3] < verts[vidx[k]].v.ob[2]) outcode[k] |= 8;
            }
            if (outcode[0] & outcode[1] & outcode[2]) continue;
            /* a bridge triangle only when it is not wholly above hy nor wholly 250 units below it */
            if (ctri->flags & COLL_TRI_BRIDGE) {
                for (k = 0; k < 3; k++) {
                    if (!(hy < verts[vidx[k]].v.ob[1])) break;
                }
                if (k == 3) continue;
                for (k = 0; k < 3; k++) {
                    if (hy - 250 < verts[vidx[k]].v.ob[1]) break;
                }
                if (k == 3) continue;
            }
            { GroundTri *b = gGroundTris; out = &b[gGroundTriCount]; }
            out->tri = n;
            out->area = slot;
            for (k = 0; k < 3; k++) {
                gGroundTris[gGroundTriCount].v[k].x = (cx + verts[vidx[k]].v.ob[0]) * 16;
                gGroundTris[gGroundTriCount].v[k].y = verts[vidx[k]].v.ob[1] << 4;
                gGroundTris[gGroundTriCount].v[k].z = (cz + verts[vidx[k]].v.ob[2]) * 16;
            }
            if (++gGroundTriCount == GROUND_MAX_TRIS) return;
        }
    }
}

/* Moves pos (m) one unit away from the first wall edge within a metre that is not removed. */
void height_push_off_wall(f32 *pos)
{
    s32 i;
    WallEdge *edge;
    f32 len;

    height_gather_walls((pos[0] - 1.0f) * 1000.0f, (pos[2] - 1.0f) * 1000.0f,
                  (pos[0] + 1.0f) * 1000.0f, (pos[2] + 1.0f) * 1000.0f);
    for (i = 0; i < sWallEdgeCount; i++) {
        edge = sWallEdges[i];
        if (!(gWallEdgeFlags[((u8 *)edge - (u8 *)sWallMesh->edges) >> 3] & WALL_EDGE_REMOVED)) {
            len = __builtin_sqrtf(edge->nx * edge->nx + edge->nz * edge->nz);
            pos[0] += edge->nx / len;
            pos[2] += edge->nz / len;
            return;
        }
    }
}

/* The normals of a desert object's box faces (1.0 = 0x4000): four sides, then the top. */
typedef struct {
    s16 x, y, z;
} Face;

extern Face sDesertBoxNormals[];

/* height_raycast_walls on the desert: the segment against the boxes of the gathered objects. */
s32 height_raycast_desert_objs(f32 *start, f32 *end, f32 *normal, f32 *tmax)
{
    Face *bestFace;
    s32 bestDist;
    s32 ax, ay, az, bx, by, bz;
    s32 n, i;
    s32 ex, ey, ez;

    ax = (s32)(start[0] * 1000.0f);
    ay = (s32)(start[1] * 1000.0f);
    az = (s32)(start[2] * 1000.0f);
    bx = (s32)(end[0] * 1000.0f);
    by = (s32)(end[1] * 1000.0f);
    bz = (s32)(end[2] * 1000.0f);
    bestDist = 0;
    bestFace = 0;
    for (n = 0; n < sDesertObjCount; n++) {
        DesertObj *obj = &sDesertObjs[n];
        for (i = 0; i < 5; i++) {
            Face *face = &sDesertBoxNormals[i];
            s32 halfW = obj->radius;
            s32 height = obj->height;
            s32 objX = obj->x;
            s32 objY = obj->y;
            s32 objZ = obj->z;
            s32 px, py, pz;
            s32 endDist, d2, dd;
            f32 t;
            s32 dx;
            px = ((halfW * face->x) >> 14) + objX;
            py = ((height * face->y) >> 14) + objY;
            pz = ((halfW * face->z) >> 14) + objZ;

            endDist = (bx - px) * face->x + (by - py) * face->y + (bz - pz) * face->z;
            if (endDist >= bestDist) continue;
            pz = (ax - px) * face->x + (ay - py) * face->y + (az - pz) * face->z;
            if (pz < 0) continue;
            py = endDist - pz;
            if (py == 0) continue;
            dx = bx - ax;
            t = -(f32)pz / (f32)py;
            px = (f32)ax + t * (f32)dx;
            dx = by - ay;
            py = (f32)ay + t * (f32)dx;
            dx = bz - az;
            pz = (f32)az + t * (f32)dx;
            if (i < 2) {
                if (objY + height < py || pz < objZ - halfW || objZ + halfW < pz) continue;
            } else if (i < 4) {
                if (objY + height < py || px < objX - halfW || objX + halfW < px) continue;
            } else {
                if (px < objX - halfW || objX + halfW < px || pz < objZ - halfW || objZ + halfW < pz) continue;
            }
            bestFace = &sDesertBoxNormals[i];
            bestDist = endDist;
        }
    }
    if (bestFace) {
        *tmax = -((f32)bestDist / 16384000.0f);
        normal[0] = bestFace->x * 6.1035156e-05f;
        normal[1] = bestFace->y * 6.1035156e-05f;
        normal[2] = bestFace->z * 6.1035156e-05f;
        return 1;
    }
    return 0;
}

/* Pushes pos (m) out of the 3072 mm circle around each gathered desert object. */
void height_push_out_of_desert_objs(f32 *pos)
{
    s32 x, z, i, dx, dz, ax, distSq, limSq;
    f32 dist;

    x = (s32)(pos[0] * 1000.0f);
    z = (s32)(pos[2] * 1000.0f);
    height_gather_walls(x - 0x1000, z - 0x1000, x + 0x1000, z + 0x1000);
    if (sDesertObjCount > 0) {
    i = 0;
    limSq = 0x900000;
    do {
        dx = sDesertObjs[i].x - x;
        if (abs(dx) < 0xc01) {
            dz = sDesertObjs[i].z - z;
            if (abs(dz) < 0xc01) {
                distSq = dx * dx + dz * dz;
                if (distSq <= limSq) {
                    dist = sqrtf((f32)distSq);
                    x = (s32)((f32)sDesertObjs[i].x - (f32)(dx * 0xc00) / dist);
                    z = (s32)((f32)sDesertObjs[i].z - (f32)(dz * 0xc00) / dist);
                }
            }
        }
    } while (++i < sDesertObjCount);
    }
    pos[0] = (f32)x / 1000.0f;
    pos[2] = (f32)z / 1000.0f;
}

/* Tests the segment start-end (m) against the gathered walls (wantFlag: only those the camera collides with):
   returns 1 for a hit closer than *tmax, with the wall's normal in out and *tmax the hit's place on the segment. */
u8 height_raycast_walls(f32 *start, f32 *end, f32 *out, f32 *tmax, s32 wantFlag)
{
    u8 result = 0;
    f32 sx, sy, sz, ex, ey, ez;
    s32 i;
    f32 t;
    f32 vx, vy, vz;
    f32 d1, d0, px;
    WallVtx *a, *b;
    s32 iax, iaz, ibx, ibz;
    f32 unused[3];

    if (gCurTrack_5->mode == TRACK_ENV_DESERT) {
        return height_raycast_desert_objs(start, end, out, tmax);
    }
    if (sWallMesh == NULL) {
        return 0;
    }
    sx = start[0] * 1000.0f;
    sy = start[1] * 1000.0f;
    sz = start[2] * 1000.0f;
    ex = end[0] * 1000.0f;
    ey = end[1] * 1000.0f;
    ez = end[2] * 1000.0f;
    gWallHitFlags = 0;
    if (sWallEdgeCount != 0) {
        for (i = 0; i < sWallEdgeCount; i++) {
            WallEdge *e = sWallEdges[i];
            u8 fl = gWallEdgeFlags[(e - sWallMesh->edges)];
            f32 ax, az, fax, faz, dz, dx, py, y0, y1, len;

            if (fl & WALL_EDGE_REMOVED) continue;
            if (wantFlag && !(fl & WALL_EDGE_CAMERA)) continue;
            a = &sWallMesh->vtx[e->v0];
            iax = a->x;
            fax = iax;
            iaz = a->z;
            faz = iaz;
            ax = fax * 64.0f;
            az = faz * 64.0f;
            b = &sWallMesh->vtx[e->v1];
            ibz = b->z;
            dz = ibz - iaz;
            ibx = b->x;
            dx = iax - ibx;
            vx = ex - ax;
            vz = ez - az;
            d1 = vx * dz + vz * dx;
            if (d1 >= 0.0f) continue;
            vx = sx - ax;
            vz = sz - az;
            d0 = vx * dz + vz * dx;
            if (d0 < 0.0f) continue;
            t = d0 / (d0 - d1);
            if (*tmax < t) continue;
            px = start[0] + (end[0] - start[0]) * t;
            py = start[1] + (end[1] - start[1]) * t;
            vz = start[2] + (end[2] - start[2]) * t;
            px *= 15.625f;
            py *= 1000.0f;
            vz *= 15.625f;
            if (__builtin_abs(e->nz) > __builtin_abs(e->nx)) {
                if (px < fax && px < ibx) continue;
                if (fax < px && ibx < px) continue;
                vz = (px - fax) / (ibx - fax);
            } else {
                if (vz < faz && vz < ibz) continue;
                if (faz < vz && ibz < vz) continue;
                vz = (vz - faz) / (ibz - faz);
            }
            y0 = a->y + vz * (b->y - a->y);
            y0 *= 16.0f;
            y1 = a->h + vz * (b->h - a->h);
            y1 *= 16.0f;
            if (py < y0) continue;
            if (y0 + y1 <= py) continue;
            *tmax = t;
            len = __builtin_sqrtf(dz * dz + dx * dx);
            out[0] = dz / len;
            out[1] = 0.0f;
            out[2] = dx / len;
            result = 1;
            sWallHitEdge = e;
            gWallHitFlags = gWallEdgeFlags[(e - sWallMesh->edges)];
            sWallHitType = (gWallHitFlags >> 1) & 3;
        }
    }
    if (sWallTriCount != 0) {
        for (i = 0; i < sWallTriCount; i++) {
            WallTri *tr = sWallTris[i];
            WallVtx *v = sWallMesh->vtx;
            f32 fax, faz, ax, ay, az, nx, ny, nz, c, len;
            WallVtx *cc;
            s32 pos, neg, jz;
            a = &v[tr->v[0]];

            ibx = a->x;
            fax = ibx;
            ax = fax * 64.0f;
            ay = (a->y + a->h) * 16.0f;
            jz = a->z;
            faz = jz;
            az = faz * 64.0f;
            nx = tr->nx;
            ny = tr->ny;
            nz = tr->nz;
            b = &v[tr->v[1]];
            vx = ex - ax;
            vy = ey - ay;
            vz = ez - az;
            d1 = vx * nx + vy * ny + vz * nz;
            cc = &v[tr->v[2]];
            if (d1 >= 0.0f) continue;
            vx = sx - ax;
            vy = sy - ay;
            vz = sz - az;
            d0 = vx * nx + vy * ny + vz * nz;
            if (d0 < 0.0f) continue;
            t = d0 / (d0 - d1);
            if (*tmax < t) continue;
            pos = neg = 0;
            px = (sx + (ex - sx) * t) * 0.015625f;
            vz = (sz + (ez - sz) * t) * 0.015625f;
            c = (b->x - ibx) * (vz - faz) - (b->z - jz) * (px - fax);
            if (c > 0.0f) pos++; else if (c < 0.0f) neg++;
            c = (cc->x - b->x) * (vz - b->z) - (cc->z - b->z) * (px - b->x);
            if (c > 0.0f) pos++; else if (c < 0.0f) neg++;
            c = (a->x - cc->x) * (vz - cc->z) - (a->z - cc->z) * (px - cc->x);
            if (c > 0.0f) pos++; else if (c < 0.0f) neg++;
            if (pos == 3 || neg == 3) {
                len = __builtin_sqrtf(tr->nx * tr->nx + tr->ny * tr->ny + tr->nz * tr->nz);
                out[0] = tr->nx / len;
                out[1] = tr->ny / len;
                out[2] = tr->nz / len;
                result = 1;
                *tmax = t;
            }
        }
    }
    return result;
}


/* Gathers the wall edges and triangles of the grid cells around the box (x0, z0)-(x1, z1) (mm) into sWallEdges
   and sWallTris (at most WALL_MAX_GATHERED each; bit 0 of nz marks the ones already taken); on the desert, the objects of the
   tiles there into sDesertObjs (at most 8). */
void height_gather_walls(s32 x0, s32 z0, s32 x1, s32 z1)
{
    s32 px[3];
    s32 py[3];
    s32 pz[3];
    s32 qz[3];
    s32 i, j, k, m;
    s32 ix0, iz0, ix1, iz1;
    s32 count, outcode, v, lo, hi;
    WallCell *cell;
    WallEdge *e;
    WallTri *t;
    WallVtx *a, *b;

    sDesertObjCount = 0;
    if (gCurTrack_5->mode == TRACK_ENV_DESERT) {
        sDesertObjCount = 0;
        for (i = 0; i < DESERT_NUM_TILES; i++) {
            if (gDesertTiles[i].sp != 0) continue;
            if (x1 < gDesertTiles[i].x * DESERT_TILE_MM) continue;
            if (z1 < gDesertTiles[i].z * DESERT_TILE_MM) continue;
            if ((gDesertTiles[i].x + 1) * DESERT_TILE_MM < x0) continue;
            if ((gDesertTiles[i].z + 1) * DESERT_TILE_MM < z0) continue;
            for (j = 0; j < gDesertTiles[i].numObjs; j++) {
                if (x1 >= gDesertTiles[i].objs[j].x - gDesertTiles[i].objs[j].radius
                    && z1 >= gDesertTiles[i].objs[j].z - gDesertTiles[i].objs[j].radius
                    && gDesertTiles[i].objs[j].x + gDesertTiles[i].objs[j].radius >= x0
                    && gDesertTiles[i].objs[j].z + gDesertTiles[i].objs[j].radius >= z0) {
                    sDesertObjs[sDesertObjCount] = gDesertTiles[i].objs[j];
                    sDesertObjCount++;
                    if (sDesertObjCount == DESERT_TILE_OBJS) return;
                }
            }
            if (sDesertObjCount == DESERT_TILE_OBJS) return;
        }
    } else if (sWallMesh != NULL) {
        sWallEdgeCount = 0;
        sWallTriCount = 0;
        ix0 = ((x0 / 16 - sWallMesh->minX) * 16) / (sWallMesh->maxX - sWallMesh->minX);
        iz0 = ((z0 / 16 - sWallMesh->minZ) * 16) / (sWallMesh->maxZ - sWallMesh->minZ);
        ix1 = ((x1 / 16 - sWallMesh->minX) * 16) / (sWallMesh->maxX - sWallMesh->minX);
        iz1 = ((z1 / 16 - sWallMesh->minZ) * 16) / (sWallMesh->maxZ - sWallMesh->minZ);
        if (ix0 > 16 || iz0 > 16 || ix1 < 0 || iz1 < 0) return;
        if (ix0 < 0) ix0 = 0;
        if (ix0 > 15) ix0 = 15;
        if (iz0 < 0) iz0 = 0;
        if (iz0 > 15) iz0 = 15;
        if (ix1 > 15) ix1 = 15;
        if (iz1 > 15) iz1 = 15;
        for (i = ix0; i <= ix1; i++) {
            for (j = iz0; j <= iz1; j++) {
                cell = &sWallMesh->cells[j * 16 + i];
                count = (u32)sWallMesh->cells[j * 16 + i + 1].edges - (u32)cell->edges;
                count = (u32)count >> 1;
                for (k = 0; k < count; k++) {
                    if (sWallEdgeCount >= WALL_MAX_GATHERED) break;
                    e = &sWallMesh->edges[cell->edges[k]];
                    if (e->nz & 1) continue;
                    a = &sWallMesh->vtx[e->v0];
                    b = &sWallMesh->vtx[e->v1];
                    if (a->x < b->x) {
                        if ((a->x << 6) >= x1 || x0 >= (b->x << 6)) continue;
                    } else {
                        if ((b->x << 6) >= x1 || x0 >= (a->x << 6)) continue;
                    }
                    if (a->z < b->z) {
                        if ((a->z << 6) >= z1 || z0 >= (b->z << 6)) continue;
                    } else {
                        if ((b->z << 6) >= z1 || z0 >= (a->z << 6)) continue;
                    }
                    {
                        {
                            sWallEdges[sWallEdgeCount] = e;
                            sWallEdgeCount++;
                            e->nz |= 1;
                        }
                    }
                }
                cell = &sWallMesh->cells[j * 16 + i];
                count = (u32)sWallMesh->cells[j * 16 + i + 1].tris - (u32)cell->tris;
                count = (u32)count >> 1;
                for (k = 0; k < count + ((count & 1) >> 1); k++) {
                    if (sWallTriCount >= WALL_MAX_GATHERED) break;
                    t = &sWallMesh->tris[cell->tris[k]];
                    if (t->nz & 1) continue;
                    outcode = 0xF;
                    for (m = 0; m < 3; m++) {
                        a = &sWallMesh->vtx[t->v[m]];
                        v = a->x << 6;
                        if (v >= x0) outcode &= ~1;
                        if (x1 >= v) outcode &= ~2;
                        v = a->z << 6;
                        if (v >= z0) outcode &= ~4;
                        if (z1 >= v) outcode &= ~8;
                    }
                    if (outcode == 0) {
                        sWallTris[sWallTriCount++] = t;
                        t->nz |= 1;
                    }
                }
            }
        }
        for (i = 0; i < sWallEdgeCount; i++) {
            sWallEdges[i]->nz &= ~1;
        }
        for (i = 0; i < sWallTriCount; i++) {
            sWallTris[i]->nz &= ~1;
        }
        if (gDrawFlags & DRAW_COLLISION) { /* the debug view of the walls: its drawing is gone */
            for (i = 0; i < sWallEdgeCount; i++) {
                px[0] = sWallMesh->vtx[sWallEdges[i]->v0].x << 6;
                px[1] = sWallMesh->vtx[sWallEdges[i]->v0].x << 6;
                px[2] = sWallMesh->vtx[sWallEdges[i]->v1].x << 6;
                pz[0] = sWallMesh->vtx[sWallEdges[i]->v0].z << 6;
                pz[1] = sWallMesh->vtx[sWallEdges[i]->v0].z << 6;
                pz[2] = sWallMesh->vtx[sWallEdges[i]->v1].z << 6;
                height_get_ground(px[0], 10000, pz[0]);
                height_get_ground(px[2], 10000, pz[2]);
                py[0] = sWallMesh->vtx[sWallEdges[i]->v0].y * 16;
                py[1] = (sWallMesh->vtx[sWallEdges[i]->v0].y + sWallMesh->vtx[sWallEdges[i]->v0].h) * 16;
                py[2] = sWallMesh->vtx[sWallEdges[i]->v1].y * 16;
                px[0] = sWallMesh->vtx[sWallEdges[i]->v0].x << 6;
                px[1] = sWallMesh->vtx[sWallEdges[i]->v1].x << 6;
                px[2] = sWallMesh->vtx[sWallEdges[i]->v1].x << 6;
                pz[0] = sWallMesh->vtx[sWallEdges[i]->v0].z << 6;
                pz[1] = sWallMesh->vtx[sWallEdges[i]->v1].z << 6;
                pz[2] = sWallMesh->vtx[sWallEdges[i]->v1].z << 6;
                py[0] = (sWallMesh->vtx[sWallEdges[i]->v0].y + sWallMesh->vtx[sWallEdges[i]->v0].h) * 16;
                py[1] = (sWallMesh->vtx[sWallEdges[i]->v1].y + sWallMesh->vtx[sWallEdges[i]->v1].h) * 16;
            }
            for (i = 0; i < sWallTriCount; i++) {
                px[0] = sWallMesh->vtx[sWallTris[i]->v[0]].x << 6;
                px[1] = sWallMesh->vtx[sWallTris[i]->v[1]].x << 6;
                px[2] = sWallMesh->vtx[sWallTris[i]->v[2]].x << 6;
                py[0] = (sWallMesh->vtx[sWallTris[i]->v[0]].y + sWallMesh->vtx[sWallTris[i]->v[0]].h) * 16;
                py[1] = (sWallMesh->vtx[sWallTris[i]->v[1]].y + sWallMesh->vtx[sWallTris[i]->v[1]].h) * 16;
                py[2] = (sWallMesh->vtx[sWallTris[i]->v[2]].y + sWallMesh->vtx[sWallTris[i]->v[2]].h) * 16;
                qz[0] = sWallMesh->vtx[sWallTris[i]->v[0]].z << 6;
                qz[1] = sWallMesh->vtx[sWallTris[i]->v[1]].z << 6;
                qz[2] = sWallMesh->vtx[sWallTris[i]->v[2]].z << 6;
            }
        }
    }
}

/* Sets up a wall mesh resource (relocating it first when `reloc`): the edge flags (gWallEdgeFlags; an edge whose
   nx is -0x8000 brings its flags in nz) and the edges' normals. */
void height_init_walls(WallMesh **handle, s32 reloc)
{
    WallMesh *mesh = *handle;
    s32 i;
    s32 dx;
    s32 dz;
    s32 len;

    if (reloc != 0) {
        mesh->vtx = (WallVtx *)((s32)mesh->vtx + (s32)mesh);
        mesh->edges = (WallEdge *)((s32)mesh->edges + (s32)mesh);
        mesh->tris = (WallTri *)((s32)mesh->tris + (s32)mesh);
        for (i = 0; i < 0x101; i++) {
            mesh->cells[i].tris = (u16 *)((s32)mesh->cells[i].tris + (s32)mesh);
            mesh->cells[i].edges = (u16 *)((s32)mesh->cells[i].edges + (s32)mesh);
        }
    }
    gWallEdgeFlags = mem_alloc_dbg(mesh->numEdges, 0, sStrHeightC, 0x87F);
    mem_fill(gWallEdgeFlags, 0, mesh->numEdges);
    for (i = 0; i < mesh->numEdges; i++) {
        gWallEdgeFlags[i] |= 1;
        if (mesh->edges[i].nx == -0x8000) {
            gWallEdgeFlags[i] = ((u8 *)&mesh->edges[i].nz)[1] ^ 1;
        }
        dx = mesh->vtx[mesh->edges[i].v1].x - mesh->vtx[mesh->edges[i].v0].x;
        dz = mesh->vtx[mesh->edges[i].v1].z - mesh->vtx[mesh->edges[i].v0].z;
        len = isqrt_u32_4(dx * dx + dz * dz);
        if (len != 0) {
            dx = (dx * 0x4000) / len;
            dz = (dz * 0x4000) / len;
        }
        mesh->edges[i].nx = dz;
        mesh->edges[i].nz = -dx;
    }
    for (i = 0; i < mesh->numEdges; i++) {
        mesh->edges[i].nz &= ~1;
    }
    for (i = 0; i < mesh->numTris; i++) {
        mesh->tris[i].nz &= ~1;
    }
}

/* The "WAL" resource type: relocates a wall mesh and sets it up. */
void height_init_walls_reloc(WallMesh **handle)
{
    height_init_walls(handle, 1);
}

/* Frees the wall edge flags. */
void height_free_walls(void)
{
    mem_free(gWallEdgeFlags);
    gWallEdgeFlags = 0;
}

/* Whether (x, z) lies inside the triangle (xs[i], zs[i]), counter-clockwise. */
s32 point_in_tri_xz(s32 *xs, s32 *zs, s32 x, s32 z)
{
    s32 z0 = zs[0];
    s32 x1 = xs[1];
    s32 x0 = xs[0];
    s32 z1 = zs[1];
    s32 x2;
    s32 z2;

    if ((z - z0) * (x1 - x0) - (x - x0) * (z1 - z0) <= 0) {
        return 0;
    }
    x2 = xs[2];
    z2 = zs[2];
    if ((z - z1) * (x2 - x1) - (x - x1) * (z2 - z1) <= 0) {
        return 0;
    }
    return (z - z2) * (x0 - x2) - (x - x2) * (z0 - z2) > 0;
}

/* The cell of the 16x16 height grid under (x, z) (mm). */
int height_get_grid_cell(int x, int z)
{
    int cellX, cellZ;

    cellZ = z >> 4;
    cellX = x >> 4;
    cellX = (cellX - sHeightAreaMin[0]) * 16 / (sHeightAreaMax[0] - sHeightAreaMin[0]);
    if (cellX < 0) cellX = 0;
    if (cellX > 15) cellX = 15;
    cellZ = (cellZ - sHeightAreaMin[0]) * 16 / (sHeightAreaMax[1] - sHeightAreaMin[1]);
    if (cellZ < 0) cellZ = 0;
    if (cellZ > 15) cellZ = 15;
    return cellZ * 16 + cellX;
}

/* Whether (px, pz) (mm) lies inside a surface polygon: on the same side of all its edges. */
s32 point_in_poly_xz(SurfacePoly *poly, s32 px, s32 pz)
{
    s32 pos = 0;
    s32 neg;
    s32 i = 0, j, n, cross;

    n = poly->count;
    neg = 0;
    px = px >> 6;
    pz = pz >> 6;
    if (n != 0) {
        do {
            j = (i + 1) % n;
            cross = (pz - poly->pts[i].z) * (poly->pts[j].x - poly->pts[i].x);
            cross -= (px - poly->pts[i].x) * (poly->pts[j].z - poly->pts[i].z);
            if (cross >= 0) {
                pos++;
            }
            if (cross <= 0) {
                neg++;
            }
            n = poly->count;
            i++;
        } while (i < n);
    }
    if (pos == poly->count) {
        goto ret1;
    }
    if (neg != poly->count) {
        goto ret0;
    }
ret1:
    return 1;
ret0:
    return 0;
}

/* Whether the segment D_800E8518 crosses the line z = `z` between x = lo and x = hi. */
s32 height_seg_cross_line_z(s32 lo, s32 hi, s32 z)
{
    s32 t, v;

    if (z >= D_800E8518.b && z >= D_800E8518.d) {
        return 0;
    }
    if (z < D_800E8518.b && z < D_800E8518.d) {
        return 0;
    }
    t = ((z - D_800E8518.b) << 8) / (D_800E8518.d - D_800E8518.b);
    v = D_800E8518.a + ((D_800E8518.c - D_800E8518.a) * t) / 256;
    if (v < lo || hi < v) {
        return 0;
    }
    return 1;
}

/* Whether the segment D_800E8518 crosses the line x = `x` between z = lo and z = hi. */
s32 height_seg_cross_line_x(s32 lo, s32 hi, s32 x)
{
    s32 t, v;

    if (x >= D_800E8518.a && x >= D_800E8518.c) {
        return 0;
    }
    if (x < D_800E8518.a && x < D_800E8518.c) {
        return 0;
    }
    t = ((x - D_800E8518.a) << 8) / (D_800E8518.c - D_800E8518.a);
    v = D_800E8518.b + ((D_800E8518.d - D_800E8518.b) * t) / 256;
    if (v < lo || hi < v) {
        return 0;
    }
    return 1;
}
