#include "game.h"

f32 path_get_distance(s32 a, s32 b)
{
    f32 acc;
    f32 K;
    f32 w, d, dx, dz;
    s32 i;

    acc = 0.0f;
    K = 10000.0f;
    for (;;) {
        if (a == b) {
            return acc;
        }
        if (K <= acc) {
            return acc;
        }
        w = gPathNodes[a].length;
        if (gPathNodes[a].branch >= 0) {
            dx = gPathNodes[a].pos.x - gPathNodes[gPathNodes[a].branch].pos.x;
            dz = gPathNodes[a].pos.z - gPathNodes[gPathNodes[a].branch].pos.z;
            d = __builtin_sqrtf(dx * dx + dz * dz);
            break;
        }
        a = gPathNodes[a].next;
        acc += w;
    }
    i = gPathNodes[a].next;
    while (gPathNodes[i].join < 0) {
        if (i == b) {
            return acc + w + path_get_distance(gPathNodes[a].next, b);
        }
        i = gPathNodes[i].next;
    }
    i = gPathNodes[a].branch;
    while (gPathNodes[i].join < 0) {
        if (i == b) {
            return acc + d + path_get_distance(gPathNodes[a].branch, b);
        }
        i = gPathNodes[i].next;
    }
    d += path_get_distance(gPathNodes[a].branch, b);
    w += path_get_distance(gPathNodes[a].next, b);
    if (d < w) {
        return d + acc;
    }
    return w + acc;
}

void path_calc_normals(void)
{
    s32 i;
    PathNode *p;
    PathNode *a;
    PathNode *c;
    f32 sz;
    f32 sx;
    f32 len;

    for (i = 0; i < gPathNodeCount; i++) {
        p = (PathNode *)(i * sizeof(PathNode) + (u32)gPathNodes);
        a = (PathNode *)(p->next * sizeof(PathNode) + (u32)gPathNodes);
        c = (PathNode *)(p->prev * sizeof(PathNode) + (u32)gPathNodes);
        sz = (a->pos.z - p->pos.z) / p->length;
        sz = sz * p->length + ((p->pos.z - c->pos.z) / c->length) * c->length;
        sx = -(a->pos.x - p->pos.x) / p->length;
        sx = sx * p->length + (-(p->pos.x - c->pos.x) / c->length) * c->length;
        len = __builtin_sqrtf(sz * sz + sx * sx);
        if (0.0f < len) {
            gPathNodes[i].lateralX = sz / len;
            gPathNodes[i].lateralZ = sx / len;
        }
    }
}

void path_calc_spline_coeffs(s32 idx, s32 stop)
{
    PathNode *a, *b, *base;
    f32 inv, t, P, Q, R, S, nb, nq, f7, f8, f4, f5;
    s32 par;

    do {
    base = gPathNodes;
    a = (PathNode *)(idx * 0x6c + (s32)base);
    inv = a->length / 3.0f;
    t = -a->lateralZ;
    S = a->lateralX;
    par = a->next;
    b = (PathNode *)(par * 0x6c + (s32)base);
    P = a->pos.x + t * inv;
    Q = a->pos.z + S * inv;
    nb = b->pos.x - -b->lateralZ * inv;
    nq = b->pos.z - b->lateralX * inv;
    a->splineX[2] = t * 3.0f * inv;
    a->splineX[1] = (nb - P) * 3.0f - a->splineX[2];
    a->splineX[0] = b->pos.x - a->pos.x - a->splineX[1] - a->splineX[2];
    a->splineZ[2] = S * 3.0f * inv;
    a->splineZ[1] = (nq - Q) * 3.0f - a->splineZ[2];
    a->splineZ[0] = b->pos.z - a->pos.z - a->splineZ[1] - a->splineZ[2];
    if (a->branch >= 0) {
        path_calc_spline_coeffs(a->branch, stop);
    }
    idx = par;
    } while (idx != stop);
}

void path_calc_slope_and_bank(s32 i, s32 end)
{
    Vec3f a;
    Vec3f b;
    f32 p;
    f32 q;
    f32 fa;
    f32 fb;
    s32 nx;

    do {
        nx = gPathNodes[i].next;
        gPathNodes[i].slope = (gPathNodes[nx].pos.y - gPathNodes[i].pos.y) / gPathNodes[i].length;
        a = gPathNodes[i].pos;
        b = a;
        p = gPathNodes[i].lateralX * 0.25f;
        q = -gPathNodes[i].lateralZ * 0.25f;
        a.x = a.x + p;
        a.z = a.z - q;
        b.x = b.x - p;
        b.z = b.z + q;
        fa = bike_get_ground_height((f32 *)&a);
        fb = bike_get_ground_height((f32 *)&b);
        gPathNodes[i].bank = (fa - fb) * 2.0f;
        if (gPathNodes[i].branch >= 0) {
            path_calc_slope_and_bank(gPathNodes[i].branch, end);
        }
        i = nx;
    } while (i != end);
}

