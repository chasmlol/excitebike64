#include "game.h"

typedef struct {
    u8 pad[0x20];
    u8 a;
    u8 b;
    s8 c;
    s8 d;
    u8 pad2[0x0C];
} Ent;

extern Ent *sSoftmixVoices_2;

s32 softmix_init(s32 param_1, u32 param_2, s32 param_3, s32 arg3)
{
    s32 *piVar1;
    s32 iVar2, iVar3, iVar4, iVar6;
    s16 *psVar5;

    if (param_1 < 9 && param_3 < 0x5623) goto body;
ret1:
    return 1;
body:
    {
        sSoftmixFlags = param_2;
        sSoftmixOutputRate = param_3;
        sSoftmixNumVoices = param_1;
        sSoftmixSampleCount = 0;
        sSoftmixEchoOn = 0;
        sSoftmixVoices = mem_alloc_dbg(param_1 * 0x30, 0, &sStrSoftmixFile, 0x2c8);
        if (sSoftmixVoices == 0) goto ret1;
        {
            mem_fill((u8 *)sSoftmixVoices, 0, param_1 * 0x30);
            mem_fill((u8 *)&sSoftmixSamples, 0, 0x1c00);
            sSoftmixVolumeTable = piVar1 = mem_alloc_dbg(0x8000, 0, &sStrSoftmixFile, 0x2d4);
            if (piVar1 == 0) {
                mem_free(sSoftmixVoices);
                return 2;
            }
            iVar6 = 0;
            if (param_2 & 2) {
                s32 *tp = piVar1;
                do {
                    iVar4 = 0;
                    do {
                        s32 prod = sSoftmixExpandTable[iVar6] * iVar4;
                        tp[iVar6 + iVar4 * 0x100] = prod / 0x1f;
                        iVar4 = iVar4 + 1;
                    } while (iVar4 < 0x20);
                    iVar6 = iVar6 + 1;
                } while (iVar6 < 0x100);
            } else {
                s32 *tp = piVar1;
                s32 sh;
                do {
                    iVar4 = 0;
                    sh = (s8)iVar6;
                    iVar3 = 0;
                    do {
                        tp[iVar6 + iVar4 * 0x100] = iVar3 / 0x1f;
                        iVar3 = iVar3 + sh;
                        iVar4 = iVar4 + 1;
                    } while (iVar4 < 0x20);
                    iVar6 = iVar6 + 1;
                } while (iVar6 < 0x100);
            }
            sMixVoices = sSoftmixVoices;
            sMixNumVoices = sSoftmixNumVoices;
            sMixVolumeTable = sSoftmixVolumeTable;
            if (res_open_set(&sStrSfxResSet, &gSfxResSet) != 0) {
                debug_printf((s32)&sStrNoSfxResSet);
                gSfxResSet = 0;
            } else {
                debug_printf((s32)&sStrOpenedResSet);
            }
            res_register_type(&sStrSamType, softmix_sam_decompress);
            return 0;
        }
}
}

typedef struct {
    s32 *data;
    s32 len;
    s32 b;
    s32 c;
    s32 d;
    s32 e;
    s32 f;
} Ent_102;

extern Ent_102 sSoftmixSamples_2[];

s32 softmix_load_sample_file(s32 param_1)
{
    s32 i;
    Ent_102 *e;
    s32 *f;
    u8 buf[8];
    u8 *bp;

    for (i = 0; i < 0x100; i++) {
        if (sSoftmixSamples_2[i].len == 0) break;
    }
    if (i == 0x100) return -1;
    mem_fill((u8 *)&sSoftmixSamples_2[i], 0, 0x1c);
    if (sSoftmixFlags & 2) {
        sprintf(sSoftmixNameBuf, sStrSampleFile16, param_1);
    } else {
        sprintf(sSoftmixNameBuf, sStrSampleFile08, param_1);
    }
    f = file_open(sSoftmixNameBuf, sStrModeRb);
    do { do { do { do { do { do { bp = buf; } while (0); } while (0); } while (0); } while (0); } while (0); } while (0);
    if (f == 0) return -2;
    file_read(bp, 1, 4, f);
    file_read((u8 *)&sSoftmixSamples_2[i].len, 1, 4, f);
    file_read((u8 *)&sSoftmixSamples_2[i].b, 1, 4, f);
    file_read((u8 *)&sSoftmixSamples_2[i].c, 1, 4, f);
    file_read((u8 *)&sSoftmixSamples_2[i].d, 1, 4, f);
    file_read(bp, 1, 4, f);
    e = &sSoftmixSamples_2[i];
    e->data = mem_alloc_dbg(e->len, 0, sStrSoftmixFile, 0x368);
    if (e->data == 0) {
        file_close(f);
        return -4;
    }
    file_read((u8 *)e->data, 1, e->len, f);
    file_close(f);
    return i;
}

