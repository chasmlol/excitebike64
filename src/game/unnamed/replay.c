#include "game.h"

/* The input recorder and player behind replays and Controller Pak ghosts (format: REPLAY_* in system.h). The
   recorder keeps, per player, the stick (direction and magnitude, -15..15 on each axis) and five buttons
   (REPLAY_BTN_*) of the game's input word, and writes their changes from frame to frame, a block of 8 frames at a
   time. Bits are written from the top of each byte: replay_write_bits takes them from the top of its value
   (0x80 with 1 bit writes a 1). */

/* Starts recording into buf (size bytes, cleared): writes the magic, room for the checksum, the setup of the race
   in gGame (replay_write_header) and room for the frame count, which replay_record_finish fills in. */
void replay_record_start(u8 *buf, s32 size)
{
    mem_fill(sReplayRecDeltas, 0, 0xc0);    /* 6 racers x 8 frames */
    mem_fill(sReplayRecPrevStickX, 0, REPLAY_MAX_RACERS);
    mem_fill(sReplayRecPrevStickY, 0, REPLAY_MAX_RACERS);
    mem_fill(sReplayRecPrevButtons, 0, REPLAY_MAX_RACERS);
    sReplayRecBlockFrame = 0;
    gReplayBuf = (s32)buf;
    gReplayWritePtr = (s32)buf;
    gReplayBufSize = size;
    sReplayBytesLeft = size;
    sReplayWriteBitsLeft = 8;
    sReplayOverflow = 0;
    sReplayRecFrames = 0;
    mem_fill(buf, 0, size);
    replay_write_bits(REPLAY_MAGIC, 8);
    replay_write_bits(0, 8);
    replay_write_bits(0, 8);
    replay_write_bits(0, 8);
    replay_write_header();
    sReplayRecPlayers = gGame.numPlayers;
    sReplayFrameCountPtr = gReplayWritePtr;
    replay_write_bits(0, 8);
    replay_write_bits(0, 8);
    replay_write_bits(0, 8);
}

/* Appends the top numBits bits of the byte value to the replay. Once the buffer is full nothing more is written and
   sReplayOverflow is set. */
void replay_write_bits(s32 value, s32 numBits)
{
    s32 byte;

    if (sReplayOverflow != 0) {
        return;
    }
    while (1) {
        value &= ((1 << numBits) - 1) << (8 - numBits);
        byte = value & 0xFF;
        { s32 shift = 8 - sReplayWriteBitsLeft; *gReplayWritePtr_2 |= byte >> shift; }
        if (numBits < sReplayWriteBitsLeft) {
            sReplayWriteBitsLeft -= numBits;
            return;
        }
        numBits -= sReplayWriteBitsLeft;
        value = byte << sReplayWriteBitsLeft;
        sReplayWriteBitsLeft = 8;
        gReplayWritePtr_2++;
        sReplayBytesLeft--;
        if (sReplayBytesLeft == 0) {
            sReplayOverflow = 1;
        }
    }
}

/* How a player's input changed in one frame of a block. */
typedef struct {
    s8 stickX;
    s8 stickY;
    u8 buttons;     /* the REPLAY_BTN_* that changed */
    u8 pad;
} ReplayDelta;

extern ReplayDelta sReplayRecDeltas_2[][REPLAY_BLOCK_FRAMES];

/* Records one frame of each player's input word. Every 8 frames it writes a block, per player:
   - 1 bit: any button changed; if so, for each frame each changed button as a 1 and its 3-bit number, then a 0;
   - 1 bit: the stick moved; if so, 1 bit for any change in x and, if set, a code per frame (then the same for y):
     0 no change, else 1, the sign (1 = positive) and the size: 1 "00", 2 "010", 3 "0110", else 1 and 5 bits.
   sReplayPercentUsed is how full the buffer is. */