void path_calc_curvature(s32 start, s32 stop)
{
    s32 p = start;
    f32 v, u, t, x, y, sq, n, n2, r, d, q1, q2, aq;
    PathNode *m;

    do {
        x = gPathNodes[p].splineX[2];
        y = gPathNodes[p].splineZ[2];
        sq = x * x + y * y;
        t = 2.0f * gPathNodes[p].splineZ[1];
        u = x * t;
        t = 2.0f * gPathNodes[p].splineX[1];
        n = u - y * t;
        r = __builtin_sqrtf(sq);
        d = sq * r;
        m = (PathNode *)(gPathNodes[p].prev * sizeof(PathNode) + (u32)gPathNodes);
        x = m->splineX[0] * 3.0f + 2.0f * m->splineX[1] + m->splineX[2];
        y = m->splineZ[0] * 3.0f + 2.0f * m->splineZ[1] + m->splineZ[2];
        sq = x * x + y * y;
        q1 = n / d;
        t = 2.0f * m->splineZ[1] + m->splineZ[0] * 6.0f;
        v = x * t;
        t = 2.0f * m->splineX[1] + m->splineX[0] * 6.0f;
        n2 = v - y * t;
        r = __builtin_sqrtf(sq);
        q2 = n2 / (sq * r);
        aq = q2;
        if (!(q2 > 0.0f)) {
            aq = -q2;
        }
        if (q1 > 0.0f ? q1 < aq : -q1 < aq) {
            gPathNodes[p].curvature = q2;
        } else {
            gPathNodes[p].curvature = q1;
        }
        p = gPathNodes[p].next;
    } while (p != stop);
}

u8 path_find_crest_ahead(f32 x, s32 idx)
{
    s32 c;
    s32 A, B;
    f32 d1, d2, v;
    x = x * x;
    for (c = 0; c < 5; c++) {
        A = gPathNodes[idx].next;
        B = gPathNodes[idx].prev;
        d1 = (gPathNodes[idx].pos.y - gPathNodes[A].pos.y) / gPathNodes[A].length;
        d2 = (gPathNodes[B].pos.y - gPathNodes[idx].pos.y) / gPathNodes[c].length;
        v = -(((d2 - d1) + (d2 - d1)) / (gPathNodes[A].length + gPathNodes[c].length)) * x;
        if (7.3500004f < v) return c & 0xFF;
        idx = A;
    }
    return 0xFF;
}

void path_calc_curvatures(void)
{
    s32 i, j, cnt;
    f32 rem;
    PathNode *base;

    for (i = 0; i < gPathNodeCount; i++) {
        path_calc_curvature(i, gPathNodes[i].next);
    }
    for (i = 0; i < gPathNodeCount; i++) {
        cnt = 0;
        j = i;
        rem = 0.06f;
        base = gPathNodes;
        while (rem > 0.0f) {
            if (base[j].branch != -1) {
                cnt++;
                break;
            }
            rem -= ((base[j].curvature > 0.0f) ? base[j].curvature : -base[j].curvature) + 0.006f;
            j = base[j].next;
            cnt++;
        }
        gPathNodes[i].lookahead = cnt - 1;
    }
}

void path_generate_straight(void)
{
    s32 i, s, a1;
    f32 R, t;
    Vec3f v, w, m;

    gPathNodes = mem_alloc_dbg(540000, 0, sStrPathFile, 0x178);
    mem_fill((u8 *)gPathNodes, 0, 540000);
    R = 30.0f;
    m.x = 0.0f;
    m.y = 0.0f;
    m.z = 0.0f;
    for (i = 0; i < 1000; i++) {
        gPathNodes[i].prev = i - 1;
        gPathNodes[i].next = i + 1;
        gPathNodes[i].alt = -1;
        gPathNodes[i].join = -1;
        gPathNodes[i].branch = -1;
        t = 3.0f;
        do {
            t -= 1.0f;
            v.x = 0.0f;
            v.y = 40.0f;
            v.z = R - t;
            v.y = bike_get_ground_height((float *)&v);
            if (gGroundFound == 0) {
                break;
            }
            height_get_ground_normal((float *)&w);
        } while (w.z * m.z < -0.01f && 1.1f < t);
        m = w;
        gPathNodes[i].pos = v;
        gPathNodes[i].length = t;
        gPathNodes[i].speedTweak = 0;
        for (s = 3; s >= 0; s--) {
            v.x = s * 2.178f;
            bike_get_ground_height((float *)&v);
            switch ((gGroundSurface >> 4) & 0xF) {
            case 2:
            case 3:
                a1 = 1;
                break;
            case 8:
                a1 = 2;
                break;
            case 7:
                a1 = 3;
                break;
            default:
                a1 = 0;
                break;
            }
            gPathNodes[i].speedTweak |= a1 << (s * 2);
        }
        R -= t;
        gPathNodes[i].maxSpeed = 50.0f;
        gPathNodes[i].widthNeg = 7.284f;
        gPathNodes[i].widthPos = 0.75f;
        if (gGroundFound == 0) {
            break;
        }
    }
    gPathNodes[0].prev = i;
    gPathNodes[i].next = 0;
    gPathNodeCount = i + 1;
    gPathStartNode = 1;
    gPathFinishNode = 0;
    path_calc_normals();
    path_calc_spline_coeffs(0, 0);
    path_calc_slope_and_bank(0, 0);
    path_calc_curvatures();
    for (i = 0; i < gPathNodeCount; i++) {
        gPathNodes[i].maxSpeed = 50.0f;
        gPathNodes[i].lateralX = -1.0f;
    }
}