typedef struct {
    void *p0;
    s32 f4;
    s32 f8;
    s32 fc;
    s32 f10;
    volatile s32 f14;
    s32 f18;
} SamEnt;

extern SamEnt sSoftmixSamples_3[];

s32 softmix_load_sample_name(s32 param_1, s32 arg1, s32 arg2, s32 arg3)
{
    s32 i;
    SamEnt *e;
    char *pcVar6;
    s32 *q;
    SamEnt *ent;
    s32 *pp;

    if (gSfxResSet == 0) {
        return -1;
    }
    for (i = 0; i < 0x100; i++) {
        if (sSoftmixSamples_3[i].f4 == 0) {
            break;
        }
    }
    if (i == 0x100) {
        return -1;
    }
    mem_fill((u8 *)&sSoftmixSamples_3[i], 0, 0x1c);
    if (sSoftmixFlags & 2) {
        sprintf(sSoftmixNameBuf, sStrSamName16, param_1);
    } else {
        sprintf(sSoftmixNameBuf, sStrSamName08, param_1);
    }
    q = (s32 *)res_load_by_name_2(gSfxResSet, sSoftmixNameBuf, &sSoftmixSamples_3[i].f18);
    ent = &sSoftmixSamples_3[i];
    ent->f14 = (s32)q;
    ent->f4 = ((volatile s32 *)q)[1];
    e = (SamEnt *)ent->f14;
    pp = (s32 *)ent->f14;
    ent->f8 = ((s32 *)e)[2];
    ent->fc = ((s32 *)e)[3];
    pcVar6 = (char *)pp;
    pp = (s32 *)pp[4];
    ent->p0 = pcVar6 + 0x18;
    ent->f10 = (s32)pp;
    return i;
}

typedef struct {
    s32 data;
    s32 f4;
    s32 f8;
    s32 fC;
    s32 f10;
    Gear *hdr;
    s32 id;
} Slot_16;

extern Slot_16 sSoftmixSamples_4[];

s32 softmix_load_sample_id(s32 param)
{
    s32 i;
    Slot_16 *e;

    if (gSfxResSet == NULL) {
        return -1;
    }
    for (i = 0; i < 0x100; i++) {
        if (sSoftmixSamples_4[i].f4 == 0) {
            break;
        }
    }
    if (i == 0x100) {
        return -1;
    }
    e = &sSoftmixSamples_4[i];
    mem_fill((u8 *)e, 0, sizeof(Slot_16));
    e->id = param;
    e->hdr = res_load(gSfxResSet, param);
    e->f4 = e->hdr->f4;
    e->f8 = e->hdr->f8;
    e->fC = e->hdr->fC;
    e->f10 = e->hdr->f10;
    e->data = (s32)(e->hdr + 1);
    return i;
}

typedef struct {
    void *ptr;
    s32 size;
    s32 f8;
    s32 fc;
    s32 f10;
    s32 f14;
    s32 f18;
} Ent_103;

extern Ent_103 sSoftmixSamples_5[];

s32 softmix_create_sample(u8 *p1, s32 p2, s32 p3, s32 p4, s32 p5)
{
    s32 n;
    s32 t;
    Ent_103 *e;
    Ent_103 *base;
    Ent_103 *w;
    void *q;

    base = sSoftmixSamples_5;
    n = 0;
    w = base;
top:
    if (w->size == 0) {
        goto done;
    }
    n++;
    t = n < 0x100;
    w++;
    if (t) {
        goto top;
    }
done:
    if (n == 0x100) {
        return -1;
    }
    e = (Ent_103 *)((s32)(n * 0x1C) + (s32)base);
    mem_fill((u8 *)e, 0, 0x1c);
    e->size = p2;
    if (p2 == 0) {
        return -3;
    }
    q = mem_alloc_dbg(p2, 0, sStrSoftmixFile, 0x421);
    e->ptr = q;
    if (q == 0) {
        return -4;
    }
    sfx_copy_bytes(q, p1, e->size);
    e->f8 = p3;
    e->fc = p4;
    e->f10 = p5;
    e->f18 = -1;
    return n;
}

typedef struct { u32 w[12]; } RecA_3;