void replay_record_frame(u32 *inputs)
{
    s32 i, j, k;
    s32 x, y, bits, buttons, size, code, codeBits;
    u32 input;

    if (sReplayOverflow != 0) {
        return;
    }
    sReplayRecFrames++;
    for (i = 0; i < sReplayRecPlayers; i++) {
        input = inputs[i];
        x = (input >> INPUT_STICK_X_SHIFT) & 0xF;
        if (input & INPUT_STICK_LEFT) x = -x;
        y = input >> INPUT_STICK_Y_SHIFT;
        if (input & INPUT_STICK_DOWN) y = -y;
        bits = (input >> 4) & 1;    /* INPUT_A: REPLAY_BTN_A */
        if (input & INPUT_B) bits |= REPLAY_BTN_B;
        if (input & INPUT_Z) bits |= REPLAY_BTN_Z;
        if (input & INPUT_C_DOWN) bits |= REPLAY_BTN_C_DOWN;
        buttons = bits;
        if (input & INPUT_R) buttons |= REPLAY_BTN_R;
        sReplayRecDeltas_2[i][sReplayRecBlockFrame].stickX = x - sReplayRecPrevStickX[i];
        sReplayRecDeltas_2[i][sReplayRecBlockFrame].stickY = y - sReplayRecPrevStickY[i];
        sReplayRecDeltas_2[i][sReplayRecBlockFrame].buttons = sReplayRecPrevButtons[i] ^ buttons;
        sReplayRecPrevStickX[i] = x;
        sReplayRecPrevStickY[i] = y;
        sReplayRecPrevButtons[i] = buttons;
    }
    sReplayRecBlockFrame++;
    if (sReplayRecBlockFrame != REPLAY_BLOCK_FRAMES) {
        return;
    }
    sReplayRecBlockFrame = 0;
    for (i = 0; i < sReplayRecPlayers; i++) {
        /* Buttons */
        for (j = 0; j < REPLAY_BLOCK_FRAMES; j++) {
            if (sReplayRecDeltas_2[i][j].buttons != 0) break;
        }
        if (j == REPLAY_BLOCK_FRAMES) {
            replay_write_bits(0, 1);
        } else {
            replay_write_bits(0x80, 1);
            for (j = 0; j < REPLAY_BLOCK_FRAMES; j++) {
                for (k = 0; k < REPLAY_NUM_BUTTONS; k++) {
                    if ((sReplayRecDeltas_2[i][j].buttons >> k) & 1) {
                        replay_write_bits(((k << 4) | ~0x7F) & 0xF0, 4);   /* 1, then k in 3 bits */
                    }
                }
                replay_write_bits(0, 1);
            }
        }
        /* Stick */
        for (j = 0; j < REPLAY_BLOCK_FRAMES; j++) {
            if (sReplayRecDeltas_2[i][j].stickX != 0 || sReplayRecDeltas_2[i][j].stickY != 0) break;
        }
        if (j == REPLAY_BLOCK_FRAMES) {
            replay_write_bits(0, 1);
            continue;
        }
        replay_write_bits(0x80, 1);
        for (j = 0; j < REPLAY_BLOCK_FRAMES; j++) {
            if (sReplayRecDeltas_2[i][j].stickX != 0) break;
        }
        if (j == REPLAY_BLOCK_FRAMES) {
            replay_write_bits(0, 1);
        } else {
            replay_write_bits(0x80, 1);
            for (j = 0; j < REPLAY_BLOCK_FRAMES; j++) {
                if (sReplayRecDeltas_2[i][j].stickX == 0) {
                    code = 0;
                    codeBits = 1;
                } else {
                    replay_write_bits(0x80, 1);
                    size = sReplayRecDeltas_2[i][j].stickX;
                    if (size > 0) {
                        replay_write_bits(0x80, 1);
                        size &= 0x1F;
                    } else {
                        replay_write_bits(0, 1);
                        size = -size;
                        size &= 0x1F;
                    }
                    if (size == 1) { code = 0; codeBits = 2; }
                    else if (size == 2) { code = 0x40; codeBits = 3; }
                    else if (size == 3) { code = 0x60; codeBits = 4; }
                    else { code = size << 2; code |= ~0x7F; code &= 0xFC; codeBits = 6; }
                }
                replay_write_bits(code, codeBits);
            }
        }
        for (j = 0; j < REPLAY_BLOCK_FRAMES; j++) {
            if (sReplayRecDeltas_2[i][j].stickY != 0) break;
        }
        if (j == REPLAY_BLOCK_FRAMES) {
            replay_write_bits(0, 1);
        } else {
            replay_write_bits(0x80, 1);
            for (j = 0; j < REPLAY_BLOCK_FRAMES; j++) {
                if (sReplayRecDeltas_2[i][j].stickY == 0) {
                    code = 0;
                    codeBits = 1;
                } else {
                    replay_write_bits(0x80, 1);
                    size = sReplayRecDeltas_2[i][j].stickY;
                    if (size > 0) {
                        replay_write_bits(0x80, 1);
                        size &= 0x1F;
                    } else {
                        replay_write_bits(0, 1);
                        size = -size;
                        size &= 0x1F;
                    }
                    if (size == 1) { code = 0; codeBits = 2; }
                    else if (size == 2) { code = 0x40; codeBits = 3; }
                    else if (size == 3) { code = 0x60; codeBits = 4; }
                    else { code = size << 2; code |= ~0x7F; code &= 0xFC; codeBits = 6; }
                }
                replay_write_bits(code, codeBits);
            }
        }
    }
    sReplayPercentUsed = ((gReplayWritePtr - gReplayBuf) * 100) / gReplayBufSize;
}

