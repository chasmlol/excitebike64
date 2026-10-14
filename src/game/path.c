#include "game.h"

/* The distance along the racing line from node `from` to node `to`, taking the shorter way round each fork; gives
   up past 10000. */
f32 path_get_distance(s32 from, s32 to)
{
    f32 dist;
    f32 maxDist;
    f32 lenNext, lenBranch, dx, dz;
    s32 node;

    dist = 0.0f;
    maxDist = 10000.0f;
    for (;;) {
        if (from == to) {
            return dist;
        }
        if (maxDist <= dist) {
            return dist;
        }
        lenNext = gPathNodes[from].length;
        if (gPathNodes[from].branch >= 0) {
            dx = gPathNodes[from].pos.x - gPathNodes[gPathNodes[from].branch].pos.x;
            dz = gPathNodes[from].pos.z - gPathNodes[gPathNodes[from].branch].pos.z;
            lenBranch = __builtin_sqrtf(dx * dx + dz * dz);
            break;
        }
        from = gPathNodes[from].next;
        dist += lenNext;
    }
    /* a fork: when `to` is on one of the two ways before they join, take that one */
    node = gPathNodes[from].next;
    while (gPathNodes[node].join < 0) {
        if (node == to) {
            return dist + lenNext + path_get_distance(gPathNodes[from].next, to);
        }
        node = gPathNodes[node].next;
    }
    node = gPathNodes[from].branch;
    while (gPathNodes[node].join < 0) {
        if (node == to) {
            return dist + lenBranch + path_get_distance(gPathNodes[from].branch, to);
        }
        node = gPathNodes[node].next;
    }
    lenBranch += path_get_distance(gPathNodes[from].branch, to);
    lenNext += path_get_distance(gPathNodes[from].next, to);
    if (lenBranch < lenNext) {
        return lenBranch + dist;
    }
    return lenNext + dist;
}

/* Sets each node's lateral vector: across the sum of the directions from the previous node and to the next. */
void path_calc_normals(void)
{
    s32 i;
    PathNode *node;
    PathNode *next;
    PathNode *prev;
    f32 latX;
    f32 latZ;
    f32 len;

    for (i = 0; i < gPathNodeCount; i++) {
        node = (PathNode *)(i * sizeof(PathNode) + (u32)gPathNodes);
        next = (PathNode *)(node->next * sizeof(PathNode) + (u32)gPathNodes);
        prev = (PathNode *)(node->prev * sizeof(PathNode) + (u32)gPathNodes);
        latX = (next->pos.z - node->pos.z) / node->length;
        latX = latX * node->length + ((node->pos.z - prev->pos.z) / prev->length) * prev->length;
        latZ = -(next->pos.x - node->pos.x) / node->length;
        latZ = latZ * node->length + (-(node->pos.x - prev->pos.x) / prev->length) * prev->length;
        len = __builtin_sqrtf(latX * latX + latZ * latZ);
        if (0.0f < len) {
            gPathNodes[i].lateralX = latX / len;
            gPathNodes[i].lateralZ = latZ / len;
        }
    }
}

/* Fits the cubic of each segment from node idx up to stop (and down each branch): a Bezier curve whose inner control
   points lie a third of the segment along the tangents (the lateral vectors turned a quarter) at its ends. */
void path_calc_spline_coeffs(s32 idx, s32 stop)
{
    PathNode *node, *next, *nodes;
    f32 third, tanX, ctrl1X, ctrl1Z, tanZ, ctrl2X, ctrl2Z;
    s32 nextIdx;

    do {
    nodes = gPathNodes;
    node = (PathNode *)(idx * 0x6c + (s32)nodes);
    third = node->length / 3.0f;
    tanX = -node->lateralZ;
    tanZ = node->lateralX;
    nextIdx = node->next;
    next = (PathNode *)(nextIdx * 0x6c + (s32)nodes);
    ctrl1X = node->pos.x + tanX * third;
    ctrl1Z = node->pos.z + tanZ * third;
    ctrl2X = next->pos.x - -next->lateralZ * third;
    ctrl2Z = next->pos.z - next->lateralX * third;
    node->splineX[2] = tanX * 3.0f * third;
    node->splineX[1] = (ctrl2X - ctrl1X) * 3.0f - node->splineX[2];
    node->splineX[0] = next->pos.x - node->pos.x - node->splineX[1] - node->splineX[2];
    node->splineZ[2] = tanZ * 3.0f * third;
    node->splineZ[1] = (ctrl2Z - ctrl1Z) * 3.0f - node->splineZ[2];
    node->splineZ[0] = next->pos.z - node->pos.z - node->splineZ[1] - node->splineZ[2];
    if (node->branch >= 0) {
        path_calc_spline_coeffs(node->branch, stop);
    }
    idx = nextIdx;
    } while (idx != stop);
}

/* Sets the slope of each segment from node i up to end (and down each branch), and the bank: the ground height
   difference a quarter unit to each side of the node. */
void path_calc_slope_and_bank(s32 i, s32 end)
{
    Vec3f posSide;
    Vec3f negSide;
    f32 offX;
    f32 offZ;
    f32 posHeight;
    f32 negHeight;
    s32 next;

    do {
        next = gPathNodes[i].next;
        gPathNodes[i].slope = (gPathNodes[next].pos.y - gPathNodes[i].pos.y) / gPathNodes[i].length;
        posSide = gPathNodes[i].pos;
        negSide = posSide;
        offX = gPathNodes[i].lateralX * 0.25f;
        offZ = -gPathNodes[i].lateralZ * 0.25f;
        posSide.x = posSide.x + offX;
        posSide.z = posSide.z - offZ;
        negSide.x = negSide.x - offX;
        negSide.z = negSide.z + offZ;
        posHeight = bike_get_ground_height((f32 *)&posSide);
        negHeight = bike_get_ground_height((f32 *)&negSide);
        gPathNodes[i].bank = (posHeight - negHeight) * 2.0f;
        if (gPathNodes[i].branch >= 0) {
            path_calc_slope_and_bank(gPathNodes[i].branch, end);
        }
        i = next;
    } while (i != end);
}