void softmix_voice_set_sample(s32 param_1, s32 param_2)
{
    RecA_3 *a;
    Rec_3 *b;
    RecA_3 *ab;
    Rec_3 *bb;
    u32 t;
    u32 fl;
    critical_enter();
    ab = sSoftmixVoices;
    a = (RecA_3 *)((s32)(param_1 * 0x30) + (s32)ab);
    bb = sSoftmixSamples_6;
    b = (Rec_3 *)((s32)(param_2 * 0x1c) + (s32)bb);
    a->w[2] = a->w[2] % b->w[1];
    a->w[7] = b->w[2];
    a->w[9] = b->w[3];
    t = b->w[0];
    fl = a->w[1];
    a->w[10] = param_2;
    a->w[0] = t;
    if (fl & 2) {
        a->w[6] = b->w[3];
    } else {
        a->w[6] = b->w[1];
    }
    critical_leave();
}

void softmix_voice_set_volume(s32 i, s32 val)
{
    s32 a, b, x, y;

    sSoftmixVoices_2[i].a = val;
    if (sSoftmixFlags & 1) {
        a = sSoftmixVoices_2[i].a;
        b = sSoftmixVoices_2[i].b;

        x = 15 - b;
        x = x * a / 10;
        y = b * a / 10;
        if (x > a) x = a;
        if (y > a) y = a;
        sSoftmixVoices_2[i].c = x;
        sSoftmixVoices_2[i].d = y;
    }
}

void softmix_voice_set_pan(s32 i, s32 val)
{
    s32 a, b, x, y;

    sSoftmixVoices_2[i].b = val;
    if (sSoftmixFlags & 1) {
        a = sSoftmixVoices_2[i].a;
        b = sSoftmixVoices_2[i].b;

        x = 15 - b;
        x = x * a / 10;
        y = b * a / 10;
        if (x > a) x = a;
        if (y > a) y = a;
        sSoftmixVoices_2[i].c = x;
        sSoftmixVoices_2[i].d = y;
    }
}

typedef struct {
    u8 pad[0x22];
    u8 a;
    u8 b;
    u8 pad2[0xC];
} Ch_2;

extern Ch_2 *sSoftmixVoices_3;

void softmix_mix(s32 *param_1, s32 param_2)
{
    s32 i;
    s32 n;
    s32 ix;
    u8 *pb;

    sMixCount = param_2;
    sMixSrc = param_1;
    D_800DB8DD = 1;
    for (i = 0; i < sSoftmixNumVoices; i++) {
        sSoftmixSavedLevels[i * 2] = sSoftmixVoices_3[i].a;
        sSoftmixSavedLevels[i * 2 + 1] = sSoftmixVoices_3[i].b;
        sSoftmixVoices_3[i].a = (sSoftmixVoices_3[i].a * sSoftmixMasterVolume) >> 8;
        sSoftmixVoices_3[i].b = (sSoftmixVoices_3[i].a * sSoftmixMasterVolume) >> 8;
    }
    if (sSoftmixFlags & 1) {
        softmix_mix_voices_stereo();
    } else {
        softmix_mix_voices_mono();
    }
    if (sSoftmixEchoOn != 0) {
        n = param_2;
        if (sSoftmixFlags & 1) {
            n = param_2 << 1;
        }
        if (sSoftmixFlags & 2) {
            i = 0;
            if (0 < n) {
                s8 *tb;
                tb = sSoftmixEchoTable;
                do {
                    pb = sSoftmixEchoBuf;
                    ix = sSoftmixEchoPos;
                    param_1[i] += *(s8 *)((s32)pb[ix] + (s32)tb) << 8;
                    pb[ix] = param_1[i] >> 10;
                    ix++;
                    sSoftmixEchoPos = ix;
                    if (ix == sSoftmixEchoSize) {
                        sSoftmixEchoPos = 0;
                    }
                    i++;
                } while (i < n);
            }
        } else {
            i = 0;
            if (0 < n) {
                s8 *tb;
                tb = sSoftmixEchoTable;
                do {
                    pb = sSoftmixEchoBuf;
                    ix = sSoftmixEchoPos;
                    param_1[i] += *(s8 *)((s32)pb[ix] + (s32)tb);
                    pb[ix] = param_1[i] >> 2;
                    ix++;
                    sSoftmixEchoPos = ix;
                    if (ix == sSoftmixEchoSize) {
                        sSoftmixEchoPos = 0;
                    }
                    i++;
                } while (i < n);
            }
        }
    }
    sSoftmixSampleCount += param_2;
    for (i = 0; i < sSoftmixNumVoices; i++) {
        sSoftmixVoices_3[i].a = sSoftmixSavedLevels[i * 2];
        sSoftmixVoices_3[i].b = sSoftmixSavedLevels[i * 2 + 1];
    }
}