/* Ends the recording: stores the frame count, pads the last block with frames of no input, flushes the last byte
   and stores the checksum. Returns the replay's size in bytes, or -1 if it did not fit. */
s32 replay_record_finish(void)
{
    u32 noInput[REPLAY_MAX_RACERS];
    s32 i;
    s32 len;
    s32 sum;
    u8 *data;

    sReplayFrameCountPtr_2[0] = sReplayRecFrames;
    sReplayFrameCountPtr_2[1] = sReplayRecFrames >> 8;
    sReplayFrameCountPtr_2[2] = sReplayRecFrames >> 16;
    mem_fill((u8 *)noInput, 0, 0x18);
    while (sReplayRecBlockFrame != 0) {
        replay_record_frame(noInput);
    }
    if (sReplayWriteBitsLeft != 8) {
        replay_write_bits(0, sReplayWriteBitsLeft);
    }
    /* The checksum: calc_checksum24 written out again. */
    sum = 0;
    len = gReplayWritePtr_2 - gReplayBuf_2 - REPLAY_OFS_SETUP;
    data = gReplayBuf_2 + REPLAY_OFS_SETUP;
    for (i = 0; i < len; i++) {
        sum += data[i] << (i % 8);
    }
    sum &= 0xFFFFFF;
    gReplayBuf_2[1] = sum;
    gReplayBuf_2[2] = sum >> 8;
    gReplayBuf_2[3] = sum >> 16;
    if (sReplayOverflow != 0) {
        return -1;
    }
    return gReplayWritePtr_2 - gReplayBuf_2;
}

extern ReplayDelta sReplayPlayDeltas[][REPLAY_BLOCK_FRAMES];

/* The next bit of the replay being played. header_read_bits, replay_skip_header, replay_read_bit and
   replay_read_bits repeat this code, each written the way that compiles to its own original. */
static inline s32 block_read_bit(void)
{
    u8 *ptr = sReplayReadPtr;
    s32 byte = *ptr;
    s32 bitPos = sReplayReadBitPos - 1;
    s32 bit;

    sReplayReadBitPos = bitPos;
    bit = (byte >> bitPos) & 1;
    if (bitPos == 0) {
        sReplayReadPtr = ptr + 1;
        sReplayReadBitPos = 8;
    }
    return bit;
}