/* Sets the curvature at each node from start up to stop: of the segment leaving it or of the one arriving, whichever
   is smaller. */
void path_calc_curvature(s32 start, s32 stop)
{
    s32 node = start;
    f32 v, u, t, dx, dz, speedSq, cross, crossIn, speed, denom, curvOut, curvIn, absIn;
    PathNode *prev;

    do {
        /* the segment leaving the node, at t = 0 */
        dx = gPathNodes[node].splineX[2];
        dz = gPathNodes[node].splineZ[2];
        speedSq = dx * dx + dz * dz;
        t = 2.0f * gPathNodes[node].splineZ[1];
        u = dx * t;
        t = 2.0f * gPathNodes[node].splineX[1];
        cross = u - dz * t;
        speed = __builtin_sqrtf(speedSq);
        denom = speedSq * speed;
        /* the segment arriving, at t = 1 */
        prev = (PathNode *)(gPathNodes[node].prev * sizeof(PathNode) + (u32)gPathNodes);
        dx = prev->splineX[0] * 3.0f + 2.0f * prev->splineX[1] + prev->splineX[2];
        dz = prev->splineZ[0] * 3.0f + 2.0f * prev->splineZ[1] + prev->splineZ[2];
        speedSq = dx * dx + dz * dz;
        curvOut = cross / denom;
        t = 2.0f * prev->splineZ[1] + prev->splineZ[0] * 6.0f;
        v = dx * t;
        t = 2.0f * prev->splineX[1] + prev->splineX[0] * 6.0f;
        crossIn = v - dz * t;
        speed = __builtin_sqrtf(speedSq);
        curvIn = crossIn / (speedSq * speed);
        absIn = curvIn;
        if (!(curvIn > 0.0f)) {
            absIn = -curvIn;
        }
        if (curvOut > 0.0f ? curvOut < absIn : -curvOut < absIn) {
            gPathNodes[node].curvature = curvIn;
        } else {
            gPathNodes[node].curvature = curvOut;
        }
        node = gPathNodes[node].next;
    } while (node != stop);
}

/* How many of the next 5 nodes come before a crest that would launch a bike at this speed (0xFF: none). */
u8 path_find_crest_ahead(f32 speed, s32 idx)
{
    s32 i;
    s32 next, prev;
    f32 slopeOut, slopeIn, lift;
    speed = speed * speed;
    for (i = 0; i < 5; i++) {
        next = gPathNodes[idx].next;
        prev = gPathNodes[idx].prev;
        slopeOut = (gPathNodes[idx].pos.y - gPathNodes[next].pos.y) / gPathNodes[next].length;
        /* gPathNodes[i]: the loop counter where a node index was meant, as in the original */
        slopeIn = (gPathNodes[prev].pos.y - gPathNodes[idx].pos.y) / gPathNodes[i].length;
        lift = -(((slopeIn - slopeOut) + (slopeIn - slopeOut)) / (gPathNodes[next].length + gPathNodes[i].length)) * speed;
        if (7.3500004f /* GRAVITY / 2 */ < lift) return i & 0xFF;
        idx = next;
    }
    return 0xFF;
}

/* Sets every node's curvature, then its lookahead: the nodes until the summed |curvature| (plus 0.006 a node)
   reaches 0.06, or a fork. */
void path_calc_curvatures(void)
{
    s32 i, j, count;
    f32 budget;
    PathNode *nodes;

    for (i = 0; i < gPathNodeCount; i++) {
        path_calc_curvature(i, gPathNodes[i].next);
    }
    for (i = 0; i < gPathNodeCount; i++) {
        count = 0;
        j = i;
        budget = 0.06f;
        nodes = gPathNodes;
        while (budget > 0.0f) {
            if (nodes[j].branch != -1) {
                count++;
                break;
            }
            budget -= ((nodes[j].curvature > 0.0f) ? nodes[j].curvature : -nodes[j].curvature) + 0.006f;
            j = nodes[j].next;
            count++;
        }
        gPathNodes[i].lookahead = count - 1;
    }
}

/* Excite 3D: a straight racing line down the track's z axis from z = 30, following the ground until it ends; each
   node's speedTweak keeps the ground type under each of 4 lanes (2 bits each). */
