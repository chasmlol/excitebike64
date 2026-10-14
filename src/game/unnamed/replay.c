#include "game.h"

void replay_record_start(u8 *param_1, s32 param_2)
{
    mem_fill(sReplayRecDeltas, 0, 0xc0);
    mem_fill(sReplayRecPrevStickX, 0, 6);
    mem_fill(sReplayRecPrevStickY, 0, 6);
    mem_fill(sReplayRecPrevButtons, 0, 6);
    sReplayRecBlockFrame = 0;
    gReplayBuf = (s32)param_1;
    gReplayWritePtr = (s32)param_1;
    gReplayBufSize = param_2;
    sReplayBytesLeft = param_2;
    sReplayWriteBitsLeft = 8;
    sReplayOverflow = 0;
    sReplayRecFrames = 0;
    mem_fill(param_1, 0, param_2);
    replay_write_bits(0x7b, 8);
    replay_write_bits(0, 8);
    replay_write_bits(0, 8);
    replay_write_bits(0, 8);
    replay_write_header();
    sReplayRecPlayers = gNumPlayers;
    sReplayFrameCountPtr = gReplayWritePtr;
    replay_write_bits(0, 8);
    replay_write_bits(0, 8);
    replay_write_bits(0, 8);
}

void replay_write_bits(s32 v, s32 n)
{
    s32 b;

    if (sReplayOverflow != 0) {
        return;
    }
    while (1) {
        v &= ((1 << n) - 1) << (8 - n);
        b = v & 0xFF;
        { s32 sh = 8 - sReplayWriteBitsLeft; *gReplayWritePtr_2 |= b >> sh; }
        if (n < sReplayWriteBitsLeft) {
            sReplayWriteBitsLeft -= n;
            return;
        }
        n -= sReplayWriteBitsLeft;
        v = b << sReplayWriteBitsLeft;
        sReplayWriteBitsLeft = 8;
        gReplayWritePtr_2++;
        sReplayBytesLeft--;
        if (sReplayBytesLeft == 0) {
            sReplayOverflow = 1;
        }
    }
}

typedef struct {
    s8 x;
    s8 y;
    u8 bits;
    u8 pad;
} Ent_60;

extern Ent_60 sReplayRecDeltas_2[][8];