void path_load(u32 *p)
{
    u32 w;
    u32 flags;
    s32 stride;
    s32 i;
    PathResNode *r;
    u8 *e;
    f32 v;
    f32 dx, dz;

    w = *p;
    stride = 0x20;
    if ((w >> 24) == 0xAA) {
        flags = w;
        flags &= 0xFFFFFF;
        p++;
        if (w & 1) stride = 0x28;
        if (w & 2) stride += 1;
        if (w & 4) stride += 2;
        if (w & 8) stride += 2;
        stride = (stride + 3) & ~3;
    }
    gPathNodeCount = *p;
    if (flags & 0x20) {
        gPathStartNode = p[1];
        gPathFinishNode = p[2];
        p += 2;
    } else {
        gPathFinishNode = 0;
        gPathStartNode = 0;
    }
    i = 0;
    gPathNodes = mem_alloc_dbg(gPathNodeCount * sizeof(PathNode), 0, (s32)sStrPathFile, 0x20C);
    mem_fill((u8 *)gPathNodes, 0, gPathNodeCount * sizeof(PathNode));
    r = (PathResNode *)(p + 1);
    for (; i < gPathNodeCount; i++) {
        mem_fill((u8 *)&gPathNodes[i], 0, sizeof(PathNode));
        gPathNodes[i].alt = r->alt;
        gPathNodes[i].prev = r->prev;
        gPathNodes[i].join = r->join;
        gPathNodes[i].next = r->next;
        gPathNodes[i].branch = r->branch;
        gPathNodes[i].pos.x = (f32)r->x / 1000.0f;
        gPathNodes[i].pos.z = (f32)r->z / 1000.0f;
        if (gPathNodes[i].prev < i) {
            gPathNodes[i].pos.y = gPathNodes[gPathNodes[i].prev].pos.y + 3.0f;
            if (gCurTrack == &gTracks[TRACK_LEFTYS_MILL]) {
                gPathNodes[i].pos.y = gPathNodes[i].pos.y + 4.0f;
            }
        } else {
            gPathNodes[i].pos.y = 0.0f;
        }
        gPathNodes[i].pos.y = bike_get_ground_height((f32 *)&gPathNodes[i]);
        gPathNodes[i].maxSpeed = r->maxSpeed;
        gPathNodes[i].respawnDir = r->respawnDir;
        gPathNodes[i].speedTweak = r->speedTweak;
        gPathNodes[i].camCeiling = r->camCeiling;
        gPathNodes[i].camWidthNeg = r->camWidthNeg;
        gPathNodes[i].camWidthPos = r->camWidthPos;
        gPathNodes[i].unk_68 = r->unk_68 / 1000.0f;
        if (gPathNodes[i].maxSpeed < gPathNodes[i].unk_68) {
            gPathNodes[i].unk_68 = 0.0f;
        }
        e = (u8 *)r + 0x20;
        if (flags & 1) {
            gPathNodes[i].widthNeg = (f32)r->widthNeg / 62.5f - 1.0f;
            if (gPathNodes[i].widthNeg < 0.0f) gPathNodes[i].widthNeg = 0.0f;
            gPathNodes[i].widthPos = (f32)r->widthPos / 62.5f - 1.0f;
            if (gPathNodes[i].widthPos < 0.0f) gPathNodes[i].widthPos = 0.0f;
            e = (u8 *)r + 0x28;
            gPathNodes[i].fogRange = r->fogRange;
        }
        if (flags & 0x10) {
            if (flags & 4) {
                gPathNodes[i].lapProgress = *e++ << 8;
                gPathNodes[i].lapProgress |= *e++;
            }
            if (flags & 8) e += 2;
            if (flags & 2) gPathNodes[i].flags = *e;
        } else {
            if (flags & 2) gPathNodes[i].flags = *e++;
            if (flags & 4) {
                gPathNodes[i].lapProgress = *e++ << 8;
                gPathNodes[i].lapProgress |= *e++;
            }
        }
        r = (PathResNode *)((u8 *)r + stride);
    }
    for (i = 0; i < gPathNodeCount; i++) {
        flags = gPathNodes[i].next;
        dx = gPathNodes[flags].pos.x - gPathNodes[i].pos.x;
        dz = gPathNodes[flags].pos.z - gPathNodes[i].pos.z;
        gPathNodes[i].length = __builtin_sqrtf(dx * dx + dz * dz);
    }
    path_calc_normals();
    path_calc_spline_coeffs(0, 0);
    path_calc_slope_and_bank(0, 0);
    path_calc_curvatures();
    gPathLapLength = path_get_distance(gPathFinishNode, gPathNodes[gPathFinishNode].prev)
               + gPathNodes[gPathNodes[gPathFinishNode].prev].length;
    path_calc_corner_speeds(0, gPathNodeCount - 1);
    path_apply_braking_limits(0, gPathNodes[0].prev);
    for (i = 0; i < gPathNodeCount; i++) {
    }
}

