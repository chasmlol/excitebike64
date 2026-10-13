#include "ctx.h"

typedef struct Glyph { s8 a; s8 b; s8 x; s8 y; u8 w; u8 h; s8 adv; u8 page; } Glyph;
typedef struct Font { u8 *kern; u8 pad4[2]; u8 first; u8 count; s8 spacing; s8 h9; u8 padA[2]; s8 hC; s8 hD; u8 padE[2]; Glyph g[1]; } Font;
typedef struct Ent { struct Ent *next; s16 x; s16 y; u8 w; u8 h; u8 a; u8 b; u32 color; } Ent;

extern Ent *D_8010536C;
extern Ent *D_801054E0[];
extern u32 *D_8010537C;
extern Font *D_80105380[];
extern void (*D_80105368)(s32, s32, s32, s32, s32);
extern s32 D_80105378;
extern s32 D_800D575C;
extern s32 D_800D576C;
extern s32 D_800D5758;
extern s32 D_801053C0[][2];

u8 *func_800C0ACC(s32 *p, s32 x, s32 y)
{
    u8 *s = (u8 *)(p + 8);
    u32 flags = p[2];
    s32 n;
    u32 color;
    Font *f;
    Glyph *g;
    Ent *e;
    u32 c, i;
    s32 x0, scale, m8, m8b, adv;
    u8 *kp, *kq, *kern;

    color = p[1];
    n = color + color * 2;
    color = n + 1;
    if (flags & 0x20000000) {
        color = n;
    } else if (flags & 0x40000000) {
        color = n + 2;
    }
    x += p[4];
    y += p[5];
    color = D_8010537C[color];
    f = D_80105380[p[0]];
    x0 = x;
    if (D_800D575C != 0x10000) {
        scale = 1;
        if (D_800D576C != 0)
            scale = D_800D575C >> 16;
        c = *s;
        if (c != 0) {
            m8 = flags & 0x8000000;
            do {
                s++;
                if (c == 10) {
                    x = x0;
                    y += f->hC + f->hD + f->h9;
                } else {
                    i = c - f->first;
                    if (i >= f->count)
                        return s;
                    g = &f->g[i];
                    if (c - 0x80 < 0x10) {
                        D_801053C0[c - 0x80][0] = x + g->x;
                        D_801053C0[c - 0x80][1] = y + g->y;
                    }
                    if (m8 != 0) {
                        kp = (u8 *)(D_80105378 - (g->page << 12));
                        D_80105368((s32)kp + (g->a + (g->b << 6)),
                                   x + g->x, y + g->y, g->w, g->h);
                    } else {
                        e = D_8010536C;
                        D_8010536C = e + 1;
                        e->next = D_801054E0[g->page];
                        D_801054E0[g->page] = e;
                        e->x = (s8)((u8 *)g)[2] + x;
                        e->y = y + g->y * scale;
                        e->w = g->w;
                        e->h = g->h;
                        e->a = g->a;
                        e->b = g->b;
                        e->color = color;
                        if (flags & 0x10000000) {
                            e = D_8010536C;
                            D_8010536C = e + 1;
                            e->next = D_801054E0[g->page];
                            D_801054E0[g->page] = e;
                            e->x = (s8)((u8 *)g)[2] + x + 2;
                            e->y = y + (g->y + 1) * scale;
                            e->w = g->w;
                            e->h = g->h;
                            e->a = g->a;
                            e->b = g->b;
                            e->color = color & 0xFF;
                        }
                    }
                    adv = g->adv + f->spacing;
                    kp = f->kern;
                    if (kp != 0) {
                        do { do { kern = kp + i; } while (0); } while (0);
                        if (*kern != 0) {
                        kp += *kern * 2;
                        i = *s;
                        while (*kp != 0) {
                            if (*kp == i) {
                                adv += (s8)kp[1];
                                break;
                            }
                            kp += 2;
                        }
                        }
                    }
                    do { x += (adv * D_800D5758) >> 16; } while (0);
                }
                c = *s;
            } while (c != 0);
        }
    } else {
        c = *s;
        if (c != 0) {
            m8b = flags & 0x8000000;
            do {
                s++;
                if (c == 10) {
                    x = x0;
                    y += f->hC + f->hD + f->h9;
                } else {
                    i = c - f->first;
                    if (i >= f->count)
                        return s;
                    g = &f->g[i];
                    if (c - 0x80 < 0x10) {
                        D_801053C0[c - 0x80][0] = x + g->x;
                        D_801053C0[c - 0x80][1] = y + g->y;
                    }
                    if (m8b != 0) {
                        kq = (u8 *)(D_80105378 - (g->page << 12));
                        D_80105368((s32)kq + (g->a + (g->b << 6)),
                                   x + g->x, y + g->y, g->w, g->h);
                    } else {
                        e = D_8010536C;
                        D_8010536C = e + 1;
                        e->next = D_801054E0[g->page];
                        D_801054E0[g->page] = e;
                        e->x = (s8)((u8 *)g)[2] + x;
                        e->y = y + g->y * (D_800D575C >> 16);
                        e->w = g->w;
                        e->h = g->h;
                        e->a = g->a;
                        e->b = g->b;
                        e->color = color;
                        if (flags & 0x10000000) {
                            e = D_8010536C;
                            D_8010536C = e + 1;
                            e->next = D_801054E0[g->page];
                            D_801054E0[g->page] = e;
                            e->x = (s8)((u8 *)g)[2] + x + 2;
                            e->y = (s8)((u8 *)g)[3] + y + 1;
                            e->w = g->w;
                            e->h = g->h;
                            e->a = g->a;
                            e->b = g->b;
                            e->color = color & 0xFF;
                        }
                    }
                    adv = g->adv + f->spacing;
                    kq = f->kern;
                    if (kq != 0) {
                        do { do { kern = kq + i; } while (0); } while (0);
                        if (*kern != 0) {
                        kq += *kern * 2;
                        i = *s;
                        while (*kq != 0) {
                            if (*kq == i) {
                                adv += (s8)kq[1];
                                break;
                            }
                            kq += 2;
                        }
                        }
                    }
                    do { x += adv; } while (0);
                }
                c = *s;
            } while (c != 0);
        }
    }
    return s;
}