void replay_record_frame(u32 *p)
{
    s32 i, j, k;
    s32 x, y, bits, c, v, a, n;
    u32 w;

    if (sReplayOverflow != 0) {
        return;
    }
    sReplayRecFrames++;
    for (i = 0; i < sReplayRecPlayers; i++) {
        w = p[i];
        x = (w >> 24) & 0xF;
        if (w & 4) x = -x;
        y = w >> 28;
        if (w & 2) y = -y;
        bits = (w >> 4) & 1;
        if (w & 0x20) bits |= 2;
        if (w & 0x20000) bits |= 4;
        if (w & 0x100) bits |= 8;
        c = bits;
        if (w & 0x10000) c |= 0x10;
        sReplayRecDeltas_2[i][sReplayRecBlockFrame].x = x - sReplayRecPrevStickX[i];
        sReplayRecDeltas_2[i][sReplayRecBlockFrame].y = y - sReplayRecPrevStickY[i];
        sReplayRecDeltas_2[i][sReplayRecBlockFrame].bits = sReplayRecPrevButtons[i] ^ c;
        sReplayRecPrevStickX[i] = x;
        sReplayRecPrevStickY[i] = y;
        sReplayRecPrevButtons[i] = c;
    }
    sReplayRecBlockFrame++;
    if (sReplayRecBlockFrame != 8) {
        return;
    }
    sReplayRecBlockFrame = 0;
    for (i = 0; i < sReplayRecPlayers; i++) {
        for (j = 0; j < 8; j++) {
            if (sReplayRecDeltas_2[i][j].bits != 0) break;
        }
        if (j == 8) {
            replay_write_bits(0, 1);
        } else {
            replay_write_bits(0x80, 1);
            for (j = 0; j < 8; j++) {
                for (k = 0; k < 5; k++) {
                    if ((sReplayRecDeltas_2[i][j].bits >> k) & 1) {
                        replay_write_bits(((k << 4) | ~0x7F) & 0xF0, 4);
                    }
                }
                replay_write_bits(0, 1);
            }
        }
        for (j = 0; j < 8; j++) {
            if (sReplayRecDeltas_2[i][j].x != 0 || sReplayRecDeltas_2[i][j].y != 0) break;
        }
        if (j == 8) {
            replay_write_bits(0, 1);
            continue;
        }
        replay_write_bits(0x80, 1);
        for (j = 0; j < 8; j++) {
            if (sReplayRecDeltas_2[i][j].x != 0) break;
        }
        if (j == 8) {
            replay_write_bits(0, 1);
        } else {
            replay_write_bits(0x80, 1);
            for (j = 0; j < 8; j++) {
                if (sReplayRecDeltas_2[i][j].x == 0) {
                    a = 0;
                    n = 1;
                } else {
                    replay_write_bits(0x80, 1);
                    v = sReplayRecDeltas_2[i][j].x;
                    if (v > 0) {
                        replay_write_bits(0x80, 1);
                        v &= 0x1F;
                    } else {
                        replay_write_bits(0, 1);
                        v = -v;
                        v &= 0x1F;
                    }
                    if (v == 1) { a = 0; n = 2; }
                    else if (v == 2) { a = 0x40; n = 3; }
                    else if (v == 3) { a = 0x60; n = 4; }
                    else { a = v << 2; a |= ~0x7F; a &= 0xFC; n = 6; }
                }
                replay_write_bits(a, n);
            }
        }
        for (j = 0; j < 8; j++) {
            if (sReplayRecDeltas_2[i][j].y != 0) break;
        }
        if (j == 8) {
            replay_write_bits(0, 1);
        } else {
            replay_write_bits(0x80, 1);
            for (j = 0; j < 8; j++) {
                if (sReplayRecDeltas_2[i][j].y == 0) {
                    a = 0;
                    n = 1;
                } else {
                    replay_write_bits(0x80, 1);
                    v = sReplayRecDeltas_2[i][j].y;
                    if (v > 0) {
                        replay_write_bits(0x80, 1);
                        v &= 0x1F;
                    } else {
                        replay_write_bits(0, 1);
                        v = -v;
                        v &= 0x1F;
                    }
                    if (v == 1) { a = 0; n = 2; }
                    else if (v == 2) { a = 0x40; n = 3; }
                    else if (v == 3) { a = 0x60; n = 4; }
                    else { a = v << 2; a |= ~0x7F; a &= 0xFC; n = 6; }
                }
                replay_write_bits(a, n);
            }
        }
    }
    sReplayPercentUsed = ((gReplayWritePtr - gReplayBuf) * 100) / gReplayBufSize;
}

s32 replay_record_finish(void)
{
    u32 buf[6];
    s32 i;
    s32 n;
    s32 acc;
    u8 *p;

    sReplayFrameCountPtr_2[0] = sReplayRecFrames;
    sReplayFrameCountPtr_2[1] = sReplayRecFrames >> 8;
    sReplayFrameCountPtr_2[2] = sReplayRecFrames >> 16;
    mem_fill((u8 *)buf, 0, 0x18);
    while (sReplayRecBlockFrame != 0) {
        replay_record_frame(buf);
    }
    if (sReplayWriteBitsLeft != 8) {
        replay_write_bits(0, sReplayWriteBitsLeft);
    }
    acc = 0;
    n = gReplayWritePtr_2 - gReplayBuf_2 - 4;
    p = gReplayBuf_2 + 4;
    for (i = 0; i < n; i++) {
        acc += p[i] << (i % 8);
    }
    acc &= 0xFFFFFF;
    gReplayBuf_2[1] = acc;
    gReplayBuf_2[2] = acc >> 8;
    gReplayBuf_2[3] = acc >> 16;
    if (sReplayOverflow != 0) {
        return -1;
    }
    return gReplayWritePtr_2 - gReplayBuf_2;
}