struct Hdr_13 {
    u32 flags;
    u32 size;
};

void softmix_sam_decompress(Hdr_13 **param_1)
{
    Hdr_13 *p;
    Hdr_13 *q;
    u8 **bp;

    p = *param_1;
    if ((p->flags & 0xF) >= 2) {
        if (sSoftmixDecompressFn != 0) {
            if (sSoftmixDecompressBufSize < p->size + 0x1c) goto spin;
            memcpy(sSoftmixDecompressBuf, p, p->size + 0x1c);
            mem_free(*param_1);
            p = (Hdr_13 *)sSoftmixDecompressBuf;
            goto join;
        spin:
            while (1) {}
        join:
            q = sSoftmixDecompressFn(p);
            bp = &sSoftmixDecompressBuf;
            if (q == 0) {
                fatal_halt(1);
            }
            if (p != (Hdr_13 *)*bp) {
                mem_free(p);
            }
            if (q == (Hdr_13 *)*bp) {
                p = mem_alloc_dbg(p->size + 0x1c, 0, sStrSoftmixFile, 0x67b);
                memcpy(p, q, p->size + 0x1c);
                q = p;
            }
            *param_1 = q;
        } else {
            fatal_halt(1);
        }
    }
}

s32 softmix_get_caps(s32 *a, s32 *b, s32 *c)
{
    *a = 8;
    *b = 3;
    *c = 0x5622;
    return 0;
}

void softmix_shutdown(void)
{
    mem_free(sSoftmixVolumeTable);
    mem_free(sSoftmixVoices);
    if (gSfxResSet != 0) {
        res_close_set(gSfxResSet);
    }
    gSfxResSet = 0;
}

void softmix_free_sample(u32 p)
{
    s32 *w;
    s32 *e;
    if (p < 0x101) {
        w = sSoftmixSamples_7;
        e = &w[p * 7];
        if (e[1] != 0) {
            if (e[6] != -1) {
                res_unload((u32 *)(gSfxResSet_2[1] + e[6] * 4));
            } else {
                mem_free(e[0]);
            }
            mem_fill((u8 *)&sSoftmixSamples_7[p * 7], 0, 0x1c);
        }
    }
}

typedef struct {
    s32 f[7];
} Rec_58;

extern Rec_58 sSoftmixSamples_8[];

void softmix_get_sample_info(s32 idx, s32 *out) {
    Rec_58 *base = sSoftmixSamples_8;
    Rec_58 *r = base + idx;
    out[0] = r->f[1];
    out[1] = r->f[2];
    out[2] = r->f[3];
    out[3] = r->f[4];
}

typedef struct {
    u32 pad;
    u32 w[4];
    u32 tail[2];
} Ent_104;

extern Ent_104 sSoftmixSamples_9[];

void softmix_set_sample_info(s32 i, u32 *src)
{
    Ent_104 *base = sSoftmixSamples_9;
    Ent_104 *e = &base[i];
    e->w[0] = src[0];
    e->w[1] = src[1];
    e->w[2] = src[2];
    e->w[3] = src[3];
}

void softmix_voice_set_rate(s32 param_1, s32 param_2)
{
    s32 iVar1;
    s32 iVar2;

    critical_enter();
    iVar1 = param_2 % sSoftmixOutputRate * 65536;
    iVar2 = iVar1 / sSoftmixOutputRate;
    iVar1 = param_1 * 0x30 + sSoftmixVoices;
    *(s32 *)(iVar1 + 0x10) = param_2 / sSoftmixOutputRate;
    *(s32 *)(iVar1 + 0x14) = iVar2 * 65536;
    critical_leave();
}

void softmix_voice_set_position(s32 i, s32 v)
{
    *(s32 *)((u8 *)sSoftmixVoices + i * 48 + 8) = v;
}

s32 softmix_voice_get_position(s32 a0)
{
    return *(s32 *)(sSoftmixVoices + a0 * 0x30 + 8);
}

typedef struct { s32 f0; s32 f1; s32 f2; s32 f3; s32 f4; s32 f5; s32 f6; } Row_7;
typedef struct { u8 pad0[4]; u32 flags; u8 pad8[0x10]; s32 val; u8 pad1c[0xC]; s32 idx; u8 pad2c[4]; } Ent_105;

extern Row_7 sSoftmixSamples_10[];

void softmix_voice_set_flags(s32 n, u32 flags)
{
    Ent_105 *base = sSoftmixVoices;
    Ent_105 *e = (Ent_105 *)((u32)(n * 0x30) + (u32)base);
    e->flags = flags;
    if (flags & 2) {
        e->val = sSoftmixSamples_10[e->idx].f3;
        return;
    }
    e->val = sSoftmixSamples_10[e->idx].f1;
}