/* Decodes the next block (the format is at replay_record_frame) into sReplayInputs: 8 frames of each player's
   input word. */
void replay_decode_block(void)
{
    s32 i, j, k, size, sign;
    u32 unused1, unused2;
    s32 x, y, buttons;

    for (i = 0; i < sReplayPlayPlayers; i++) {
        mem_fill((u8 *)sReplayPlayDeltas[i], 0, 0x20);   /* the player's 8 deltas */
        if (block_read_bit()) {
            for (j = 0; j < REPLAY_BLOCK_FRAMES; j++) {
                while (block_read_bit()) {
                    k = block_read_bit() << 2;
                    k |= block_read_bit() << 1;
                    k |= block_read_bit();
                    sReplayPlayDeltas[i][j].buttons |= 1 << k;
                }
            }
        }
        if (block_read_bit()) {
            if (block_read_bit()) {
                for (j = 0; j < REPLAY_BLOCK_FRAMES; j++) {
                    if (!block_read_bit()) continue;
                    sign = block_read_bit();
                    if (block_read_bit()) {
                        size = 0;
                        for (k = 0; k < 5; k++) { size <<= 1; size |= block_read_bit(); }
                    } else {
                        size = 1;
                        while (block_read_bit()) size++;
                    }
                    if (sign == 0) size = -size;
                    sReplayPlayDeltas[i][j].stickX = size;
                }
            }
            if (block_read_bit()) {
                for (j = 0; j < REPLAY_BLOCK_FRAMES; j++) {
                    if (!block_read_bit()) continue;
                    sign = block_read_bit();
                    if (block_read_bit()) {
                        size = 0;
                        for (k = 0; k < 5; k++) { size <<= 1; size |= block_read_bit(); }
                    } else {
                        size = 1;
                        while (block_read_bit()) size++;
                    }
                    if (sign == 0) size = -size;
                    sReplayPlayDeltas[i][j].stickY = size;
                }
            }
        }
    }

    for (i = 0; i < sReplayPlayPlayers; i++) {
        for (j = 0; j < REPLAY_BLOCK_FRAMES; j++) {
            x = sReplayStickX_2[i] + sReplayPlayDeltas[i][j].stickX;
            y = sReplayStickY_2[i] + sReplayPlayDeltas[i][j].stickY;
            buttons = sReplayButtons_2[i] ^ sReplayPlayDeltas[i][j].buttons;
            sReplayStickX_2[i] = x;
            sReplayStickY_2[i] = y;
            sReplayButtons_2[i] = buttons;
            /* k builds the input word */
            k = (buttons & REPLAY_BTN_A) << 4;  /* INPUT_A */
            if (buttons & REPLAY_BTN_B) k |= INPUT_B;
            if (buttons & REPLAY_BTN_Z) k |= INPUT_Z;
            if (buttons & REPLAY_BTN_C_DOWN) k |= INPUT_C_DOWN;
            if (buttons & REPLAY_BTN_R) k |= INPUT_R;
            if (x < 0) {
                x = -x;
                k |= INPUT_STICK_LEFT;
                k |= (x & 0xF) << INPUT_STICK_X_SHIFT;
            } else if (x != 0) {
                k |= INPUT_STICK_RIGHT;
                k |= (x & 0xF) << INPUT_STICK_X_SHIFT;
            }
            if (y < 0) {
                y = -y;
                k |= INPUT_STICK_DOWN;
                k |= (y & 0xF) << INPUT_STICK_Y_SHIFT;
            } else if (y != 0) {
                k |= INPUT_STICK_UP;
                k |= (y & 0xF) << INPUT_STICK_Y_SHIFT;
            }
            sReplayInputs[i][j] = k;
        }
    }
    sReplayBlockFramesLeft = REPLAY_BLOCK_FRAMES;
}