void path_calc_corner_speed(s32 idx)
{
    f32 f;
    f32 w;
    f32 v;

    f = gPathNodes[idx].curvature;
    if ((f > 0.0f) ? (f > 0.005f) : (-f > 0.005f)) {
        w = gPathNodes[idx].curvature;
        if (w > 0.0f) {
            v = gPathNodes[idx].bank * 14.700001f + 19.11f;
            w = v / w;
        } else {
            v = 19.11f - gPathNodes[idx].bank * 14.700001f;
            v = -v;
            w = v / w;
        }
        if (w < 1.0f) {
            w = 1.0f;
        }
        gPathNodes[idx].maxSpeed = __builtin_sqrtf(w);
    } else {
        gPathNodes[idx].maxSpeed = 50.0f;
    }
    if (gPathNodes[idx].flags & 4) {
        gPathNodes[idx].maxSpeed += gPathNodes[idx].maxSpeed;
    }
    gPathNodes[idx].maxSpeed *= gPathNodes[idx].speedTweak / 10.0f + 1.0f;
}

#define sqrtf __builtin_sqrtf

void path_apply_braking_limits(s32 end, s32 i)
{
    f32 v;
    f32 t;
    f32 q;
    f32 sm;
    f32 r;
    f32 a;
    s32 nx;
    s32 c;
    s32 p;

    for (; end != i; i = gPathNodes[i].prev) {
        if (i == 0) {
            return;
        }
        v = gPathNodes[i].maxSpeed;
        t = v * v * gPathNodes[i].curvature - gPathNodes[i].bank * 14.700001f;
        t = t * t;
        nx = gPathNodes[i].prev;
        a = gPathNodes[i].length;
        if (t > 365.39212f) {
            q = 0.0f;
        } else {
            q = sqrtf(365.39212f - t);
        }
        if (q > 10.29f) {
            q = 10.29f;
        }
        q = q + gPathNodes[i].slope * 14.700001f;
        sm = (q + q) * a + v * v;
        if (sm < 1.0f) {
            sm = 1.0f;
        }
        sm = sqrtf(sm);
        if (sm < gPathNodes[nx].maxSpeed) {
            gPathNodes[nx].maxSpeed = sm;
        }
        p = gPathNodes[i].join;
        if (p >= 0) {
            while (c = gPathNodes[p].prev, gPathNodes[c].branch == -1) {
                p = c;
            }
            path_apply_braking_limits(p, gPathNodes[i].join);
        }
    }
}

/* gPathNodes read as a struct member: an aggregate access, so the load is not reused across the stores to pts[]
   (the original reloads it there). */
#define PATH_NODES_RELOAD (((struct { PathNode *p; } *)&gPathNodes)->p)