s32 softmix_voice_get_flags(s32 i)
{
    return *(s32 *)((u8 *)sSoftmixVoices + i * 48 + 4);
}

s32 softmix_get_sample_count(s32 arg0) {
    return sSoftmixSampleCount;
}

void softmix_convert_output(s32 a, s32 b, s32 c) {
    s32 flags;

    sMixSrc = a;
    sMixDst = b;
    flags = sSoftmixFlags;
    sMixCount = c;
    if (flags & 1) {
        sMixCount = c << 1;
    }
    if (flags & 2) {
        D_800DB8D0 = 0;
        D_800DB8DC = 0;
        sMixClampMin = (s32)0xFFFF8001;
        sMixClampMax = 0x7FFF;
        softmix_output_16bit();
    } else {
        D_800DB8DC = 0;
        D_800DB8D0 = 0;
        sMixClampMin = 0;
        sMixClampMax = 0xFF;
        softmix_output_8bit();
    }
}

s32 softmix_echo_init(s32 size)
{
    s32 i;
    u8 *q;

    if (sSoftmixEchoOn != 0) {
        return -1;
    }
    if (sSoftmixFlags & 1) {
        size <<= 1;
    }
    sSoftmixEchoBuf = q = mem_alloc_dbg(size, 0, sStrSoftmixFile, 0x601);
    if (q == NULL) {
        return 1;
    }
    mem_fill(q, 0, size);
    sSoftmixEchoSize = size;
    i = 0xFF;
    {
        u8 *pp = sSoftmixEchoTable + i;
        do {
            *pp = i;
            i--;
            pp--;
        } while (i >= 0);
    }
    sSoftmixEchoPos = 0;
    sSoftmixEchoOn = 1;
    return 0;
}

void softmix_echo_enable(s32 p)
{
    if (p != 0) {
        sSoftmixEchoOn = 1;
        return;
    }
    if (sSoftmixEchoOn != 0) {
        mem_fill(sSoftmixEchoBuf, 0, sSoftmixEchoSize);
    }
    sSoftmixEchoOn = 0;
}

void softmix_echo_set_level(s32 k)
{
    s32 i = 0;
    s32 x;
    u8 *dst;
    do {
        x = (s8)i * k;
        dst = sSoftmixEchoTable + i;
        if (x < 0) {
            x += 0xFF;
        }
        *dst = (u8)(x >> 8);
        i++;
    } while (i < 0x100);
}

void softmix_echo_free(s32 arg0)
{
    if (sSoftmixEchoOn != 0) {
        mem_free(sSoftmixEchoBuf);
        sSoftmixEchoOn = 0;
    }
}

typedef struct {
    u8 *buf;
    u32 len;
    u8 unk8[0x1C - 0x8];
} Chan_7;

extern Chan_7 sSoftmixSamples_11[];

void softmix_write_sample(u32 idx, u8 *src, s32 n) {
    s32 i;

    if (idx <= 0x100) {
        if (sSoftmixSamples_11[idx].len < n) {
            n = sSoftmixSamples_11[idx].len;
        }
        if (sSoftmixFlags & 2) {
            sfx_copy_bytes(sSoftmixSamples_11[idx].buf, src, n);
        } else {
            for (i = 0; i < n; i++) {
                sSoftmixSamples_11[idx].buf[i] = sSoftmixExpandTable[src[i]] >> 8;
            }
        }
    }
}

void softmix_set_decompressor(s32 a0)
{
    sSoftmixDecompressFn = a0;
}

void softmix_set_decompress_buf(s32 a, s32 b)
{
    sSoftmixDecompressBuf = a;
    sSoftmixDecompressBufSize = b;
}

void softmix_voice_update_levels(s32 i)
{
    s32 a, b, x, y;
    if (sSoftmixFlags & 1) {
        a = sSoftmixVoices_2[i].a;
        b = sSoftmixVoices_2[i].b;

        x = 15 - b;
        x = x * a / 10;
        y = b * a / 10;
        if (x > a) x = a;
        if (y > a) y = a;
        sSoftmixVoices_2[i].c = x;
        sSoftmixVoices_2[i].d = y;
    }
}

s32 softmix_get_sample_data(s32 a0)
{
    return *(s32 *)(sSoftmixSamples_12 + a0 * 0x1C);
}

void softmix_set_master_volume(s32 x)
{
    sSoftmixMasterVolume = x;
}