#define RB() rb()

typedef struct {
    s8 a;
    s8 b;
    u8 c;
    u8 d;
} Cell_5;

extern Cell_5 sReplayPlayDeltas[][8];

static inline s32 rb(void)
{
    u8 *p = sReplayReadPtr;
    s32 b = *p;
    s32 c = sReplayReadBitPos - 1;
    s32 bit;

    sReplayReadBitPos = c;
    bit = (b >> c) & 1;
    if (c == 0) {
        sReplayReadPtr = p + 1;
        sReplayReadBitPos = 8;
    }
    return bit;
}

void replay_decode_block(void)
{
    s32 i, j, k, n, s;
    u32 v, f;
    s32 x, y, z;

    for (i = 0; i < sReplayPlayPlayers; i++) {
        mem_fill((u8 *)sReplayPlayDeltas[i], 0, 0x20);
        if (RB()) {
            for (j = 0; j < 8; j++) {
                while (RB()) {
                    k = RB() << 2;
                    k |= RB() << 1;
                    k |= RB();
                    sReplayPlayDeltas[i][j].c |= 1 << k;
                }
            }
        }
        if (RB()) {
            if (RB()) {
                for (j = 0; j < 8; j++) {
                    if (!RB()) continue;
                    s = RB();
                    if (RB()) {
                        n = 0;
                        for (k = 0; k < 5; k++) { n <<= 1; n |= RB(); }
                    } else {
                        n = 1;
                        while (RB()) n++;
                    }
                    if (s == 0) n = -n;
                    sReplayPlayDeltas[i][j].a = n;
                }
            }
            if (RB()) {
                for (j = 0; j < 8; j++) {
                    if (!RB()) continue;
                    s = RB();
                    if (RB()) {
                        n = 0;
                        for (k = 0; k < 5; k++) { n <<= 1; n |= RB(); }
                    } else {
                        n = 1;
                        while (RB()) n++;
                    }
                    if (s == 0) n = -n;
                    sReplayPlayDeltas[i][j].b = n;
                }
            }
        }
    }

    for (i = 0; i < sReplayPlayPlayers; i++) {
        for (j = 0; j < 8; j++) {
            x = sReplayStickX_2[i] + sReplayPlayDeltas[i][j].a;
            y = sReplayStickY_2[i] + sReplayPlayDeltas[i][j].b;
            z = sReplayButtons_2[i] ^ sReplayPlayDeltas[i][j].c;
            sReplayStickX_2[i] = x;
            sReplayStickY_2[i] = y;
            sReplayButtons_2[i] = z;
            k = (z & 1) << 4;
            if (z & 2) k |= 0x20;
            if (z & 4) k |= 0x20000;
            if (z & 8) k |= 0x100;
            if (z & 0x10) k |= 0x10000;
            if (x < 0) {
                x = -x;
                k |= 4;
                k |= (x & 0xF) << 24;
            } else if (x != 0) {
                k |= 8;
                k |= (x & 0xF) << 24;
            }
            if (y < 0) {
                y = -y;
                k |= 2;
                k |= (y & 0xF) << 28;
            } else if (y != 0) {
                k |= 1;
                k |= (y & 0xF) << 28;
            }
            sReplayInputs[i][j] = k;
        }
    }
    sReplayBlockFramesLeft = 8;
}

void replay_write_header(void)
{
    s32 i;
    u32 lo, hi;

    replay_write_bits((u8)gGame.numPlayers, 8);
    replay_write_bits((u8)gGame.numCpuRiders, 8);
    replay_write_bits((u8)gGame.trackSel, 8);
    replay_write_bits((u8)gGame.numLaps, 8);
    for (i = 0; i < 6; i++) {
        lo = gGame.riderSkins[i] & 0xF;
        hi = (gGame.bikeSkins[i] & 0xF) << 4;
        replay_write_bits(hi | lo, 8);
    }
    for (i = 0; i < 6; i++) {
        if (gGhostSlot == i) {
            replay_write_bits(((s32)(u8)gGame.riders[i] | -0x80) & 0xFF, 8);
        } else {
            replay_write_bits((u8)gGame.riders[i], 8);
        }
    }
}

