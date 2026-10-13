#include "ctx.h"

typedef struct OSScTask_s {
    struct OSScTask_s *next;    /* 0x00 */
    u32 state;                  /* 0x04 */
    u32 flags;                  /* 0x08 */
    void *framebuffer;          /* 0x0C */
    OSTask list;                /* 0x10 */
    OSMesgQueue *msgQ;          /* 0x50 */
    OSMesg msg;                 /* 0x54 */
    u64 startTime;              /* 0x58 */
    u64 totalTime;              /* 0x60 */
} OSScTask;

typedef struct Ctx {
    u8 pad0[0x440];
    Gfx *dl;                    /* 0x440 */
    u8 pad444[0x4C8 - 0x444];
    s16 msgtype;                /* 0x4C8 */
    u8 pad4CA[0x4E8 - 0x4CA];
    OSScTask task;              /* 0x4E8 */
    u8 pad550[0x560 - 0x550];
} Ctx;

typedef struct { u16 button; s8 x, y; } Pad;
typedef struct { s32 w[4]; } Blk16;

typedef struct St {
    void (*fn0)(s32);           /* 0x00 */
    void (*fn4)(Pad *);         /* 0x04 */
    void (*fn8)(Ctx *, s32, Gfx **); /* 0x08 */
    void (*fnC)(s32);           /* 0x0C */
    s32 mode;                   /* 0x10 */
    s32 dlsize;                 /* 0x14 */
    s32 cnt;                    /* 0x18 */
} St;

typedef struct { u8 pad[8]; s16 arr[1]; } Trk;
typedef struct { Trk *trk; u8 pad4[0x60]; s16 f64; } Race;
typedef struct { u8 pad[0xA0]; } Rec;

typedef struct {
    s32 flags;                  /* 0x000 */
    u8 pad4[0x18];
    s32 f1C[6];                 /* 0x01C */
    u8 pad34[0x8];
    s32 f3C;                    /* 0x03C */
    u8 pad40[0xC];
    s32 f4C;                    /* 0x04C */
    u8 pad50[0x43C - 0x50];
    u8 f43C[6];                 /* 0x43C */
    u8 pad442[0x778 - 0x442];
    s32 f778[6];                /* 0x778 */
    s32 f790[6];                /* 0x790 */
} Game;

extern u32 D_80000318;
extern s32 D_80100634;
extern s32 D_800C2934;
extern s32 D_800C2938;
extern s32 D_800C293C;
extern s32 D_800C2940;
extern s32 D_800C2944;
extern s32 D_800C2948;
extern s32 D_800C294C;
extern s32 D_800C2950;
extern s32 D_800C2954;
extern volatile s32 D_800C2958;
extern St D_800C295C[];
extern St *D_800C29CC;
extern u16 *D_800C29D8[2];
extern void *D_800C29E0;
extern s32 D_800C29E4;
extern s32 D_800C29E8;
extern s32 D_800C29EC;
extern s32 D_800C8CE8;
extern s32 D_800CA920;
extern s32 D_800CA924[2];
extern Race *D_800CA930;
extern s32 D_800CB120;
extern Rec D_800CB210[];
extern s32 D_800CCCA4;
extern volatile s32 D_800CCCA8;
extern s32 D_800CCD64;
extern char D_800D6014[];
extern u8 D_800D19E0[];
extern u8 D_800A9C30[];
extern u8 D_800A9D00[];
extern u8 D_800AB0A0[];
extern Pad D_800E1E40[4];
extern s32 D_800E1E18;
extern s32 D_800E2030;
extern s32 D_800E2034;
extern s32 D_800E2038;
extern void *D_800E203C;
extern u8 D_800E2040[];
extern OSMesgQueue D_800E2048;
extern s32 D_800E2060;
extern s32 D_800E2064;
extern u8 D_800E20A8[];
extern s32 D_800E20D4;
extern Gfx *D_800E20E0;
extern s32 D_800E20E4;
extern s32 D_800E20E8;
extern s32 D_800E20EC;
extern OSMesg D_800E20F0[];
extern s32 D_800E2134;
extern Pad D_800E25B0[4];
extern Ctx D_800E25C0[2];
extern Ctx *D_800E3080;
extern void *D_800E3084;
extern s32 D_800E308C;
extern OSMesgQueue D_800E3090;
extern s32 D_800E30A8;
extern Pad D_800E30B0[4];
extern s32 D_800E30C0;
extern void *D_800E30C4;
extern OSThread D_800E30C8;
extern s32 D_800E3278;
extern s32 D_800E76D0;
extern Game D_800EFD70;
extern Rec *D_800EFD64;
extern s32 D_800F13EC;
extern s32 D_800F13F0;