/* Writes the 16-byte setup from gGame: players, CPU riders, track, laps, each racer's bike and rider skins (a
   nibble each) and each racer's rider, the ghost's marked with REPLAY_RIDER_GHOST. */
void replay_write_header(void)
{
    s32 i;
    u32 riderSkin, bikeSkin;

    replay_write_bits((u8)gGame.numPlayers, 8);
    replay_write_bits((u8)gGame.numCpuRiders, 8);
    replay_write_bits((u8)gGame.trackSel, 8);
    replay_write_bits((u8)gGame.numLaps, 8);
    for (i = 0; i < REPLAY_MAX_RACERS; i++) {
        riderSkin = gGame.riderSkins[i] & 0xF;
        bikeSkin = (gGame.bikeSkins[i] & 0xF) << 4;
        replay_write_bits(bikeSkin | riderSkin, 8);
    }
    for (i = 0; i < REPLAY_MAX_RACERS; i++) {
        if (gGhostSlot == i) {
            replay_write_bits(((s32)(u8)gGame.riders[i] | -0x80) & 0xFF, 8);   /* | REPLAY_RIDER_GHOST */
        } else {
            replay_write_bits((u8)gGame.riders[i], 8);
        }
    }
}

/* The next numBits bits of the replay being played (see block_read_bit). */
static inline s32 header_read_bits(s32 numBits)
{
    s32 value;
    s32 j;
    s32 bit;
    s32 bitPos;

    value = 0;
    for (j = 0; j < numBits; j++) {
        value <<= 1;
        bit = *sReplayReadPtr;
        bitPos = sReplayReadBitPos - 1;
        sReplayReadBitPos = bitPos;
        bit = bit >> bitPos;
        bit = bit & 1;
        if (bitPos == 0) {
            sReplayReadPtr++;
            sReplayReadBitPos = 8;
        }
        value |= bit;
    }
    return value;
}

/* Reads the setup into gGame: players, CPU riders, skins and riders, and sets gGhostSlot to the racer marked as the
   ghost. The track and the laps are skipped. */
void replay_read_header(void)
{
    s32 i;
    s32 byte;

    gGame.numPlayers = header_read_bits(8);
    gGame.numCpuRiders = header_read_bits(8);
    header_read_bits(8);
    header_read_bits(8);
    for (i = 0; i < REPLAY_MAX_RACERS; i++) {
        byte = header_read_bits(8);
        gGame.riderSkins[i] = byte & 0xF;
        gGame.bikeSkins[i] = (byte >> 4) & 0xF;
    }
    for (i = 0; i < REPLAY_MAX_RACERS; i++) {
        byte = header_read_bits(8);
        gGame.riders[i] = byte & 0x7F;
        if (byte & REPLAY_RIDER_GHOST) {
            gGhostSlot = i;
        }
    }
}

/* Skips the 16-byte setup one bit at a time (the four single bytes, the six skin bytes, the six rider bytes),
   leaving gGame as it is. */