static inline s32 getbits(s32 n)
{
    s32 v;
    s32 j;
    s32 b;
    s32 c;

    v = 0;
    for (j = 0; j < n; j++) {
        v <<= 1;
        b = *sReplayReadPtr;
        c = sReplayReadBitPos - 1;
        sReplayReadBitPos = c;
        b = b >> c;
        b = b & 1;
        if (c == 0) {
            sReplayReadPtr++;
            sReplayReadBitPos = 8;
        }
        v |= b;
    }
    return v;
}

void replay_read_header(void)
{
    s32 i;
    s32 n;

    gNumPlayers = getbits(8);
    D_800EFDB4 = getbits(8);
    getbits(8);
    getbits(8);
    for (i = 0; i < 6; i++) {
        n = getbits(8);
        gGame.riderSkins[i] = n & 0xF;
        gGame.bikeSkins[i] = (n >> 4) & 0xF;
    }
    for (i = 0; i < 6; i++) {
        n = getbits(8);
        gGame.riders[i] = n & 0x7F;
        if (n & 0x80) {
            gGhostSlot = i;
        }
    }
}

void replay_skip_header(void)
{
    s32 i;
    s32 j;
    s32 c;
    s32 v;
    s32 w;

    i = 0;
    do {
        v = sReplayReadPtr;
        c = sReplayReadBitPos;
        c = c - 1;
        sReplayReadBitPos = c;
        if (c == 0) {
            w = v + 1;
            sReplayReadPtr = w;
            sReplayReadBitPos = 8;
        }
        i++;
    } while (i < 8);
    i = 0;
    do {
        v = sReplayReadPtr;
        c = sReplayReadBitPos;
        c = c - 1;
        sReplayReadBitPos = c;
        if (c == 0) {
            w = v + 1;
            sReplayReadPtr = w;
            sReplayReadBitPos = 8;
        }
        i++;
    } while (i < 8);
    i = 0;
    do {
        v = sReplayReadPtr;
        c = sReplayReadBitPos;
        c = c - 1;
        sReplayReadBitPos = c;
        if (c == 0) {
            w = v + 1;
            sReplayReadPtr = w;
            sReplayReadBitPos = 8;
        }
        i++;
    } while (i < 8);
    i = 0;
    do {
        v = sReplayReadPtr;
        c = sReplayReadBitPos;
        c = c - 1;
        sReplayReadBitPos = c;
        if (c == 0) {
            w = v + 1;
            sReplayReadPtr = w;
            sReplayReadBitPos = 8;
        }
        i++;
    } while (i < 8);
    j = 0;
    do {
        i = 0;
        do {
            v = sReplayReadPtr;
            c = sReplayReadBitPos;
            c = c - 1;
            sReplayReadBitPos = c;
            if (c == 0) {
                w = v + 1;
                sReplayReadPtr = w;
                sReplayReadBitPos = 8;
            }
            i++;
        } while (i < 8);
        j++;
    } while (j < 6);
    j = 0;
    do {
        i = 0;
        do {
            v = sReplayReadPtr;
            c = sReplayReadBitPos;
            c = c - 1;
            sReplayReadBitPos = c;
            if (c == 0) {
                w = v + 1;
                sReplayReadPtr = w;
                sReplayReadBitPos = 8;
            }
            i++;
        } while (i < 8);
        j++;
    } while (j < 6);
}