void path_limit_jump_speeds(void)
{
    Vec3f pts[20];
    Vec3f pos;
    s32 cur, following, nx, j, idx, i;
    u8 flag;
    Vec3f *pa, *pb;
    PathNode *e;
    f32 w;
    f32 t, t2, d, s, x, h, v;

    cur = 0;
    while (cur != gPathNodes[0].prev) {
        j = gPathNodes[cur].prev;
        following = gPathNodes[cur].next;
        {
        f32 bq = (gPathNodes[cur].pos.y - gPathNodes[j].pos.y) / gPathNodes[j].length;
        if (7.3500004f < -(((gPathNodes[following].pos.y - gPathNodes[cur].pos.y) / gPathNodes[cur].length - bq) * 2.0f / (gPathNodes[j].length + gPathNodes[cur].length)) * (gPathNodes[cur].unk_68 * gPathNodes[cur].unk_68)) {
            for (i = 0; i < 10; i++) {
                t = (f32)i / 10.0f;
                pts[i] = gPathNodes[j].pos;
                e = PATH_NODES_RELOAD;
                pts[i].x += e[j].splineX[2] * t + e[j].splineX[1] * (t * t) + e[j].splineX[0] * (t * t * t);
                pts[i].z += e[j].splineZ[2] * t + e[j].splineZ[1] * (t * t) + e[j].splineZ[0] * (t * t * t);
                pts[i].y = bike_get_ground_height(&pts[i]);
            }
            for (i = 0; i < 10; i++) {
                t = (f32)i / 10.0f;
                pts[i + 10] = gPathNodes[cur].pos;
                e = PATH_NODES_RELOAD;
                pts[i + 10].x += e[cur].splineX[2] * t + e[cur].splineX[1] * (t * t) + e[cur].splineX[0] * (t * t * t);
                pts[i + 10].z += e[cur].splineZ[2] * t + e[cur].splineZ[1] * (t * t) + e[cur].splineZ[0] * (t * t * t);
                pts[i + 10].y = bike_get_ground_height(&pts[i + 10]);
            }
            for (i = 0; i < 18; i++) {
                if (pts[i].y < pts[i + 1].y && pts[i + 2].y < pts[i + 1].y) {
                    d = __builtin_sqrtf((pts[i + 1].x - pts[i].x) * (pts[i + 1].x - pts[i].x)
                                        + (pts[i + 1].z - pts[i].z) * (pts[i + 1].z - pts[i].z));
                    s = (pts[i + 1].y - pts[i].y) / d;
                    if (s > 1.0f) {
                        s = 1.0f;
                    }
                    flag = 0;
                    pb = &pts[i + 1];
                    pa = &pts[i];
                    while (!((x = gPathNodes[cur].maxSpeed * gPathNodes[cur].maxSpeed * 2.0f * s / 14.700001f) < 10.0f)) {
                        pos.x = pb->x + x * (pb->x - pa->x) / d;
                        pos.z = pb->z + x * (pb->z - pa->z) / d;
                        idx = path_find_segment(&pos, cur, 20, 2);
                        h = path_get_lateral_offset(&pos, idx) * 1000.0f;
                        if (-h < gPathNodes[idx].widthPos && h < gPathNodes[idx].widthNeg && gPathNodes[cur].maxSpeed <= gPathNodes[idx].maxSpeed) {
                            break;
                        }
                        w = gPathNodes[cur].maxSpeed * 0.95f;
                        gPathNodes[cur].maxSpeed = w;
                        flag = 1;
                    }
                    if (flag) {
                        j = cur;
                        do {
                            s = gPathNodes[gPathNodes[j].prev].maxSpeed;
                            path_apply_braking_limits(gPathNodes[j].prev, j);
                            nx = gPathNodes[j].prev;
                            j = nx;
                        } while (gPathNodes[nx].maxSpeed < s);
                    }
                }
            }
        }
        }
        cur = following;
    }
}

void path_calc_widths_and_progress(void)
{
    s32 i;
    Vec3f v;
    f32 st[4];
    f32 l[2];
    f32 acc;
    PathNode *r;

    for (i = 0; i < gPathNodeCount; i++) {
        height_gather_walls((s32)((gPathNodes[i].pos.x - 8.0f) * 1000.0f),
                      (s32)((gPathNodes[i].pos.z - 8.0f) * 1000.0f),
                      (s32)((gPathNodes[i].pos.x + 8.0f) * 1000.0f),
                      (s32)((gPathNodes[i].pos.z + 8.0f) * 1000.0f));
        v = gPathNodes[i].pos;
        v.x -= gPathNodes[i].lateralX * 8.0f;
        v.z -= gPathNodes[i].lateralZ * 8.0f;
        l[0] = 1.0f;
        height_raycast_walls(&gPathNodes[i], &v, st, l, 0);
        gPathNodes[i].widthNeg = l[0] * 8.0f;
        v = gPathNodes[i].pos;
        v.x += gPathNodes[i].lateralX * 8.0f;
        v.z += gPathNodes[i].lateralZ * 8.0f;
        l[0] = 1.0f;
        height_raycast_walls(&gPathNodes[i], &v, st, l, 0);
        gPathNodes[i].widthPos = l[0] * 8.0f;
    }

    acc = 0.0f;
    i = 1;
    if (gPathNodes[i].next != 1) {
        r = gPathNodes;
        do {
            r[i].lapProgress = ((gPathLapLength - acc) * 32768.0f) / gPathLapLength;
            acc += r[i].length;
            i = r[i].next;
        } while (r[i].next != 1);
    }
}