extern void func_8005B5E0(void);
extern void func_80065A5C(void);
extern void func_8000ECB8() __attribute__((noreturn));
extern void func_800C0258(s32 a0);
extern void func_8000ED10(u8 *a0, u8 a1, s32 a2);
extern void func_80065830(s32 a0);
extern void func_800657FC(s32 a0, s32 a1);
extern void func_8000D064(void);
extern void func_8000CD00(void);
extern void func_8000CEAC(void);
extern void func_8000CFE8(void);
extern void func_8000E31C(Pad *, Pad *);
extern void func_8000E5C0(void);
extern void func_8000E6E8(void *);
extern void func_8000E8B0(s32);
extern void func_8000E8F4(void);
extern void func_8000E980(void);
extern void func_8000ED30(char *, char *);
extern void func_8000EE30(s32, char *);
extern void func_8000F060(void);
extern void func_80010D44(void);
extern void func_800AD990(void);
extern s32 func_800AEC10(char *);
extern void func_800B05E0(OSMesgQueue *, OSMesg *, s32);
extern s32 func_800B0610(OSMesgQueue *, OSMesg *, s32);
extern s32 func_800B1330(void);
extern s32 func_800B1340(void);
extern void func_800B1B40(OSThread *, s32, void (*)(void *), void *, void *, s32);
extern void func_800B1DE0(OSThread *);
extern void *func_800BC908(s32, s32, char *, s32);
extern void func_800BCB04(void *);
extern void *func_800BCCF0(s32, s32, char *, s32);
extern void func_800BDF80(s32);
extern void func_800BEE78(void);