void replay_play_start(s32 param_1)
{
    s32 u;

    sReplayBlockFramesLeft = 0;
    sReplayReadPtr = param_1;
    sReplayReadBitPos = 8;
    sReplayReadPtr = param_1 + 4;
    replay_read_header();
    sReplayPlayPlayers = gNumPlayers;
    sReplayFramesLeft = replay_read_bits(8);
    u = replay_read_bits(8);
    sReplayFramesLeft = sReplayFramesLeft + u * 0x100;
    u = replay_read_bits(8);
    sReplayFramesLeft = sReplayFramesLeft + u * 0x10000;
    mem_fill(&sReplayStickX[0], 0, 6);
    mem_fill(&sReplayStickY[0], 0, 6);
    mem_fill(&sReplayButtons[0], 0, 6);
}

void replay_play_start_keep_setup(s32 param_1)
{
    s32 u;

    sReplayBlockFramesLeft = 0;
    sReplayReadPtr = param_1;
    sReplayReadBitPos = 8;
    sReplayReadPtr = param_1 + 4;
    replay_skip_header();
    sReplayPlayPlayers = gNumPlayers;
    sReplayFramesLeft = replay_read_bits(8);
    u = replay_read_bits(8);
    sReplayFramesLeft = sReplayFramesLeft + u * 0x100;
    u = replay_read_bits(8);
    sReplayFramesLeft = sReplayFramesLeft + u * 0x10000;
    mem_fill(&sReplayStickX[0], 0, 6);
    mem_fill(&sReplayStickY[0], 0, 6);
    mem_fill(&sReplayButtons[0], 0, 6);
}

s32 replay_play_frame(s32 *dst)
{
    s32 i;

    if (sReplayFramesLeft == 0) {
        return 1;
    }
    if (sReplayBlockFramesLeft == 0) {
        replay_decode_block();
    }
    for (i = 0; i < sReplayPlayPlayers; i++) {
        dst[i] = sReplayInputs[i][8 - sReplayBlockFramesLeft];
    }
    sReplayBlockFramesLeft--;
    sReplayFramesLeft--;
    return 0;
}

s32 replay_get_info(u8 *s, u32 *a1, u32 *a2, u32 *a3)
{
    u32 v;

    if (s[0] != 0x7B) {
        return 1;
    }
    *a1 = s[6];
    *a2 = s[7];
    v = s[0x14];
    *a3 = v;
    v = v + (s[0x15] << 8);
    *a3 = v;
    *a3 = v + (s[0x16] << 16);
    return 0;
}

s32 replay_validate(u8 *p, s32 n)
{
    s32 sum;
    s32 i;
    s32 lim;
    u32 hdr;
    u8 *q;

    if (p[0] != 0x7B) {
        return 1;
    }
    if (n != 0x2800) {
        return 1;
    }
    q = p + 4;
    lim = 0x27FC;
    sum = 0;
    i = sum;
    do {
        u8 b = q[i];
        sum += b << (i % 8);
        i++;
    } while (i < lim);
    sum &= 0xFFFFFF;
    hdr = p[1] | (p[2] << 8) | (p[3] << 16);
    return (hdr ^ sum) != 0;
}

u32 calc_checksum24(u8 *p, s32 n)
{
    s32 i;
    u32 acc = 0;

    for (i = 0; i < n; i++) {
        acc += (u32)p[i] << (i % 8);
    }
    acc &= 0xFFFFFF;
    return acc;
}

s32 replay_read_bit(void)
{
    u8 *p = sReplayReadPtr;
    s32 byte = *p;
    s32 n = sReplayReadBitPos - 1;
    s32 bit;

    sReplayReadBitPos = n;
    bit = byte >> n;
    bit = bit & 1;
    if (n == 0) {
        sReplayReadPtr = p + 1;
        sReplayReadBitPos = 8;
    }
    return bit;
}

s32 replay_read_bits(s32 n)
{
    s32 r = 0;
    s32 i;

    for (i = 0; i < n; i++) {
        u8 *p = sReplayReadPtr;
        s32 byte = *p;
        s32 c = sReplayReadBitPos - 1;
        s32 bit;

        r = r << 1;
        sReplayReadBitPos = c;
        bit = (byte >> c) & 1;
        if (c == 0) {
            sReplayReadPtr = p + 1;
            sReplayReadBitPos = 8;
        }
        r = r | bit;
    }
    return r;
}