void path_init(u32 *param_1)
{
    s32 iVar1;
    u32 uVar2;

    if (gGame.mode == 6) {
        path_generate_straight();
        goto end;
    }
    if ((*(u32 *)(gCurTrack + 0x50) & 2) != 0) {
        sPathRawData = gUserTrackData.path;
        path_load(gUserTrackData.path);
        path_calc_widths_and_progress();
        path_limit_jump_speeds();
        gPathFinishNode = 1;
        gPathStartNode = 1;
        mem_free((s32)sPathRawData);
        goto end;
    }
    if (*(s32 *)(gCurTrack + 0x2c) != 2) {
        if (gGame.mode == 9 && (gTutorialLessons[gSeason->round].flags & 2)) {
            iVar1 = sTutorialPathResId;
            goto L134;
        }
        if (*(volatile s32 *)&gGame.mode == 7 && (gCheatFlags & 8)) {
            iVar1 = sDownhillPathResId;
            goto L134;
        }
        iVar1 = track_res_find(gGidPathSpline);
L134:
        res_set_load_buffer(gZBuffer);
        uVar2 = res_load(param_1, iVar1);
        sPathRawData = (u32 *)(uVar2 & 0xfffffffc);
        path_load(sPathRawData);
        res_unload((u32 *)(param_1[1] + iVar1 * 4));
        res_set_load_buffer(0);
    }
end:
    return;
}

s32 path_find_nearest_node(Vec3f *pos, s32 start, s32 count, u8 mode) {
    f32 best = 100000000.0f;
    s32 bestIdx;
    s32 seg;
    s32 i;
    f32 dx;
    f32 dz;
    f32 d;

    seg = start;
    for (i = 0; i < count; i++) {
        dx = pos->x - gPathNodes[seg].pos.x;
        dz = pos->z - gPathNodes[seg].pos.z;
        d = dx * dx + dz * dz;
        if (d < best) {
            best = d;
            bestIdx = seg;
        }
        if (gPathNodes[seg].branch >= 0 && mode == 2) {
            seg = gPathNodes[seg].branch;
        } else {
            seg = gPathNodes[seg].next;
        }
    }
    seg = start;
    for (i = 0; i < count; i++) {
        dx = pos->x - gPathNodes[seg].pos.x;
        dz = pos->z - gPathNodes[seg].pos.z;
        d = dx * dx + dz * dz;
        if (d < best) {
            best = d;
            bestIdx = seg;
        }
        seg = gPathNodes[seg].prev;
    }
    if (mode == 0) {
        seg = gPathNodes[bestIdx].alt;
        if (seg >= 0) {
            dx = pos->x - gPathNodes[seg].pos.x;
            dz = pos->z - gPathNodes[seg].pos.z;
            d = dx * dx + dz * dz;
            if (d < best) {
                bestIdx = seg;
            }
        }
    }
    return bestIdx;
}

f32 path_get_lateral_offset(Vec3f *pos, s32 idx)
{
    PathNode *b;
    PathNode *a;
    PathNode *base;
    f32 dx, dz;
    f32 dA, dB, t, t2, t3;
    f32 sx, sz, r;

    base = gPathNodes;
    b = (PathNode *)(idx * sizeof(PathNode) + (u32)base);
    a = (PathNode *)(b->next * sizeof(PathNode) + (u32)base);
    dz = a->pos.z - pos->z;
    dx = a->pos.x - pos->x;
    dA = dz * a->lateralX - dx * a->lateralZ;
    dz = b->pos.z - pos->z;
    dx = b->pos.x - pos->x;
    dB = dz * b->lateralX - dx * b->lateralZ;
    t = dB / (dB - dA);
    t2 = t * t;
    t3 = t2 * t;
    dx += t3 * b->splineX[0] + t2 * b->splineX[1] + t * b->splineX[2];
    dz += t3 * b->splineZ[0] + t2 * b->splineZ[1] + t * b->splineZ[2];
    sx = t2 * 3.0f * b->splineX[0] + (t + t) * b->splineX[1] + b->splineX[2];
    sz = t2 * 3.0f * b->splineZ[0] + (t + t) * b->splineZ[1] + b->splineZ[2];
    dB = __builtin_sqrtf(sx * sx + sz * sz);
    gPathSegmentT = t;
    gPathTangentX = sx / dB;
    gPathTangentZ = sz / dB;
    return (sx * dz - sz * dx) / dB;
}