void func_8000D2E0(void)
{
    char buf[0x780];
    OSMesg msg;
    s32 st;
    s32 cfg;
    s32 sz;
    OSScTask *q = NULL;
    s32 i;
    u32 base;
    Pad *pads;
    St *next;
    St *cur;

    msg = NULL;
    cfg = *(u16 *)0x80000402;
    if (cfg == 0xDEAD) {
        D_800C2950 = 0;
    } else if (cfg == 0xBEEF) {
        D_800C2950 = 1;
    } else {
        for (;;) {
        }
    }
    if (D_800C2950 != 0) {
        __builtin_memcpy(D_800E20A8, (void *)0x803FFFC0, 0x30);
        D_800C294C = D_800E20D4;
        cfg = *(u16 *)0x80000406;
        if (cfg == 0xDEAF) {
            D_800C2954 = 1;
        }
    }
    func_8000ED10((u8 *)D_800E25C0, 0, sizeof(D_800E25C0));
    if (D_80000318 == 0x800000) {
        D_800C2940 = 1;
    } else {
        D_800C2940 = 0;
    }
    D_800E2034 = 0;
    D_800E2038 = 0;
    D_800E2030 = -1;
    D_800E2064 = -1;
    func_8000E5C0();
    if (D_800C2940 != 0) {
        base = 0x80600000;
        D_800E20E8 = 640;
        D_800E20EC = 480;
    } else {
        base = 0x80300000;
        D_800E20E8 = 512;
        D_800E20EC = 240;
    }
    sz = D_800E20E8 * D_800E20EC * 4 + 0x40;
    D_800C29D8[0] = func_800BCCF0(base - sz, sz - 0x10, D_800D6014, 361);
    D_800C29D8[1] = (u16 *)((u8 *)D_800C29D8[0] + D_800E20E8 * D_800E20EC * 2);
    {
    void *z;
    D_800C29E0 = z = func_800BCCF0(base, D_800E20E8 * D_800E20EC * 2, D_800D6014, 363);
    if (D_800C29D8[0] == NULL || z == NULL) {
        func_8000ECB8(1);
    }
    }
    func_8000ED10((u8 *)D_800C29D8[0], 0, D_800E20E8 * D_800E20EC * 4);
    func_800BEE78();
    func_8000E980();
    func_8000ED10((u8 *)D_800E30B0, 0, sizeof(D_800E30B0));
    func_8000CFE8();
    func_8000D064();
    func_8000CD00();
    func_8000CEAC();
    for (i = 0; i < 4; i++) {
        func_8000E31C(&D_800E1E40[i], &D_800E30B0[i]);
    }
    D_800E203C = func_800BC908(0xC800, 0, D_800D6014, 405);
    D_800E30C4 = func_800BC908(0x401, 0, D_800D6014, 412);
    D_800E3084 = func_800BC908(0xC00, 0, D_800D6014, 413);
    func_800BDF80(D_800C294C);
    func_800C0258(0);
    D_800C29CC = &D_800C295C[0];
    D_800E25C0[0].dl = NULL;
    D_800E25C0[1].dl = NULL;
    func_8005B5E0();
    func_800B05E0(&D_800E2048, D_800E20F0, 16);
    func_800B1B40(&D_800E30C8, 5, func_8000E6E8, NULL, D_800E25B0, 12);
    func_800B1DE0(&D_800E30C8);
    func_800657FC((s32)D_800E2040, (s32)&D_800E2048);
    func_8000F060();
    D_800CB120 = 0;
    func_8000E8F4();
    func_8000E8B0(0);

    for (;;) {
        if (D_800C2934 != 0) {
            for (i = 0; i < D_800C2938; i++) {
            }
            D_800C2938 += 100;
        }
        D_800E3278 = func_800B1340();
        {
        s32 n;
        D_800E2060 = D_80000318 - (n = D_80100634 * 16 + 0x400000);
        }
        D_800E2060 = D_800E2060 - D_800E20E8 * D_800E20EC * 6 + 0xB4000;
        func_800B0610(&D_800E3090, &msg, 1);
        switch (*(s16 *)msg) {
        case 1:
            if (D_800C29CC->mode == 0) {
                if (D_800E25C0[0].dl == NULL) {
                    D_800E25C0[0].dl = func_800BC908(D_800C29CC->dlsize * 8, 0, D_800D6014, 523);
                    D_800E25C0[1].dl = func_800BC908(D_800C29CC->dlsize * 8, 0, D_800D6014, 524);
                }
                i = D_800C29CC - D_800C295C;
                if (i == 0 || i == 2) {
                    st = 1;
                } else if (i == 3) {
                    st = 0;
                } else if (i == 1) {
                    st = 1;
                }
                D_800CCCA4 = st;
                while (D_800CCCA4 != D_800CCCA8) {
                }
                func_8000ED10((u8 *)D_800C29D8[0], 0, D_800E20E8 * D_800E20EC * 2);
                func_8000ED10((u8 *)D_800C29D8[1], 0, D_800E20E8 * D_800E20EC * 2);
                D_800C29CC->mode = 2;
                D_800C29E4 = 4;
                if (D_800EFD70.flags & 0x8000) {
                    D_800C8CE8 = 1;
                } else {
                    D_800C8CE8 = 0;
                }
                D_800C29CC->fn0(0);
                break;
            }
            D_800E2038++;
            if (D_800E1E18 != 0) {
                func_8000CD00();
                func_8000CEAC();
            }
            if ((D_800E1E40[0].button & 0xC00) == 0xC00) {
                func_8000E8B0(1);
                do {
                    func_8000D064();
                    func_8000CD00();
                    func_8000CEAC();
                } while ((D_800E1E40[0].button & 0x300) != 0x300);
                func_8000E8B0(0);
                do {
                    func_8000D064();
                    func_8000CD00();
                    func_8000CEAC();
                } while ((D_800E1E40[0].button & 0x300) == 0x300);
            }
            func_8000D064();
            *(Blk16 *)D_800E25B0 = *(Blk16 *)D_800E30B0;
            for (i = 0; i < 4; i++) {
                func_8000E31C(&D_800E1E40[i], &D_800E30B0[i]);
            }
            pads = D_800E30B0;
            D_800E30C0 = func_800B1330();
            func_80010D44();
            D_800E20E4 = func_800B1330();
            D_800E2134 = D_800E20E4 - D_800E30C0;
            if (D_800E2038 - D_800E2064 > 0 || D_800C29CC->mode == 5) {
                if (D_800C29CC->mode == 2) {
                    if (D_800C29CC->fn4 != NULL) {
                        D_800C29CC->fn4(pads);
                    }
                }
                if (D_800C29CC->mode == 3) {
                    D_800C29CC->mode = 5;
                    D_800C29CC->cnt = 10;
                }
                if (D_800C29CC->mode == 5 && D_800C29CC->cnt == 0) {
                    D_800C29CC->mode = 3;
                }
                if (D_800C29CC->mode == 3) {
                    D_800C29CC->mode = 4;
                    D_800C29CC->fnC(0);
                    i = D_800C29CC - D_800C295C;
                    D_800C29CC->mode = 0;
                    if (i == 2) {
                        i = 0;
                        if (D_800EFD70.flags & 2) {
                            D_800CA920 = 6;
                        } else {
                            D_800EFD64 = &D_800CB210[21];
                            i = 1;
                        }
                    } else if (i == 3) {
                        i = 0;
                    } else if (D_800EFD70.flags & 0x40) {
                        i = 3;
                    } else if (i == 0 && (D_800EFD70.flags & 0x20)) {
                        i = 2;
                    } else if (D_800CA924[0] > 0 && D_800CA924[1] < D_800CA924[0]) {
                        i = 1;
                    } else {
                        if (D_800EFD70.f4C == 1 && D_800CA930 != NULL && !(D_800EFD70.flags & 0x8000)) {
                            D_800EFD64 = &D_800CB210[D_800CA930->trk->arr[D_800CA930->f64]];
                        }
                        if (i == 1) {
                            if (D_800EFD70.flags & 0x40000) {
                                s32 k;
                                for (k = 0; k < 6; k++) {
                                    D_800EFD70.f1C[k] = D_800EFD70.f778[k];
                                    D_800EFD70.f43C[k] = D_800EFD70.f790[k];
                                }
                            }
                        }
                        if (i == 1 && D_800CA920 == 8) {
                            i = 2;
                        } else if (i == 1 && (D_800EFD70.flags & 0x808000) == 0x800000) {
                            D_800CCD64 = 1;
                            D_800EFD70.f4C = 0;
                            D_800EFD70.flags = (D_800EFD70.flags | 0x4000000) & 0xFF0BFF7F;
                            D_800CA920 = 6;
                            D_800EFD64 = &D_800CB210[13];
                            D_800EFD70.f3C = 1;
                        } else if (!(D_800EFD70.flags & 4)) {
                            i ^= 1;
                        }
                    }
                    next = &D_800C295C[i];
                    if (D_800C29CC->dlsize != next->dlsize) {
                        func_800BCB04(D_800E25C0[0].dl);
                        func_800BCB04(D_800E25C0[1].dl);
                        D_800E25C0[0].dl = NULL;
                    }
                    D_800C29CC = next;
                }
                D_800E2064 = D_800E2038;
            }
            D_800E30C0 = func_800B1330();
            func_80010D44();
            D_800E20E4 = func_800B1330();
            D_800E2134 = D_800E20E4 - D_800E30C0;
            if (D_800C2958 < 2 && D_800C29CC->fn8 != NULL) {
                if (D_800C29CC->mode == 5) {
                    if (D_800C2958 == 0) {
                        D_800C29CC->cnt--;
                    }
                } else if (D_800C29CC->mode == 2) {
                    D_800E2030 = D_800E2038;
                    D_800E3080 = &D_800E25C0[D_800C29E8];
                    D_800E20E0 = D_800E3080->dl;
                    {
                    OSScTask *t = &D_800E25C0[D_800C29E8].task;
                    if (D_800E76D0 < 60) {
                        func_80065A5C();
                    }
                    D_800CB120 = 1;
                    D_800E30A8 = D_800C29E8;
                    D_800C29CC->fn8(D_800E3080, D_800C29E8, &D_800E20E0);
                    D_800CB120 = 0;
                    gDPPipeSync(D_800E20E0++);
                    gDPFullSync(D_800E20E0++);
                    gSPEndDisplayList(D_800E20E0++);
                    D_800E308C = D_800E20E0 - D_800E3080->dl;
                    cur = D_800C29CC;
                    if (D_800C29EC < D_800E308C) {
                        D_800C29EC = D_800E308C;
                    }
                    if (D_800E308C >= cur->dlsize) {
                        for (;;) {
                        }
                    }
                    func_800AD990();
                    t->list.t.data_ptr = (u64 *)D_800E3080->dl;
                    q = t;
#define T q
                    T->list.t.data_size = (D_800E20E0 - D_800E3080->dl) * sizeof(Gfx);
                    T->list.t.type = 1;
                    T->list.t.flags = 6;
                    T->list.t.ucode_boot = (u64 *)D_800A9C30;
                    T->list.t.ucode_boot_size = D_800A9D00 - D_800A9C30;
                    T->list.t.ucode = (u64 *)D_800AB0A0;
                    T->list.t.ucode_data = (u64 *)D_800D19E0;
                    T->list.t.ucode_data_size = 0x800;
                    T->list.t.dram_stack = D_800E30C4;
                    T->list.t.dram_stack_size = 0x400;
                    T->list.t.output_buff = NULL;
                    T->list.t.output_buff_size = NULL;
                    T->list.t.yield_data_ptr = D_800E3084;
                    T->list.t.yield_data_size = 0xC00;
                    T->next = NULL;
                    T->flags = 0x63;
                    T->msgQ = &D_800E2048;
                    D_800E25C0[D_800C29E8].msgtype = 2;
                    T->msg = &D_800E25C0[D_800C29E8].msgtype;
                    T->framebuffer = D_800C29D8[D_800C29E8];
                    T->totalTime = 0;
                    func_80065830((s32)q);
                    D_800C2958++;
                    D_800C29E8 = 1 - D_800C29E8;
                    D_800E30C0 = func_800B1330();
                    func_80010D44();
                    D_800E20E4 = func_800B1330();
                    D_800E2134 = D_800E20E4 - D_800E30C0;
                    }
                }
            }
            break;
        case 2:
            if (D_800C2948 != 0) {
                s32 x, y;
                __builtin_strcpy(buf, "e:\\log\\scr");
                D_800C293C = D_800E2038 * 765 % 99999;
                func_8000EE30(D_800C293C, buf + func_800AEC10(buf));
                func_8000ED30(buf, ".raw");
                D_800C293C++;
                for (y = 0; y < D_800F13F0; y++) {
                    for (x = 0; x < D_800F13EC; x++) {
                        s32 c = D_800C29D8[D_800C2944][y * D_800F13EC + x];
                        buf[x * 3 + 0] = (c >> 11) << 3;
                        buf[x * 3 + 1] = (c >> 6) << 3;
                        buf[x * 3 + 2] = (c >> 1) << 3;
                    }
                }
                D_800C2948 = 0;
            }
            D_800C2944 = 1 - D_800C2944;
            D_800C2958--;
            break;
        }
    }
}