void replay_skip_header(void)
{
    s32 i;
    s32 j;
    s32 bitPos;
    s32 ptr;
    s32 nextPtr;

    i = 0;
    do {
        ptr = sReplayReadPtr;
        bitPos = sReplayReadBitPos;
        bitPos = bitPos - 1;
        sReplayReadBitPos = bitPos;
        if (bitPos == 0) {
            nextPtr = ptr + 1;
            sReplayReadPtr = nextPtr;
            sReplayReadBitPos = 8;
        }
        i++;
    } while (i < 8);
    i = 0;
    do {
        ptr = sReplayReadPtr;
        bitPos = sReplayReadBitPos;
        bitPos = bitPos - 1;
        sReplayReadBitPos = bitPos;
        if (bitPos == 0) {
            nextPtr = ptr + 1;
            sReplayReadPtr = nextPtr;
            sReplayReadBitPos = 8;
        }
        i++;
    } while (i < 8);
    i = 0;
    do {
        ptr = sReplayReadPtr;
        bitPos = sReplayReadBitPos;
        bitPos = bitPos - 1;
        sReplayReadBitPos = bitPos;
        if (bitPos == 0) {
            nextPtr = ptr + 1;
            sReplayReadPtr = nextPtr;
            sReplayReadBitPos = 8;
        }
        i++;
    } while (i < 8);
    i = 0;
    do {
        ptr = sReplayReadPtr;
        bitPos = sReplayReadBitPos;
        bitPos = bitPos - 1;
        sReplayReadBitPos = bitPos;
        if (bitPos == 0) {
            nextPtr = ptr + 1;
            sReplayReadPtr = nextPtr;
            sReplayReadBitPos = 8;
        }
        i++;
    } while (i < 8);
    j = 0;
    do {
        i = 0;
        do {
            ptr = sReplayReadPtr;
            bitPos = sReplayReadBitPos;
            bitPos = bitPos - 1;
            sReplayReadBitPos = bitPos;
            if (bitPos == 0) {
                nextPtr = ptr + 1;
                sReplayReadPtr = nextPtr;
                sReplayReadBitPos = 8;
            }
            i++;
        } while (i < 8);
        j++;
    } while (j < REPLAY_MAX_RACERS);
    j = 0;
    do {
        i = 0;
        do {
            ptr = sReplayReadPtr;
            bitPos = sReplayReadBitPos;
            bitPos = bitPos - 1;
            sReplayReadBitPos = bitPos;
            if (bitPos == 0) {
                nextPtr = ptr + 1;
                sReplayReadPtr = nextPtr;
                sReplayReadBitPos = 8;
            }
            i++;
        } while (i < 8);
        j++;
    } while (j < REPLAY_MAX_RACERS);
}

/* Starts playing a replay: reads its setup into gGame (replay_read_header) and its frame count. */
void replay_play_start(s32 replay)
{
    s32 byte;

    sReplayBlockFramesLeft = 0;
    sReplayReadPtr = replay;
    sReplayReadBitPos = 8;
    sReplayReadPtr = replay + REPLAY_OFS_SETUP;
    replay_read_header();
    sReplayPlayPlayers = gGame.numPlayers;
    sReplayFramesLeft = replay_read_bits(8);
    byte = replay_read_bits(8);
    sReplayFramesLeft = sReplayFramesLeft + byte * 0x100;
    byte = replay_read_bits(8);
    sReplayFramesLeft = sReplayFramesLeft + byte * 0x10000;
    mem_fill(&sReplayStickX[0], 0, REPLAY_MAX_RACERS);
    mem_fill(&sReplayStickY[0], 0, REPLAY_MAX_RACERS);
    mem_fill(&sReplayButtons[0], 0, REPLAY_MAX_RACERS);
}

/* Starts playing a replay with the setup already in gGame (the setup is skipped). */
void replay_play_start_keep_setup(s32 replay)
{
    s32 byte;

    sReplayBlockFramesLeft = 0;
    sReplayReadPtr = replay;
    sReplayReadBitPos = 8;
    sReplayReadPtr = replay + REPLAY_OFS_SETUP;
    replay_skip_header();
    sReplayPlayPlayers = gGame.numPlayers;
    sReplayFramesLeft = replay_read_bits(8);
    byte = replay_read_bits(8);
    sReplayFramesLeft = sReplayFramesLeft + byte * 0x100;
    byte = replay_read_bits(8);
    sReplayFramesLeft = sReplayFramesLeft + byte * 0x10000;
    mem_fill(&sReplayStickX[0], 0, REPLAY_MAX_RACERS);
    mem_fill(&sReplayStickY[0], 0, REPLAY_MAX_RACERS);
    mem_fill(&sReplayButtons[0], 0, REPLAY_MAX_RACERS);
}

