/* What every source file of the boot loader includes. */
#ifndef BOOT_H
#define BOOT_H

#include "common.h"

/* src/boot/boot_80000450.c */
void boot_main(void);
void boot_idle(s32 param_1);

/* src/boot/boot_80000824.c */
void boot_int_to_str(s32 n, char *s);

/* src/boot/boot_8000095C.c */
void boot_log_write(u8 *param_1, u32 param_2);
void boot_log_print(u8 *p);

/* src/boot/boot_80000B30.c */
void boot_dma_update(void);
s32 boot_dma_init(void);

/* src/boot/boot_80000E40.c */
void boot_draw_char(s32 c, s32 x, s32 y, u16 *fb, s32 color);
void boot_draw_string(u8 *s, s32 x, s32 y, u16 *fb, s32 color);
void boot_func_80000FC8(s32 x0, s32 x1, s32 y0);
void boot_thread_main(void);
void boot_relocate_stub(u32 *from, u32 size, u32 *dst);
s32 boot_init_controllers(void);
void boot_init_prenmi_queue(void);
void boot_check_reset(void);

/* src/boot/boot_80001AC4.c */
void boot_fade_image(u16 *dst, u16 *src, s32 k);

/* src/boot/boot_80001B50.c */
u32 sn_disable_ints(void);
void sn_restore_ints(u32 a0);
u32 sn_restore_ints_2(u32);
void sn_pi_write_word(s32 *p, s32 v);
u32 sn_pi_read_word(u32 *param_1);
u32 sn_pi_read_byte(u32 addr);
u32 boot_func_80001CB4(u32 a);
void sn_pi_read_bytes(u32 src, u8 *dst, u32 n);
void sn_pi_read_bytes_wordwise(u32 src, u32 dst, u32 cnt);
void sn_pi_write_byte(u32 param_1, u8 param_2);
void boot_func_80001F98(u32 addr, u8 val);
void sn_pi_fill(u8 *p0, u32 c0, u32 n0);
void sn_pi_copy_bytes(u8 *dst, u8 *src, s32 n);
void boot_func_800023EC(void);
s32 sn_strlen(char *s);
void sn_host_call(s32 p1, s32 p2, s32 p3, s32 p4);
u32 sn_host_call_2(u32, u32, u32, u32);
s32 sn_host_call_3(s32 arg0, s32 arg1, s32 arg2, s32 arg3);
u32 sn_host_call_4(s32 a, s32 b, u32 c, s32 d);
s32 sn_host_call_5(s32 a0, s32 a1, u32 a2, s32 a3);
s32 sn_pc_open(u8 *arg0, s32 arg1);
s32 sn_pc_creat(u8 *arg0, s32 arg1);
s32 sn_pc_read(s32 fd, u8 *buf, u32 size);
s32 sn_pc_write(s32 fd, u8 *buf, u32 size);
u32 sn_pc_lseek(u32 p1, u32 p2, u32 p3);

/* src/boot/boot_80001C2C.c */
void sn_delay(s32 param_1);
void sn_write_byte_wordwise(u32 addr, u32 val);
void sn_pi_write_bytes(u8 *d, u8 *s, u32 n);
void sn_host_mem_access(u32 p);
void sn_debug_init(void);
void sn_debug_command_loop(void);
u32 sn_pc_init(void);
u32 sn_pc_close(u32 p);

/* the boot loader's copies of game functions (built from the game's C) */
void boot_debug_printf(s32 a, ...);
u16 boot_isqrt_u32(u32 r);
s32 boot_char_to_upper(s32 a0);
s32 boot_char_to_lower(s32 a0);
void boot_mem_fill(u8 *a0, u8 a1, s32 a2);
void boot_str_cat(char *dst, char *src);
void boot_str_copy_n(u8 *d, u8 *s, s32 n);
s32 boot_str_icmp(u8 *param_1, u8 *param_2);
void boot_func_80000814(void);
void boot_func_8000081C(void);
void boot_str_upper(u8 *s);
void boot_str_copy(char *a0, char *a1);
s32 boot_dma_start(s32 a0, s32 a1, s32 a2, s32 a3, s32 a4);
s32 boot_dma_get_status(s32 a0);
void boot_dma_release(s32 a0);
void boot_dma_wait(s32 x);

/* libultra functions its headers don't declare, and other views of libultra functions */
void __osDisableInt(void);
u32 strlen(char *s);
u32 osViSwapBuffer_2(u32 a);
void __osMotorAccess();
void __osInitialize_autodetect(void);

/* other code */
void sn_install_exception_hooks(void);
u32 sn_get_status_reg(void);
void sn_set_status_reg(u32 v);
void boot_swd_expand(void);

extern s32 sBootLogFd;
extern void (*sBootDmaCopyFunc)(s32, s32, s32);
extern u8 sBootFontBitmap[];
extern u8 sBootSplashSwd[];
extern u8 D_8000F498[];
extern u32 sSnMemRegions[];
extern u32 D_8000F4A4[];
extern u32 D_8000F4A8;
extern u32 D_8000F4B8;
extern OSViMode sBootViModeNtsc;
extern OSViMode sBootViModeMpal;
extern char sBootLogFileName[];
extern u32 gSwdLoadCur;
extern u32 gSwdLoadEnd;
extern u32 gSwdSaveCur;
extern u32 gSwdSaveEnd;
extern u8 gBootStubStart[];
extern u8 gBootStubSrc[];
extern u8 gBootStubSize[];
extern u8 gBootStubDst[];
extern u8 gBootStubEntry[];
extern u8 gBootStubEnd[];
extern OSThread sBootIdleThread;
extern OSThread sBootMainThread;
extern s32 sBootMainThreadStackEnd;
extern s32 sBootDmaPendingCount;
extern OSMesgQueue sBootPiCmdQueue;
extern s32 D_80017984[];
extern OSMesg sBootPiCmdMsgs[];
extern s32 D_80017C00;
extern s32 sBootCartHandle;
extern OSContPad sBootContPads[];
extern OSMesgQueue sBootViQueue;
extern OSPfs sBootGbPfs;
extern u32 sBootFramebuffer;
extern u32 sBootGameSize;
extern OSContStatus sBootContStatus[];
extern OSPfs sBootPfs[];
extern OSMesgQueue sBootPreNmiQueue;
extern OSMesgQueue sBootSiQueue;
extern u32 sBootGameDst;
extern u8 sBootContPattern[];
extern OSMesg sBootViMsgs[];
extern void (*sBootStubFunc)(void);
extern OSPfs sBootMotorPfs[];
extern OSMesg sBootPreNmiMsgs[];
extern OSMesg sBootSiMsgs[];
extern u32 sBootStubDelta;
extern u32 sBootGameSrc;
extern u32 sBootUnpackBuf;
extern volatile s32 sSnDelayCounter;
extern u32 D_80018288;
extern u8 D_80018430;
extern u8 D_80018432;
extern u8 D_80100000[];
extern u8 D_80100030[];
extern u8 D_8F018110[];
extern u8 gBootGameContainerRom[];

#endif
