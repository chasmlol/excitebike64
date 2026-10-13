#include "game.h"

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

typedef struct St {
    void (*fn0)(s32);           /* 0x00 */
    void (*fn4)(Pad *);         /* 0x04 */
    void (*fn8)(Ctx *, s32, Gfx **); /* 0x08 */
    void (*fnC)(s32);           /* 0x0C */
    s32 mode;                   /* 0x10 */
    s32 dlsize;                 /* 0x14 */
    s32 cnt;                    /* 0x18 */
} St;

typedef struct { u8 pad[8]; s16 arr[1]; } Trk_5;
typedef struct { Trk_5 *trk; u8 pad4[0x60]; s16 f64; } Race;

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
} Game_8;

extern St D_800C295C[];
extern St *D_800C29CC_2;
extern Race *D_800CA930_6;
extern Pad D_800E1E40_2[4];
extern Pad D_800E25B0_4[4];
extern Ctx D_800E25C0[2];
extern Ctx *D_800E3080;
extern Pad D_800E30B0_4[4];
extern Game_8 D_800EFD70_18;

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
    D_800C29D8_2[0] = func_800BCCF0(base - sz, sz - 0x10, D_800D6014, 361);
    D_800C29D8_2[1] = (u16 *)((u8 *)D_800C29D8_2[0] + D_800E20E8 * D_800E20EC * 2);
    {
    void *z;
    D_800C29E0 = z = func_800BCCF0(base, D_800E20E8 * D_800E20EC * 2, D_800D6014, 363);
    if (D_800C29D8_2[0] == NULL || z == NULL) {
        func_8000ECB8(1);
    }
    }
    func_8000ED10((u8 *)D_800C29D8_2[0], 0, D_800E20E8 * D_800E20EC * 4);
    func_800BEE78();
    func_8000E980();
    func_8000ED10((u8 *)D_800E30B0_4, 0, sizeof(D_800E30B0_4));
    func_8000CFE8();
    func_8000D064();
    func_8000CD00();
    func_8000CEAC();
    for (i = 0; i < 4; i++) {
        func_8000E31C(&D_800E1E40_2[i], &D_800E30B0_4[i]);
    }
    D_800E203C = func_800BC908(0xC800, 0, D_800D6014, 405);
    D_800E30C4 = func_800BC908(0x401, 0, D_800D6014, 412);
    D_800E3084 = func_800BC908(0xC00, 0, D_800D6014, 413);
    func_800BDF80(D_800C294C);
    func_800C0258(0);
    D_800C29CC_2 = &D_800C295C[0];
    D_800E25C0[0].dl = NULL;
    D_800E25C0[1].dl = NULL;
    func_8005B5E0();
    func_800B05E0_2(&D_800E2048, D_800E20F0, 16);
    func_800B1B40(&D_800E30C8, 5, func_8000E6E8, NULL, D_800E25B0_4, 12);
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
            if (D_800C29CC_2->mode == 0) {
                if (D_800E25C0[0].dl == NULL) {
                    D_800E25C0[0].dl = func_800BC908(D_800C29CC_2->dlsize * 8, 0, D_800D6014, 523);
                    D_800E25C0[1].dl = func_800BC908(D_800C29CC_2->dlsize * 8, 0, D_800D6014, 524);
                }
                i = D_800C29CC_2 - D_800C295C;
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
                func_8000ED10((u8 *)D_800C29D8_2[0], 0, D_800E20E8 * D_800E20EC * 2);
                func_8000ED10((u8 *)D_800C29D8_2[1], 0, D_800E20E8 * D_800E20EC * 2);
                D_800C29CC_2->mode = 2;
                D_800C29E4 = 4;
                if (D_800EFD70_18.flags & 0x8000) {
                    D_800C8CE8 = 1;
                } else {
                    D_800C8CE8 = 0;
                }
                D_800C29CC_2->fn0(0);
                break;
            }
            D_800E2038++;
            if (D_800E1E18 != 0) {
                func_8000CD00();
                func_8000CEAC();
            }
            if ((D_800E1E40_2[0].button & 0xC00) == 0xC00) {
                func_8000E8B0(1);
                do {
                    func_8000D064();
                    func_8000CD00();
                    func_8000CEAC();
                } while ((D_800E1E40_2[0].button & 0x300) != 0x300);
                func_8000E8B0(0);
                do {
                    func_8000D064();
                    func_8000CD00();
                    func_8000CEAC();
                } while ((D_800E1E40_2[0].button & 0x300) == 0x300);
            }
            func_8000D064();
            *(Blk *)D_800E25B0_4 = *(Blk *)D_800E30B0_4;
            for (i = 0; i < 4; i++) {
                func_8000E31C(&D_800E1E40_2[i], &D_800E30B0_4[i]);
            }
            pads = D_800E30B0_4;
            D_800E30C0 = func_800B1330();
            func_80010D44();
            D_800E20E4 = func_800B1330();
            D_800E2134 = D_800E20E4 - D_800E30C0;
            if (D_800E2038 - D_800E2064 > 0 || D_800C29CC_2->mode == 5) {
                if (D_800C29CC_2->mode == 2) {
                    if (D_800C29CC_2->fn4 != NULL) {
                        D_800C29CC_2->fn4(pads);
                    }
                }
                if (D_800C29CC_2->mode == 3) {
                    D_800C29CC_2->mode = 5;
                    D_800C29CC_2->cnt = 10;
                }
                if (D_800C29CC_2->mode == 5 && D_800C29CC_2->cnt == 0) {
                    D_800C29CC_2->mode = 3;
                }
                if (D_800C29CC_2->mode == 3) {
                    D_800C29CC_2->mode = 4;
                    D_800C29CC_2->fnC(0);
                    i = D_800C29CC_2 - D_800C295C;
                    D_800C29CC_2->mode = 0;
                    if (i == 2) {
                        i = 0;
                        if (D_800EFD70_18.flags & 2) {
                            D_800CA920 = 6;
                        } else {
                            D_800EFD64 = &D_800CB210[21];
                            i = 1;
                        }
                    } else if (i == 3) {
                        i = 0;
                    } else if (D_800EFD70_18.flags & 0x40) {
                        i = 3;
                    } else if (i == 0 && (D_800EFD70_18.flags & 0x20)) {
                        i = 2;
                    } else if (D_800CA924[0] > 0 && D_800CA924[1] < D_800CA924[0]) {
                        i = 1;
                    } else {
                        if (D_800EFD70_18.f4C == 1 && D_800CA930_6 != NULL && !(D_800EFD70_18.flags & 0x8000)) {
                            D_800EFD64 = &D_800CB210[D_800CA930_6->trk->arr[D_800CA930_6->f64]];
                        }
                        if (i == 1) {
                            if (D_800EFD70_18.flags & 0x40000) {
                                s32 k;
                                for (k = 0; k < 6; k++) {
                                    D_800EFD70_18.f1C[k] = D_800EFD70_18.f778[k];
                                    D_800EFD70_18.f43C[k] = D_800EFD70_18.f790[k];
                                }
                            }
                        }
                        if (i == 1 && D_800CA920 == 8) {
                            i = 2;
                        } else if (i == 1 && (D_800EFD70_18.flags & 0x808000) == 0x800000) {
                            D_800CCD64 = 1;
                            D_800EFD70_18.f4C = 0;
                            D_800EFD70_18.flags = (D_800EFD70_18.flags | 0x4000000) & 0xFF0BFF7F;
                            D_800CA920 = 6;
                            D_800EFD64 = &D_800CB210[13];
                            D_800EFD70_18.f3C = 1;
                        } else if (!(D_800EFD70_18.flags & 4)) {
                            i ^= 1;
                        }
                    }
                    next = &D_800C295C[i];
                    if (D_800C29CC_2->dlsize != next->dlsize) {
                        func_800BCB04(D_800E25C0[0].dl);
                        func_800BCB04(D_800E25C0[1].dl);
                        D_800E25C0[0].dl = NULL;
                    }
                    D_800C29CC_2 = next;
                }
                D_800E2064 = D_800E2038;
            }
            D_800E30C0 = func_800B1330();
            func_80010D44();
            D_800E20E4 = func_800B1330();
            D_800E2134 = D_800E20E4 - D_800E30C0;
            if (D_800C2958 < 2 && D_800C29CC_2->fn8 != NULL) {
                if (D_800C29CC_2->mode == 5) {
                    if (D_800C2958 == 0) {
                        D_800C29CC_2->cnt--;
                    }
                } else if (D_800C29CC_2->mode == 2) {
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
                    D_800C29CC_2->fn8(D_800E3080, D_800C29E8, &D_800E20E0);
                    D_800CB120 = 0;
                    gDPPipeSync(D_800E20E0++);
                    gDPFullSync(D_800E20E0++);
                    gSPEndDisplayList(D_800E20E0++);
                    D_800E308C = D_800E20E0 - D_800E3080->dl;
                    cur = D_800C29CC_2;
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
                    T->framebuffer = D_800C29D8_2[D_800C29E8];
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
                        s32 c = D_800C29D8_2[D_800C2944][y * D_800F13EC + x];
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

void func_8000E31C(OSContPad *pad, u32 *out)
{
    s32 x;
    s32 y;
    u32 v;
    u32 u;
    u32 t;

    *out = 0;
    x = pad->stick_x;
    x = x * 127 / 80;
    y = pad->stick_y;
    y = y * 127 / 80;
    if (x < -127) {
        x = -127;
    }
    if (x > 127) {
        x = 127;
    }
    if (y < -127) {
        y = -127;
    }
    if (y > 127) {
        y = 127;
    }
    if (x < -5) {
        u = (-x >> 3) << 24;
        if (u != 0) {
            *out = u | 4;
        }
    } else if (x > 5) {
        u = (x >> 3) << 24;
        if (u != 0) {
            *out = u | 8;
        }
    }
    if (y < -5) {
        v = (-y >> 3) << 28;
        if (v == 0) {
            goto done;
        }
        t = *out | 2;
    } else {
        if (y < 6 || (v = (y >> 3) << 28) == 0) {
            goto done;
        }
        t = *out | 1;
    }
    *out = t | v;
done:
    if (pad->button & 0x8000) {
        *out |= 0x10;
    }
    if (pad->button & 0x4000) {
        *out |= 0x20;
    }
    if (pad->button & 0x1000) {
        *out |= 0x40;
    }
    if (pad->button & 8) {
        *out |= 0x80;
    }
    if (pad->button & 4) {
        *out |= 0x100;
    }
    if (pad->button & 2) {
        *out |= 0x200;
    }
    if (pad->button & 1) {
        *out |= 0x400;
    }
    if (pad->button & 0x800) {
        *out |= 0x800;
    }
    if (pad->button & 0x400) {
        *out |= 0x1000;
    }
    if (pad->button & 0x200) {
        *out |= 0x2000;
    }
    if (pad->button & 0x100) {
        *out |= 0x4000;
    }
    if (pad->button & 0x20) {
        *out |= 0x8000;
    }
    if (pad->button & 0x10) {
        *out |= 0x10000;
    }
    if (pad->button & 0x2000) {
        *out |= 0x20000;
    }
}

void func_8000E5C0(void)
{
    u32 top;
    u32 s0;
    u32 s2;
    u32 s3;
    u32 s1;
    u32 t;

    top = ((u32)D_80000318 | 0x80000000) - 0x100;
    s0 = (u32)D_800731D0;
    s2 = (u32)D_800A42C0;
    s3 = (u32)D_800A9C20;
    if (s0 & 0xF) {
        func_8000ECB8(0x33);
    }
    if (s2 & 0xF) {
        func_8000ECB8(0x34);
    }
    if (s3 & 0xF) {
        func_8000ECB8(0x35);
    }
    s1 = (u32)D_80105680;
    s1 = (s1 + 0xFF) & ~0xFF;
    func_800B3020(1);
    func_8000ED10((u8 *)s1, 0, top - s1);
    func_800BD698(0, 0, s0, top - s0);
    t = 0x30;
    func_800BCCF0(s0 + 0x30, s2 - t - s0, (u32)D_800D6014, 0x4FA);
    t = 0x10;
    func_800BCCF0(s2 + 0x10, s3 - t - s2, (u32)D_800D6014, 0x4FB);
    func_800BCCF0(s3 + 0x10, s1 - t - s3, (u32)D_800D6014, 0x4FC);
}

typedef struct {
    u8 pad[0x4C8];
    s16 msg[16];      /* 0x4C8 */
    u8 f4E8[0x68];    /* 0x4E8 */
    u32 a;            /* 0x550 */
    u32 b;            /* 0x554 */
    u32 c;            /* 0x558 */
    u32 d;            /* 0x55C */
} Task;

extern Task D_800E25C0_2[2];

void func_8000E6E8(void)
{
    s16 *msg;
    u32 idx;
    u32 t;
    u32 v;

    for (;;) {
        func_800B0610(&D_800E2048, (OSMesg *)&msg, 1);
        switch (*msg) {
        case 1:
            D_800C29F0++;
            idx = func_80065858() != (s32)D_800E25C0_2[0].f4E8;
            if (idx != D_800C29F8) {
                D_800E25C0_2[idx].a = func_800B1330();
            }
            D_800C29F8 = idx;
            if ((D_800C29FC ^ D_800C2350) & 0x1000) {
                D_800C29FC = D_800C2350 & 0x1000;
                func_800B2B00(D_800C29FC ? 0x6A : 0xAA);
            }
            if (MQ_GET_COUNT(&D_800E3090) < 14) {
                func_800B1150(&D_800E3090, msg, 0);
            }
            break;
        case 2:
            idx = msg != D_800E25C0_2[0].msg;
            D_800C29F4++;
            if (D_800CEC04 != 0) {
                func_800A11BC(idx);
            } else {
                func_800325FC(idx);
                func_80033E30(idx);
            }
            t = func_800B1330();
            D_800E25C0_2[idx].b = t;
            D_800E25C0_2[idx].c = t - D_800E25C0_2[idx].a;
            func_800B1150(&D_800E3090, msg, 1);
            break;
        }
    }
}

void func_8000E8B0(s32 param_1)
{
    u8 *p = D_800E2138;
    s32 flag = 0;
    if (param_1 != 0) {
        flag = -1;
    }
    func_800ACD70(p, 0xFF70000, flag);
    D_800C2930 = param_1;
}

typedef struct {
    void *next;
    u8 f4, f5, f6, f7, f8, f9;
    u8 pad[2];
    u32 fc;
} Obj_4;

extern Obj_4 D_800E2138_2;

void func_8000E8F4(void)
{
    s32 r;

    func_8000ED10((u8 *)&D_800E2138_2, 0, 0x74);
    D_800E2138_2.fc = 0xAFF70000;
    D_800E2138_2.f5 = 5;
    D_800E2138_2.f8 = 0xC;
    D_800E2138_2.f6 = 0xD;
    D_800E2138_2.f7 = 2;
    D_800E2138_2.f4 = 0;
    D_800E2138_2.f9 = 1;
    r = func_800AC890();
    D_800E2138_2.next = D_800D1E0C;
    D_800D1E0C = &D_800E2138_2;
    func_800AC900(r);
}

void func_8000E980(void)
{
    func_800B05E0_3(D_800E3090_2, D_800E2068, 0x10);
    func_800651A0(0xD, 1);
}

void func_8000E9B8(void)
{
    while (1) {}
}

void func_8000E9C0(void)
{
    func_800B2550(10);
    func_800B2AB0(D_800D1FF0);
    func_800B3020(0);
    func_800B2CC0_2((void *)0x80200000);
    func_800B3020(0);
    func_800B2CC0_2((void *)0x80200000);
    func_8000ED10((u8 *)0x80200000, 0xFF, 0x25800);
    func_800AD990();
    __asm__ volatile("break 0x407");
    while (1) {}
}

void func_8000EA24(void)
{
}

void func_8000EA2C(void) {
    func_8000EFB0();
    func_8000EFB0(&D_800D6010);
}

void func_8000EA54(void)
{
}

s32 func_8000EA5C(void)
{
    __asm__ volatile("addu $9, $30, $0" : : : "$9");
    while ((*(volatile u32 *)0xA4600010 & 3) != 0) {
    }
    if ((*(volatile u32 *)0xB0000574 & 0xFFFF) != 0x6C07) {
        return 0;
    }
    while ((*(volatile u32 *)0xA4600010 & 3) != 0) {
    }
    return (*(volatile u32 *)0xB0000578 & 0xFFFF) == 0x8965;
}
