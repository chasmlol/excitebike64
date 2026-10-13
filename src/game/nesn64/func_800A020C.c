#include "ctx.h"

typedef struct { u8 b[0x220]; } Blk;
typedef struct { s32 a; s32 b; s32 pad[7]; } Ent;
typedef struct { u8 pad[0x2001]; u8 flags; u8 pad2[0x1412C - 0x2002]; Ent e[4]; } Big;
typedef struct { u8 pad[0x10000]; u8 buf[0x2800]; } Vram;

extern Gfx *D_800F5A44;
extern s32 D_800F5A98;
extern s32 D_800F5A48;
extern s32 D_800F5A4C;
extern s32 D_800F5AA0;
extern s32 D_800CEC1C;
extern s32 D_800CEC20;
extern s32 D_800CEC00;
extern u8 D_800CEBE0[];
extern Vram *D_800D0670;
extern u32 D_800CEC08[];
extern Blk D_800F1BD4;

Gfx *func_8009FDF4(s32 a0, s32 a1, Gfx *gdl);
void func_800A2D64(void);
void func_800A0D60(s32 a);
s32 func_800A1AA0(void);
void func_800A1898(void);
void func_8000ED10(u8 *a0, u8 a1, s32 a2);
void func_8006F5F8();

#define SAVE (*(Blk *)0x803805E0)
#define BIG ((Big *)0x80380000)
#define FLAGS (BIG->flags)
#define CART (*(u16 *)0x80394100)

void func_800A020C(s32 a0, s32 a1, Gfx **pgdl)
{
    u8 *p;
    s32 k;
    s32 i;
    s32 t;

    D_800F5A44 = *pgdl;
    D_800F5A44 = func_8009FDF4(a0, a1, D_800F5A44);
    if (D_800F5A98 != 0) {
        *pgdl = D_800F5A44;
        return;
    }
    {
        Gfx *g = D_800F5A44++;
        g->words.w0 = 0xED090000;
        g->words.w1 = 0x4703C0;
    }
    D_800F5A48 = 0;
    D_800F5A4C = -1;
    func_800A2D64();
    D_800F5AA0++;
    D_800CEC20 = D_800CEC1C / D_800F5AA0;
    gDPPipeSync(D_800F5A44++);
    func_800A0D60(D_800CEC20);
    gDPPipeSync(D_800F5A44++);
    if (func_800A1AA0() != 0) {
        k = 0x2342;
        if ((D_800F5AA0 / 30) & 1) {
            if (D_800CEBE0[0] != 0) {
                i = 0;
                do {
                    if (D_800CEBE0[i] != ' ') {
                        D_800D0670->buf[k] = D_800CEBE0[i] - 0x37;
                        D_800D0670->buf[k + 0x400] = D_800CEBE0[i] - 0x37;
                    }
                    i++;
                    k++;
                } while (D_800CEBE0[i] != 0);
            }
            func_8000ED10(&D_800D0670->buf[0x23F0], 0x5F, 8);
        } else {
            t = 0;
            if (D_800CEBE0[0] != 0) {
                do {
                    t++;
                    D_800D0670->buf[k] = 0xFC;
                    D_800D0670->buf[k + 0x400] = 0xFC;
                    k++;
                } while (D_800CEBE0[t] != 0);
            }
        }
    }
    if (CART == 0xC661 || CART == 0xC66A) {
        if (D_800CEC00 == 0) {
            D_800CEC00 = 0x3C;
            D_800F1BD4 = SAVE;
            func_8006F5F8(0, 0);
        }
    } else if (CART == 0xC641 || CART == 0xC69A) {
        if (D_800CEC00 == 0) {
            D_800CEC00 = 0x3C;
            SAVE = D_800F1BD4;
        }
    }
    if (!(FLAGS & 8) || D_800F5AA0 < 10) {
        gDPPipeSync(D_800F5A44++);
        gDPSetCycleType(D_800F5A44++, G_CYC_FILL);
        t = FLAGS >> 5;
        gDPSetFillColor(D_800F5A44++, D_800CEC08[t]);
        {
            Gfx *g = D_800F5A44++;
            g->words.w0 = 0xF64703C0;
            g->words.w1 = 0x90000;
        }
        gDPPipeSync(D_800F5A44++);
        gDPSetCycleType(D_800F5A44++, G_CYC_1CYCLE);
    }
    if (D_800CEC00 != 0) {
        for (i = 0; i < 4; i++) {
            BIG->e[i].a = 0;
            BIG->e[i].b = 0;
        }
    }
    *pgdl = D_800F5A44;
    func_800A1898();
}