/* Writes the next frame's input word of each player to inputs. Returns 1 (writing nothing) once the replay is
   over, else 0. */
s32 replay_play_frame(s32 *inputs)
{
    s32 i;

    if (sReplayFramesLeft == 0) {
        return 1;
    }
    if (sReplayBlockFramesLeft == 0) {
        replay_decode_block();
    }
    for (i = 0; i < sReplayPlayPlayers; i++) {
        inputs[i] = sReplayInputs[i][REPLAY_BLOCK_FRAMES - sReplayBlockFramesLeft];
    }
    sReplayBlockFramesLeft--;
    sReplayFramesLeft--;
    return 0;
}

/* Reads a replay's track, laps and frame count without playing it. Returns 1 if the magic is wrong, else 0. */
s32 replay_get_info(u8 *replay, u32 *track, u32 *laps, u32 *frames)
{
    u32 count;

    if (replay[0] != REPLAY_MAGIC) {
        return 1;
    }
    *track = replay[REPLAY_OFS_TRACK];
    *laps = replay[REPLAY_OFS_LAPS];
    count = replay[REPLAY_OFS_FRAMES];
    *frames = count;
    count = count + (replay[REPLAY_OFS_FRAMES + 1] << 8);
    *frames = count;
    *frames = count + (replay[REPLAY_OFS_FRAMES + 2] << 16);
    return 0;
}

/* Checks a ghost read from the Controller Pak (the Controller Pak screen's validate callback): the magic, the size
   and the checksum. Returns 0 if it is good, else 1. */
s32 replay_validate(u8 *replay, s32 size)
{
    s32 sum;
    s32 i;
    s32 len;
    u32 stored;
    u8 *data;

    if (replay[0] != REPLAY_MAGIC) {
        return 1;
    }
    if (size != REPLAY_GHOST_SIZE) {
        return 1;
    }
    data = replay + REPLAY_OFS_SETUP;
    len = REPLAY_GHOST_SIZE - REPLAY_OFS_SETUP;
    sum = 0;
    i = sum;
    do {
        u8 b = data[i];
        sum += b << (i % 8);
        i++;
    } while (i < len);
    sum &= 0xFFFFFF;
    stored = replay[1] | (replay[2] << 8) | (replay[3] << 16);
    return (stored ^ sum) != 0;
}

/* The replay checksum of len bytes: the sum of each byte shifted left by its index mod 8, to 24 bits. */
u32 calc_checksum24(u8 *data, s32 len)
{
    s32 i;
    u32 sum = 0;

    for (i = 0; i < len; i++) {
        sum += (u32)data[i] << (i % 8);
    }
    sum &= 0xFFFFFF;
    return sum;
}

/* The next bit of the replay being played. */
s32 replay_read_bit(void)
{
    u8 *ptr = sReplayReadPtr;
    s32 byte = *ptr;
    s32 bitPos = sReplayReadBitPos - 1;
    s32 bit;

    sReplayReadBitPos = bitPos;
    bit = byte >> bitPos;
    bit = bit & 1;
    if (bitPos == 0) {
        sReplayReadPtr = ptr + 1;
        sReplayReadBitPos = 8;
    }
    return bit;
}

/* The next numBits bits of the replay being played, first bit highest. */
s32 replay_read_bits(s32 numBits)
{
    s32 value = 0;
    s32 i;

    for (i = 0; i < numBits; i++) {
        u8 *ptr = sReplayReadPtr;
        s32 byte = *ptr;
        s32 bitPos = sReplayReadBitPos - 1;
        s32 bit;

        value = value << 1;
        sReplayReadBitPos = bitPos;
        bit = (byte >> bitPos) & 1;
        if (bitPos == 0) {
            sReplayReadPtr = ptr + 1;
            sReplayReadBitPos = 8;
        }
        value = value | bit;
    }
    return value;
}