void path_get_segment_and_t(Vec3f *pos, s32 *area, f32 *t, s32 p4)
{
    s32 i;
    PathNode *s;
    PathNode *tab;
    PathNode *a;
    PathNode *b;
    f32 fa;
    f32 fb;
    f32 r;
    f32 d, m1, m2, m3, m4;

    i = path_find_nearest_node(pos, p4, 4, 1);
    tab = gPathNodes;
    s = (PathNode *)(i * sizeof(PathNode) + (u32)tab);
    if ((pos->z - s->pos.z) * s->lateralX - (pos->x - s->pos.x) * s->lateralZ < 0.0f) {
        i = s->prev;
    }
    *area = i;
    a = (PathNode *)(i * sizeof(PathNode) + (u32)gPathNodes);
    b = (PathNode *)(a->next * sizeof(PathNode) + (u32)gPathNodes);
    d = b->pos.z - pos->z;
    m1 = d * b->lateralX;
    d = b->pos.x - pos->x;
    m2 = d * b->lateralZ;
    d = a->pos.z - pos->z;
    m3 = d * a->lateralX;
    d = a->pos.x - pos->x;
    m4 = d * a->lateralZ;
    fb = m1 - m2;
    fa = m3 - m4;
    r = fa / (fa - fb);
    *t = r;
    if (r < 0.0f) {
        *t = 0.0f;
    }
    if (1.0f < *t) {
        *t = 1.0f;
    }
}

typedef struct {
    s16 id;
    s16 arg;
    s32 (*fn)(Rider *, s32);
} Trig;

typedef struct {
    s16 a, b, c, d;
} Ev;

typedef struct {
    u8 pad0[0x2C];
    s32 f2C;
    u8 pad30[0x54];
    Trig *f84;
    Ev ev[3];
} Track_10;

extern Track_10 *gCurTrack_26;

void path_update_bike(Rider *b)
{
    s32 oldSeg;
    s32 n;
    s32 i;
    PathNode *s;
    Trig *t;
    Ev *e;
    s32 w;
    s32 nx;
    f32 v;
    f32 old;
    f32 sq;
    s32 k;
    s32 cur;

    oldSeg = b->segment;
    if ((b->rearWheel.surface->flags & 0x80) && b->rearWheel.airTicks <= 0) {
        bike_crash_begin(b, 0x19);
    }
    if ((b->frontWheel.surface->flags & 0x80) && b->frontWheel.airTicks <= 0) {
        bike_crash_begin(b, 0x1C);
    }
    if (gCurTrack_26->f2C == 1) {
        height_get_ground(b->drawPos.x, b->drawPos.y, b->drawPos.z);
        if (gGroundFound == 0) {
            bike_respawn(b);
        }
    }
    if (b->rearWheel.airTicks <= 0 && b->frontWheel.airTicks <= 0 && b->crashState == 0) {
        if (!(b->rearWheel.surface->flags & 0x100) && !(b->frontWheel.surface->flags & 0x100)) {
            b->prevSegment = oldSeg;
            b->crashTimer = 0;
        } else if (gGame.mode != 0xE && (b->rearWheel.surface->flags & 0x100) && (b->frontWheel.surface->flags & 0x100)) {
            if (++b->crashTimer > 10) {
                bike_respawn(b);
            }
        }
    }
    if (gEvent.state < 5 && gGame.mode != 9) {
        gPathSegmentT = 0.0f;
        b->segment = ((s16 *)&gPathStartNode)[1];
    } else {
        Vec3f *pos = &b->bike.pos;
        n = path_find_nearest_node(pos, oldSeg, 2, b->pathSearch);
        if ((pos->z - gPathNodes[n].pos.z) * gPathNodes[n].lateralX - (b->bike.pos.x - gPathNodes[n].pos.x) * gPathNodes[n].lateralZ < 0.0f) {
            n = gPathNodes[n].prev;
        }
        b->segment = n;
        if ((s16)n != oldSeg) {
            if (b->isCpu) {
                t = gCurTrack_26->f84;
                if (t != NULL) {
                    while (t->id >= 0) {
                        if (t->id != 0 && b->segment == t->id) {
                            b->pathSearch = t->fn(b, t->arg) + 1;
                        }
                        t++;
                    }
                } else {
                    b->pathSearch = 3;
                }
            }
            for (i = 0; i < 3; i++) {
                if (gCurTrack_26->ev[i].a == -1) {
                    break;
                }
                if (gCurTrack_26->ev[i].a == b->race.lap + b->lapAdjust
                    && (gCurTrack_26->ev[i].b == b->segment || gCurTrack_26->ev[i].c == b->segment)) {
                    remove_walls_trigger(gCurTrack_26->ev[i].d);
                }
            }
            if (gGame.mode == 7) {
                if (b->segment == gPathNodes->prev) {
                    b->segment = 0;
                }
                n = b->segment;
                if (gPathNodes[n].respawnDir == 0 && b->finishState == 0 && b->crashState == 0 && b->bestSegment < n) {
                    if (n < 0x78) {
                        b->engine.heat = 0.0f;
                        b->race.lapMsgTimer = 0x78;
                        b->bestSegment = b->segment;
                    } else {
                        b->finishState = 1;
                        b->race.place = 0;
                        b->race.finishTime = gEvent.time;
                        event_check_record_time(b);
                        b->isCpu = 1;
                    }
                }
            }
            path_check_lap(b, oldSeg);
        }
        b->lateral = path_get_lateral_offset(&b->bike.pos, b->segment);
        b->segmentT = gPathSegmentT;
        if (b->isCpu == 0) {
            b->wrongWay = b->bike.fwd.x * gPathTangentX + b->bike.fwd.z * gPathTangentZ < 0.0f && b->fwdSpeed > -0.5f && b->anim != 0xF;
        }
    }
    if (b->crashState == 0) {
        k = b->segment;
        if (b->segmentT > 0.5f) {
            k = gPathNodes[k].next;
        }
        if (gCurTrack_26->f2C == 0
            && (gPathNodes[k].widthPos < b->lateral - 3.5f || gPathNodes[k].widthNeg < -(b->lateral + 3.5f))) {
            if (++b->offTrackTimer > 180 && b->rearWheel.airTicks <= 0 && b->frontWheel.airTicks <= 0) {
                bike_respawn(b);
            }
        } else {
            b->prevSegment = oldSeg;
            b->offTrackTimer = 0;
        }
    }
    {
        w = gPathSegmentT * 256.0f;
        k = gPathNodes[b->segment].next;
        cur = gPathNodes[b->segment].lapProgress;
        nx = gPathNodes[k].lapProgress;
    }
    if (nx == 0x8000) {
        nx = 0;
    }
    v = (f32)(s32)(cur * (0x100 - w) + nx * w) * gPathLapLength * 1.1920929e-07f;
    sq = b->bike.vel.x * b->bike.vel.x + b->bike.vel.z * b->bike.vel.z;
    old = b->lapDistLeft;
    b->lapDistLeft = v;
    b->progressSpeed = (v - old) / gPhysDeltaTime;
    if (sq > 1.0f && b->airFrames < -15) {
        sq = __builtin_sqrtf(sq);
        if (b->bike.vel.x * gPathTangentX + b->bike.vel.z * gPathTangentZ / sq > 0.7f) {
            b->crestAhead = path_find_crest_ahead(sq, b->segment);
        }
    } else {
        b->crestAhead = -1;
    }
    if (b->race.finishPlace == -1) {
        b->raceDist = gPathLapLength * (f32)(b->lapAdjust + 1 + b->race.lap) - b->lapDistLeft;
    }
    if (b->isCpu == 0 && b->segment >= 0x13A && b->segment < 0x140 && b->airFrames > 30
        && gCurTrack_26 == &gTracks[TRACK_CANYON_CHASM] && b->bike.vel.y < 0.0f && b->bike.pos.y < gPathNodes[0x140].pos.y) {
        bike_crash_begin(b, 0x1C);
        cam_start_fall_view(b);
    }
}