void path_generate_straight(void)
{
    s32 i, lane, ground;
    f32 z, step;
    Vec3f pos, normal, prevNormal;

    gPathNodes = mem_alloc_dbg(540000 /* 5000 nodes */, 0, sStrPathFile, 0x178);
    mem_fill((u8 *)gPathNodes, 0, 540000);
    z = 30.0f;
    prevNormal.x = 0.0f;
    prevNormal.y = 0.0f;
    prevNormal.z = 0.0f;
    for (i = 0; i < 1000; i++) {
        gPathNodes[i].prev = i - 1;
        gPathNodes[i].next = i + 1;
        gPathNodes[i].alt = -1;
        gPathNodes[i].join = -1;
        gPathNodes[i].branch = -1;
        /* a step of 2, or 1 where the ground's slope turns over (a crest or a dip) */
        step = 3.0f;
        do {
            step -= 1.0f;
            pos.x = 0.0f;
            pos.y = 40.0f;
            pos.z = z - step;
            pos.y = bike_get_ground_height((float *)&pos);
            if (gGroundFound == 0) {
                break;
            }
            height_get_ground_normal((float *)&normal);
        } while (normal.z * prevNormal.z < -0.01f && 1.1f < step);
        prevNormal = normal;
        gPathNodes[i].pos = pos;
        gPathNodes[i].length = step;
        gPathNodes[i].speedTweak = 0;
        for (lane = 3; lane >= 0; lane--) {
            pos.x = lane * 2.178f;
            bike_get_ground_height((float *)&pos);
            switch ((gGroundSurface >> 4) & 0xF) {
            case 2:
            case 3:
                ground = 1;
                break;
            case 8:
                ground = 2;
                break;
            case 7:
                ground = 3;
                break;
            default:
                ground = 0;
                break;
            }
            gPathNodes[i].speedTweak |= ground << (lane * 2);
        }
        z -= step;
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

/* Builds gPathNodes from a path resource (PathResNode records) and precomputes the curves, the lap length and the
   corner and braking speeds. */
void path_load(u32 *res)
{
    u32 header;
    u32 flags;
    s32 stride;
    s32 i;
    PathResNode *rec;
    u8 *extra;
    f32 dx, dz;

    header = *res;
    stride = 0x20;
    if ((header >> 24) == PATH_RES_MAGIC) {
        flags = header;
        flags &= 0xFFFFFF;
        res++;
        if (header & PATH_RES_WIDTHS) stride = 0x28;
        if (header & PATH_RES_NODE_FLAGS) stride += 1;
        if (header & PATH_RES_PROGRESS) stride += 2;
        if (header & PATH_RES_EXTRA) stride += 2;
        stride = (stride + 3) & ~3;
    }
    /* without the header word, flags is read uninitialised (as in the original) */
    gPathNodeCount = *res;
    if (flags & PATH_RES_START_FINISH) {
        gPathStartNode = res[1];
        gPathFinishNode = res[2];
        res += 2;
    } else {
        gPathFinishNode = 0;
        gPathStartNode = 0;
    }
    i = 0;
    gPathNodes = mem_alloc_dbg(gPathNodeCount * sizeof(PathNode), 0, (s32)sStrPathFile, 0x20C);
    mem_fill((u8 *)gPathNodes, 0, gPathNodeCount * sizeof(PathNode));
    rec = (PathResNode *)(res + 1);
    for (; i < gPathNodeCount; i++) {
        mem_fill((u8 *)&gPathNodes[i], 0, sizeof(PathNode));
        gPathNodes[i].alt = rec->alt;
        gPathNodes[i].prev = rec->prev;
        gPathNodes[i].join = rec->join;
        gPathNodes[i].next = rec->next;
        gPathNodes[i].branch = rec->branch;
        gPathNodes[i].pos.x = (f32)rec->x / 1000.0f;
        gPathNodes[i].pos.z = (f32)rec->z / 1000.0f;
        /* the ground is searched from a little above the previous node */
        if (gPathNodes[i].prev < i) {
            gPathNodes[i].pos.y = gPathNodes[gPathNodes[i].prev].pos.y + 3.0f;
            if (gCurTrack == &gTracks[TRACK_LEFTYS_MILL]) {
                gPathNodes[i].pos.y = gPathNodes[i].pos.y + 4.0f;
            }
        } else {
            gPathNodes[i].pos.y = 0.0f;
        }
        gPathNodes[i].pos.y = bike_get_ground_height((f32 *)&gPathNodes[i]);
        gPathNodes[i].maxSpeed = rec->maxSpeed;
        gPathNodes[i].respawnDir = rec->respawnDir;
        gPathNodes[i].speedTweak = rec->speedTweak;
        gPathNodes[i].camCeiling = rec->camCeiling;
        gPathNodes[i].camWidthNeg = rec->camWidthNeg;
        gPathNodes[i].camWidthPos = rec->camWidthPos;
        gPathNodes[i].crestSpeed = rec->crestSpeed / 1000.0f;
        if (gPathNodes[i].maxSpeed < gPathNodes[i].crestSpeed) {
            gPathNodes[i].crestSpeed = 0.0f;
        }
        extra = (u8 *)rec + 0x20;
        if (flags & PATH_RES_WIDTHS) {
            gPathNodes[i].widthNeg = (f32)rec->widthNeg / 62.5f - 1.0f;
            if (gPathNodes[i].widthNeg < 0.0f) gPathNodes[i].widthNeg = 0.0f;
            gPathNodes[i].widthPos = (f32)rec->widthPos / 62.5f - 1.0f;
            if (gPathNodes[i].widthPos < 0.0f) gPathNodes[i].widthPos = 0.0f;
            extra = (u8 *)rec + 0x28;
            gPathNodes[i].fogRange = rec->fogRange;
        }
        if (flags & PATH_RES_PROGRESS_FIRST) {
            if (flags & PATH_RES_PROGRESS) {
                gPathNodes[i].lapProgress = *extra++ << 8;
                gPathNodes[i].lapProgress |= *extra++;
            }
            if (flags & PATH_RES_EXTRA) extra += 2;
            if (flags & PATH_RES_NODE_FLAGS) gPathNodes[i].flags = *extra;
        } else {
            if (flags & PATH_RES_NODE_FLAGS) gPathNodes[i].flags = *extra++;
            if (flags & PATH_RES_PROGRESS) {
                gPathNodes[i].lapProgress = *extra++ << 8;
                gPathNodes[i].lapProgress |= *extra++;
            }
        }
        rec = (PathResNode *)((u8 *)rec + stride);
    }
    for (i = 0; i < gPathNodeCount; i++) {
        flags = gPathNodes[i].next; /* reused: the next node */
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
        /* empty in the original */
    }
}

/* A node's maxSpeed from its curvature: the speed the grip (1.3 g, more on a bank) holds in the turn; 50 on a
   straight. Then the node's PATH_NODE_FAST_CORNER and speedTweak. */
void path_calc_corner_speed(s32 idx)
{
    f32 curv;
    f32 speedSq;
    f32 grip;

    curv = gPathNodes[idx].curvature;
    if ((curv > 0.0f) ? (curv > 0.005f) : (-curv > 0.005f)) {
        speedSq = gPathNodes[idx].curvature; /* the curvature, then v^2 */
        if (speedSq > 0.0f) {
            grip = gPathNodes[idx].bank * GRAVITY + 19.11f /* 1.3 g */;
            speedSq = grip / speedSq;
        } else {
            grip = 19.11f - gPathNodes[idx].bank * GRAVITY;
            grip = -grip;
            speedSq = grip / speedSq;
        }
        if (speedSq < 1.0f) {
            speedSq = 1.0f;
        }
        gPathNodes[idx].maxSpeed = __builtin_sqrtf(speedSq);
    } else {
        gPathNodes[idx].maxSpeed = 50.0f;
    }
    if (gPathNodes[idx].flags & PATH_NODE_FAST_CORNER) {
        gPathNodes[idx].maxSpeed += gPathNodes[idx].maxSpeed;
    }
    gPathNodes[idx].maxSpeed *= gPathNodes[idx].speedTweak / 10.0f + 1.0f;
}

#define sqrtf __builtin_sqrtf

/* Walks back from node i to end lowering each previous node's maxSpeed to what braking can bring down to this
   node's: the grip left over from the turn (at most 0.7 g) plus the slope. Branches are walked from their joins. */
void path_apply_braking_limits(s32 end, s32 i)
{
    f32 speed;
    f32 lateral;
    f32 brake;
    f32 prevSpeed;
    f32 len;
    s32 prev;
    s32 before;
    s32 join;

    for (; end != i; i = gPathNodes[i].prev) {
        if (i == 0) {
            return;
        }
        speed = gPathNodes[i].maxSpeed;
        lateral = speed * speed * gPathNodes[i].curvature - gPathNodes[i].bank * GRAVITY;
        lateral = lateral * lateral;
        prev = gPathNodes[i].prev;
        len = gPathNodes[i].length;
        if (lateral > 365.39212f) {
            brake = 0.0f;
        } else {
            brake = sqrtf(365.39212f - lateral);
        }
        if (brake > 10.29f /* 0.7 g */) {
            brake = 10.29f;
        }
        brake = brake + gPathNodes[i].slope * GRAVITY;
        prevSpeed = (brake + brake) * len + speed * speed;
        if (prevSpeed < 1.0f) {
            prevSpeed = 1.0f;
        }
        prevSpeed = sqrtf(prevSpeed);
        if (prevSpeed < gPathNodes[prev].maxSpeed) {
            gPathNodes[prev].maxSpeed = prevSpeed;
        }
        join = gPathNodes[i].join;
        if (join >= 0) {
            while (before = gPathNodes[join].prev, gPathNodes[before].branch == -1) {
                join = before;
            }
            path_apply_braking_limits(join, gPathNodes[i].join);
        }
    }
}

/* gPathNodes read as a struct member: an aggregate access, so the load is not reused across the stores to pts[]
   (the original reloads it there). */
#define PATH_NODES_RELOAD (((struct { PathNode *p; } *)&gPathNodes)->p)

/* At each crest that launches a bike at its crestSpeed, samples the ground of the two segments around it and lowers
   the node's maxSpeed (5% at a time) until the landing point of each rise is on the track and no faster than the
   landing node; then reapplies the braking limits behind it. */
void path_limit_jump_speeds(void)
{
    Vec3f pts[20];
    Vec3f landing;
    s32 node, next, prevIdx, prev, landNode, i;
    u8 lowered;
    Vec3f *base, *top;
    PathNode *nodes;
    f32 newSpeed;
    f32 t, dist, slope, flight, lateral;

    node = 0;
    while (node != gPathNodes[0].prev) {
        prev = gPathNodes[node].prev;
        next = gPathNodes[node].next;
        {
        f32 slopeIn = (gPathNodes[node].pos.y - gPathNodes[prev].pos.y) / gPathNodes[prev].length;
        if (7.3500004f /* GRAVITY / 2 */ < -(((gPathNodes[next].pos.y - gPathNodes[node].pos.y) / gPathNodes[node].length - slopeIn) * 2.0f / (gPathNodes[prev].length + gPathNodes[node].length)) * (gPathNodes[node].crestSpeed * gPathNodes[node].crestSpeed)) {
            for (i = 0; i < 10; i++) {
                t = (f32)i / 10.0f;
                pts[i] = gPathNodes[prev].pos;
                nodes = PATH_NODES_RELOAD;
                pts[i].x += nodes[prev].splineX[2] * t + nodes[prev].splineX[1] * (t * t) + nodes[prev].splineX[0] * (t * t * t);
                pts[i].z += nodes[prev].splineZ[2] * t + nodes[prev].splineZ[1] * (t * t) + nodes[prev].splineZ[0] * (t * t * t);
                pts[i].y = bike_get_ground_height(&pts[i]);
            }
            for (i = 0; i < 10; i++) {
                t = (f32)i / 10.0f;
                pts[i + 10] = gPathNodes[node].pos;
                nodes = PATH_NODES_RELOAD;
                pts[i + 10].x += nodes[node].splineX[2] * t + nodes[node].splineX[1] * (t * t) + nodes[node].splineX[0] * (t * t * t);
                pts[i + 10].z += nodes[node].splineZ[2] * t + nodes[node].splineZ[1] * (t * t) + nodes[node].splineZ[0] * (t * t * t);
                pts[i + 10].y = bike_get_ground_height(&pts[i + 10]);
            }
            for (i = 0; i < 18; i++) {
                /* a local top of the ground: the jump's lip */
                if (pts[i].y < pts[i + 1].y && pts[i + 2].y < pts[i + 1].y) {
                    dist = __builtin_sqrtf((pts[i + 1].x - pts[i].x) * (pts[i + 1].x - pts[i].x)
                                        + (pts[i + 1].z - pts[i].z) * (pts[i + 1].z - pts[i].z));
                    slope = (pts[i + 1].y - pts[i].y) / dist;
                    if (slope > 1.0f) {
                        slope = 1.0f;
                    }
                    lowered = 0;
                    top = &pts[i + 1];
                    base = &pts[i];
                    while (!((flight = gPathNodes[node].maxSpeed * gPathNodes[node].maxSpeed * 2.0f * slope / GRAVITY) < 10.0f)) {
                        landing.x = top->x + flight * (top->x - base->x) / dist;
                        landing.z = top->z + flight * (top->z - base->z) / dist;
                        landNode = path_find_segment(&landing, node, 20, 2);
                        lateral = path_get_lateral_offset(&landing, landNode) * 1000.0f;
                        if (-lateral < gPathNodes[landNode].widthPos && lateral < gPathNodes[landNode].widthNeg && gPathNodes[node].maxSpeed <= gPathNodes[landNode].maxSpeed) {
                            break;
                        }
                        newSpeed = gPathNodes[node].maxSpeed * 0.95f;
                        gPathNodes[node].maxSpeed = newSpeed;
                        lowered = 1;
                    }
                    if (lowered) {
                        prev = node;
                        do {
                            slope = gPathNodes[gPathNodes[prev].prev].maxSpeed; /* reused: the speed before */
                            path_apply_braking_limits(gPathNodes[prev].prev, prev);
                            prevIdx = gPathNodes[prev].prev;
                            prev = prevIdx;
                        } while (gPathNodes[prevIdx].maxSpeed < slope);
                    }
                }
            }
        }
        }
        node = next;
    }
}

/* The user track: each node's free width to each side (a raycast against the walls, up to 8), then its lapProgress
   from node 1 round the loop. */
void path_calc_widths_and_progress(void)
{
    s32 i;
    Vec3f probe;
    f32 hit[4];
    f32 hitFrac[2];
    f32 dist;
    PathNode *nodes;

    for (i = 0; i < gPathNodeCount; i++) {
        height_gather_walls((s32)((gPathNodes[i].pos.x - 8.0f) * 1000.0f),
                      (s32)((gPathNodes[i].pos.z - 8.0f) * 1000.0f),
                      (s32)((gPathNodes[i].pos.x + 8.0f) * 1000.0f),
                      (s32)((gPathNodes[i].pos.z + 8.0f) * 1000.0f));
        probe = gPathNodes[i].pos;
        probe.x -= gPathNodes[i].lateralX * 8.0f;
        probe.z -= gPathNodes[i].lateralZ * 8.0f;
        hitFrac[0] = 1.0f;
        height_raycast_walls(&gPathNodes[i], &probe, hit, hitFrac, 0);
        gPathNodes[i].widthNeg = hitFrac[0] * 8.0f;
        probe = gPathNodes[i].pos;
        probe.x += gPathNodes[i].lateralX * 8.0f;
        probe.z += gPathNodes[i].lateralZ * 8.0f;
        hitFrac[0] = 1.0f;
        height_raycast_walls(&gPathNodes[i], &probe, hit, hitFrac, 0);
        gPathNodes[i].widthPos = hitFrac[0] * 8.0f;
    }

    dist = 0.0f;
    i = 1;
    if (gPathNodes[i].next != 1) {
        nodes = gPathNodes;
        do {
            nodes[i].lapProgress = ((gPathLapLength - dist) * 32768.0f) / gPathLapLength;
            dist += nodes[i].length;
            i = nodes[i].next;
        } while (nodes[i].next != 1);
    }
}

/* Loads the racing line for the race: generated for Excite 3D, the user track's, or the track's path resource
   (the tutorial's and the downhill cheat's have their own); the desert has none. */
void path_init(u32 *resSet)
{
    s32 resId;
    u32 handle;

    if (gGame.mode == 6 /* Excite 3D */) {
        path_generate_straight();
        goto end;
    }
    if ((*(u32 *)(gCurTrack + 0x50) & 2) != 0 /* the user track */) {
        sPathRawData = gUserTrackData.path;
        path_load(gUserTrackData.path);
        path_calc_widths_and_progress();
        path_limit_jump_speeds();
        gPathFinishNode = 1;
        gPathStartNode = 1;
        mem_free((s32)sPathRawData);
        goto end;
    }
    if (*(s32 *)(gCurTrack + 0x2c) != 2 /* not the desert */) {
        if (gGame.mode == 9 /* tutorial */ && (gTutorialLessons[gSeason->round].flags & 2)) {
            resId = sTutorialPathResId;
            goto load;
        }
        if (*(volatile s32 *)&gGame.mode == 7 /* hill climb */ && (gCheatFlags & 8)) {
            resId = sDownhillPathResId;
            goto load;
        }
        resId = track_res_find(gGidPathSpline);
load:
        /* loaded into the z-buffer, freed once the nodes are built */
        res_set_load_buffer(gZBuffer);
        handle = res_load(resSet, resId);
        sPathRawData = (u32 *)(handle & 0xfffffffc);
        path_load(sPathRawData);
        res_unload((u32 *)(resSet[1] + resId * 4));
        res_set_load_buffer(0);
    }
end:
    return;
}

/* The node nearest pos among count nodes on from start (mode 2: taking the branches) and count nodes back; mode 0
   also tries the nearest node's alternative. */
s32 path_find_nearest_node(Vec3f *pos, s32 start, s32 count, u8 mode) {
    f32 bestDistSq = 100000000.0f;
    s32 best;
    s32 node;
    s32 i;
    f32 dx;
    f32 dz;
    f32 distSq;

    node = start;
    for (i = 0; i < count; i++) {
        dx = pos->x - gPathNodes[node].pos.x;
        dz = pos->z - gPathNodes[node].pos.z;
        distSq = dx * dx + dz * dz;
        if (distSq < bestDistSq) {
            bestDistSq = distSq;
            best = node;
        }
        if (gPathNodes[node].branch >= 0 && mode == 2) {
            node = gPathNodes[node].branch;
        } else {
            node = gPathNodes[node].next;
        }
    }
    node = start;
    for (i = 0; i < count; i++) {
        dx = pos->x - gPathNodes[node].pos.x;
        dz = pos->z - gPathNodes[node].pos.z;
        distSq = dx * dx + dz * dz;
        if (distSq < bestDistSq) {
            bestDistSq = distSq;
            best = node;
        }
        node = gPathNodes[node].prev;
    }
    if (mode == 0) {
        node = gPathNodes[best].alt;
        if (node >= 0) {
            dx = pos->x - gPathNodes[node].pos.x;
            dz = pos->z - gPathNodes[node].pos.z;
            distSq = dx * dx + dz * dz;
            if (distSq < bestDistSq) {
                best = node;
            }
        }
    }
    return best;
}

/* The signed distance of pos from the curve of segment idx; sets gPathSegmentT (where along the segment pos is) and
   gPathTangentX/Z (the curve's direction there). */
f32 path_get_lateral_offset(Vec3f *pos, s32 idx)
{
    PathNode *node;
    PathNode *next;
    PathNode *nodes;
    f32 dx, dz;
    f32 distNext, distNode, t, t2, t3;
    f32 tanX, tanZ;

    nodes = gPathNodes;
    node = (PathNode *)(idx * sizeof(PathNode) + (u32)nodes);
    next = (PathNode *)(node->next * sizeof(PathNode) + (u32)nodes);
    /* t from the distances along the segment to the two nodes' lateral lines */
    dz = next->pos.z - pos->z;
    dx = next->pos.x - pos->x;
    distNext = dz * next->lateralX - dx * next->lateralZ;
    dz = node->pos.z - pos->z;
    dx = node->pos.x - pos->x;
    distNode = dz * node->lateralX - dx * node->lateralZ;
    t = distNode / (distNode - distNext);
    t2 = t * t;
    t3 = t2 * t;
    dx += t3 * node->splineX[0] + t2 * node->splineX[1] + t * node->splineX[2];
    dz += t3 * node->splineZ[0] + t2 * node->splineZ[1] + t * node->splineZ[2];
    tanX = t2 * 3.0f * node->splineX[0] + (t + t) * node->splineX[1] + node->splineX[2];
    tanZ = t2 * 3.0f * node->splineZ[0] + (t + t) * node->splineZ[1] + node->splineZ[2];
    distNode = __builtin_sqrtf(tanX * tanX + tanZ * tanZ); /* reused: the tangent's length */
    gPathSegmentT = t;
    gPathTangentX = tanX / distNode;
    gPathTangentZ = tanZ / distNode;
    return (tanX * dz - tanZ * dx) / distNode;
}

/* The segment pos is on, searching from node start, and where along it (0..1). */
void path_get_segment_and_t(Vec3f *pos, s32 *outNode, f32 *outT, s32 start)
{
    s32 idx;
    PathNode *nearest;
    PathNode *nodes;
    PathNode *node;
    PathNode *next;
    f32 distNode;
    f32 distNext;
    f32 t;
    f32 d, m1, m2, m3, m4;

    idx = path_find_nearest_node(pos, start, 4, 1);
    nodes = gPathNodes;
    nearest = (PathNode *)(idx * sizeof(PathNode) + (u32)nodes);
    /* behind the nearest node's lateral line: on the segment before it */
    if ((pos->z - nearest->pos.z) * nearest->lateralX - (pos->x - nearest->pos.x) * nearest->lateralZ < 0.0f) {
        idx = nearest->prev;
    }
    *outNode = idx;
    node = (PathNode *)(idx * sizeof(PathNode) + (u32)gPathNodes);
    next = (PathNode *)(node->next * sizeof(PathNode) + (u32)gPathNodes);
    d = next->pos.z - pos->z;
    m1 = d * next->lateralX;
    d = next->pos.x - pos->x;
    m2 = d * next->lateralZ;
    d = node->pos.z - pos->z;
    m3 = d * node->lateralX;
    d = node->pos.x - pos->x;
    m4 = d * node->lateralZ;
    distNext = m1 - m2;
    distNode = m3 - m4;
    t = distNode / (distNode - distNext);
    *outT = t;
    if (t < 0.0f) {
        *outT = 0.0f;
    }
    if (1.0f < *outT) {
        *outT = 1.0f;
    }
}

/* A CPU rider's trigger on the track (+0x84): reaching the node calls fn, which returns the next pathSearch - 1. */
typedef struct PathTrigger {
    s16 node;
    s16 arg;
    s32 (*fn)(Rider *, s32);
} PathTrigger;

/* A wall that comes down (remove_walls_trigger) when a rider on the lap reaches either node. */
typedef struct WallEvent {
    s16 lap;
    s16 node1, node2;
    s16 wall;
} WallEvent;

/* The parts of the track record (gTracks) the path code reads. */
typedef struct TrackPathInfo {
    u8 pad0[0x2C];
    s32 env;                        /* 0 stadium, 1 outdoor, 2 desert */
    u8 pad30[0x54];
    PathTrigger *cpuTriggers;       /* NULL: CPU riders always search mode 3 */
    WallEvent wallEvents[3];        /* lap -1 ends the list */
} TrackPathInfo;

extern TrackPathInfo *gCurTrack_26;

/* The rider's place on the racing line each frame: crash and respawn checks on the ground, the segment and lateral
   offset, CPU triggers, falling walls, the hill climb's progress, laps, wrong way, the off-track timer, the distance
   along the lap and to go, and the crest ahead. */
void path_update_bike(Rider *rider)
{
    s32 oldSeg;
    s32 node;
    s32 i;
    PathTrigger *trig;
    s32 frac;
    s32 nextProgress;
    f32 dist;
    f32 oldDist;
    f32 speedSq;
    s32 seg;
    s32 progress;

    oldSeg = rider->segment;
    if ((rider->rearWheel.surface->flags & 0x80) && rider->rearWheel.airTicks <= 0) {
        bike_crash_begin(rider, 0x19);
    }
    if ((rider->frontWheel.surface->flags & 0x80) && rider->frontWheel.airTicks <= 0) {
        bike_crash_begin(rider, 0x1C);
    }
    if (gCurTrack_26->env == 1 /* outdoor */) {
        height_get_ground(rider->drawPos.x, rider->drawPos.y, rider->drawPos.z);
        if (gGroundFound == 0) {
            bike_respawn(rider);
        }
    }
    if (rider->rearWheel.airTicks <= 0 && rider->frontWheel.airTicks <= 0 && rider->crashState == 0) {
        if (!(rider->rearWheel.surface->flags & 0x100) && !(rider->frontWheel.surface->flags & 0x100)) {
            rider->prevSegment = oldSeg;
            rider->crashTimer = 0;
        } else if (gGame.mode != 0xE && (rider->rearWheel.surface->flags & 0x100) && (rider->frontWheel.surface->flags & 0x100)) {
            /* both wheels on such ground for 10 frames (not in the editor's test ride) */
            if (++rider->crashTimer > 10) {
                bike_respawn(rider);
            }
        }
    }
    if (gEvent.state < 5 && gGame.mode != 9 /* tutorial */) {
        gPathSegmentT = 0.0f;
        rider->segment = ((s16 *)&gPathStartNode)[1]; /* the low half */
    } else {
        Vec3f *pos = &rider->bike.pos;
        node = path_find_nearest_node(pos, oldSeg, 2, rider->pathSearch);
        if ((pos->z - gPathNodes[node].pos.z) * gPathNodes[node].lateralX - (rider->bike.pos.x - gPathNodes[node].pos.x) * gPathNodes[node].lateralZ < 0.0f) {
            node = gPathNodes[node].prev;
        }
        rider->segment = node;
        if ((s16)node != oldSeg) {
            if (rider->isCpu) {
                trig = gCurTrack_26->cpuTriggers;
                if (trig != NULL) {
                    while (trig->node >= 0) {
                        if (trig->node != 0 && rider->segment == trig->node) {
                            rider->pathSearch = trig->fn(rider, trig->arg) + 1;
                        }
                        trig++;
                    }
                } else {
                    rider->pathSearch = 3;
                }
            }
            for (i = 0; i < 3; i++) {
                if (gCurTrack_26->wallEvents[i].lap == -1) {
                    break;
                }
                if (gCurTrack_26->wallEvents[i].lap == rider->race.lap + rider->lapAdjust
                    && (gCurTrack_26->wallEvents[i].node1 == rider->segment || gCurTrack_26->wallEvents[i].node2 == rider->segment)) {
                    remove_walls_trigger(gCurTrack_26->wallEvents[i].wall);
                }
            }
            if (gGame.mode == 7 /* hill climb: a node past the best so far cools the engine; node 120 finishes */) {
                if (rider->segment == gPathNodes->prev) {
                    rider->segment = 0;
                }
                node = rider->segment;
                if (gPathNodes[node].respawnDir == 0 && rider->finishState == 0 && rider->crashState == 0 && rider->bestSegment < node) {
                    if (node < 0x78) {
                        rider->engine.heat = 0.0f;
                        rider->race.lapMsgTimer = 0x78;
                        rider->bestSegment = rider->segment;
                    } else {
                        rider->finishState = 1;
                        rider->race.place = 0;
                        rider->race.finishTime = gEvent.time;
                        event_check_record_time(rider);
                        rider->isCpu = 1;
                    }
                }
            }
            path_check_lap(rider, oldSeg);
        }
        rider->lateral = path_get_lateral_offset(&rider->bike.pos, rider->segment);
        rider->segmentT = gPathSegmentT;
        if (rider->isCpu == 0) {
            rider->wrongWay = rider->bike.fwd.x * gPathTangentX + rider->bike.fwd.z * gPathTangentZ < 0.0f && rider->fwdSpeed > -0.5f && rider->anim != 0xF;
        }
    }
    if (rider->crashState == 0) {
        seg = rider->segment;
        if (rider->segmentT > 0.5f) {
            seg = gPathNodes[seg].next;
        }
        /* stadiums: 180 frames off the track (beyond its widths) respawn the rider */
        if (gCurTrack_26->env == 0
            && (gPathNodes[seg].widthPos < rider->lateral - 3.5f || gPathNodes[seg].widthNeg < -(rider->lateral + 3.5f))) {
            if (++rider->offTrackTimer > 180 && rider->rearWheel.airTicks <= 0 && rider->frontWheel.airTicks <= 0) {
                bike_respawn(rider);
            }
        } else {
            rider->prevSegment = oldSeg;
            rider->offTrackTimer = 0;
        }
    }
    {
        frac = gPathSegmentT * 256.0f;
        seg = gPathNodes[rider->segment].next;
        progress = gPathNodes[rider->segment].lapProgress;
        nextProgress = gPathNodes[seg].lapProgress;
    }
    if (nextProgress == 0x8000) {
        nextProgress = 0;
    }
    /* lapProgress interpolated along the segment, as a distance: 2^-23 = 1 / (0x8000 * 0x100) */
    dist = (f32)(s32)(progress * (0x100 - frac) + nextProgress * frac) * gPathLapLength * 1.1920929e-07f;
    speedSq = rider->bike.vel.x * rider->bike.vel.x + rider->bike.vel.z * rider->bike.vel.z;
    oldDist = rider->segmentDist;
    rider->segmentDist = dist;
    rider->progressSpeed = (dist - oldDist) / gPhysDeltaTime;
    if (speedSq > 1.0f && rider->airFrames < -15) {
        speedSq = __builtin_sqrtf(speedSq); /* the speed from here on */
        if (rider->bike.vel.x * gPathTangentX + rider->bike.vel.z * gPathTangentZ / speedSq > 0.7f) {
            rider->crestAhead = path_find_crest_ahead(speedSq, rider->segment);
        }
    } else {
        rider->crestAhead = -1;
    }
    if (rider->race.finishPlace == -1) {
        rider->distToGo = gPathLapLength * (f32)(rider->lapAdjust + 1 + rider->race.lap) - rider->segmentDist;
    }
    /* Canyon Chasm: falling into the chasm (nodes 0x13A-0x13F, below node 0x140) */
    if (rider->isCpu == 0 && rider->segment >= 0x13A && rider->segment < 0x140 && rider->airFrames > 30
        && gCurTrack_26 == &gTracks[TRACK_CANYON_CHASM] && rider->bike.vel.y < 0.0f && rider->bike.pos.y < gPathNodes[0x140].pos.y) {
        bike_crash_begin(rider, 0x1C);
        cam_start_fall_view(rider);
    }
}

#define SEG(i) ((PathNode *)((s32)((i) * 0x6C) + (s32)gPathNodes))

/* Laps: crossing the finish node backwards takes lapAdjust down; crossing it forwards brings lapAdjust back up, or
   with lapAdjust 0 completes a lap (event_rider_lap_completed). */
void path_check_lap(Rider *rider, s32 oldSeg)
{
    s32 seg;
    s32 finish;
    s32 beforeFinish;
    s32 riderIdx;

    if (oldSeg == gPathFinishNode) {
        if (rider->segment == SEG(oldSeg)->prev) {
            rider->lapAdjust--;
        }
    }
    seg = rider->segment;
    finish = gPathFinishNode;
    if (seg == finish || seg == SEG(finish)->next) {
        beforeFinish = SEG(finish)->prev;
        if (oldSeg == beforeFinish || oldSeg == SEG(beforeFinish)->prev) {
            if ((s8)rider->lapAdjust == 0 && (gGame.flags & 1) == 0) {
                riderIdx = rider - gRiders;
                if (riderIdx < gEvent.numRiders) {
                    event_rider_lap_completed(riderIdx);
                    return;
                }
            }
            rider->lapAdjust++;
        }
    }
}

/* The segment pos is on: the nearest node, or the one before it when pos is behind the node's lateral line. */
s32 path_find_segment(Vec3f *pos, s32 start, s32 count, u8 mode)
{
    s32 idx;
    PathNode *nearest;
    PathNode *nodes;

    idx = path_find_nearest_node(pos, start, count, mode);
    nodes = gPathNodes;
    nearest = (PathNode *)((u32)(idx * 0x6c) + (u32)nodes);
    if ((pos->z - nearest->pos.z) * nearest->lateralX - (pos->x - nearest->pos.x) * nearest->lateralZ < 0.0f) {
        idx = nearest->prev;
    }
    return idx;
}

/* Every node's corner speed from node 0 to last (first is not used). */
void path_calc_corner_speeds(s32 first, s32 last)
{
    s32 i;
    for (i = 0; i <= last; i++) {
        path_calc_corner_speed(i);
    }
}

extern TrackPathInfo *gCurTrack_27;

void path_free(void)
{
    if (gCurTrack_27->env != 2 /* the desert has no path */) {
        mem_free(gPathNodes);
    }
}