#define SEG(i) ((PathNode *)((s32)((i) * 0x6C) + (s32)gPathNodes))

void path_check_lap(Rider *p, s32 a1)
{
    s32 h;
    s32 cur;
    s32 iv;
    s32 v;

    if (a1 == gPathFinishNode) {
        if (p->segment == SEG(a1)->prev) {
            p->lapAdjust--;
        }
    }
    h = p->segment;
    cur = gPathFinishNode;
    if (h == cur || h == SEG(cur)->next) {
        iv = SEG(cur)->prev;
        if (a1 == iv || a1 == SEG(iv)->prev) {
            if ((s8)p->lapAdjust == 0 && (gGame.flags & 1) == 0) {
                v = p - gRiders;
                if (v < gEvent.numRiders) {
                    event_rider_lap_completed(v);
                    return;
                }
            }
            p->lapAdjust++;
        }
    }
}

s32 path_find_segment(Vec3f *pos, s32 start, s32 count, u8 mode)
{
    s32 idx;
    PathNode *e;
    PathNode *base;

    idx = path_find_nearest_node(pos, start, count, mode);
    base = gPathNodes;
    e = (PathNode *)((u32)(idx * 0x6c) + (u32)base);
    if ((pos->z - e->pos.z) * e->lateralX - (pos->x - e->pos.x) * e->lateralZ < 0.0f) {
        idx = e->prev;
    }
    return idx;
}

void path_calc_corner_speeds(s32 param_1, s32 param_2)
{
    s32 i;
    for (i = 0; i <= param_2; i++) {
        path_calc_corner_speed(i);
    }
}

typedef struct {
    char pad[0x2C];
    s32 f2C;
} Obj_36;

extern Obj_36 *gCurTrack_27;

void path_free(void)
{
    if (gCurTrack_27->f2C != 2) {
        mem_free(gPathNodes);
    }
}
