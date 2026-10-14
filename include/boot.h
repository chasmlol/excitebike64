/* What every source file of the boot loader includes. */
#ifndef BOOT_H
#define BOOT_H

#include "common.h"

/* Memory the boot loader uses (docs/overview.md, "Startup"). */
#define BOOT_FRAMEBUFFER        0x80200000  /* 320x240, 16-bit pixels */
#define BOOT_CONTAINER_BUF      0x80100000  /* where the BRSOHYES container is DMAed */
#define BOOT_CONTAINER_SIZE     0xAF000     /* the bytes DMAed */
#define BOOT_STUB_ADDR          0x802F0000  /* where the copy stub (boot_game_copy_stub) runs */
#define BOOT_UNPACK_BUF         0x80300000  /* the splash, then the game image, unpacked */
#define BOOT_HEADER_ADDR        0x803FFFC0  /* the container header, left for the game (gBootHeader) */
#define BOOT_GAME_ENTRY         0x80000400
/* The stub copies the game image over the boot loader except for its first 8 bytes: the boot loader's entry stays,
   whose 0xBEEF at 0x80000402 tells game_main it was started by the boot loader. */
#define BOOT_GAME_COPY_SRC      0x80300008
#define BOOT_GAME_COPY_DST      0x80000408

#define BOOT_SCREEN_WIDTH       320
#define BOOT_SCREEN_HEIGHT      240
#define BOOT_SCREEN_PIXELS      0x12C00
#define BOOT_FB_SIZE            0x25800     /* bytes of a framebuffer */

/* The splash screen: sBootSplashSwd unpacks to a header, 320x240 bytes of pixels stored bottom up, then their
   palette of 16-bit colours. */
#define BOOT_SPLASH_SWD_SIZE    0x1688
#define BOOT_SPLASH_HEADER_SIZE 0x28
#define BOOT_SPLASH_FRAMES      120         /* how long it shows */

/* The BRSOHYES container (gBootGameContainerRom, ROM 0x12200): the 8-byte tag, then {unpacked size, packed size}
   of each of the game image's segments, then the segments, SWD-compressed, and the game's resources. */
#define BOOT_CONTAINER_HEADER_SIZE  0x28
#define BOOT_SEGMENT_COUNT      4

/* DMA request slots (sBootDmaRequests), the same as the game's (DMA_* in structs/system.h). */
#define BOOT_DMA_SLOT_COUNT     8
#define BOOT_DMA_SLOT_WORDS     19
#define BOOT_DMA_FREE           0
#define BOOT_DMA_RUNNING        1
#define BOOT_DMA_QUEUED         2
#define BOOT_DMA_DONE           3

/* MIPS instruction fields boot_relocate_stub patches. */
#define MIPS_OP_MASK            0xFC000000
#define MIPS_OP_J               0x08000000
#define MIPS_OP_JAL             0x0C000000
#define MIPS_OP_ORI             0x34000000
#define MIPS_OP_LUI             0x3C000000
#define MIPS_OP_LW              0x8C000000
#define MIPS_OP_SW              0xAC000000
#define MIPS_LUI_MASK           0xFFE00000  /* the opcode and rs, which is 0 in a lui */
#define MIPS_TARGET_MASK        0x3FFFFFF   /* a j or jal's target >> 2 */

/* A PI register, through KSEG1. */
#define PI_REG(reg) (*(volatile u32 *)PHYS_TO_K1(reg))

/* SN Systems' development board (the sn_* debug runtime), at the top of PI domain 1. */
#define SN_DATA_PORT            0xB1FF0000  /* the data of PCopen, PCread, PCwrite... */
#define SN_DATA_PORT_SIZE       0x8000      /* the most a PCread or PCwrite moves at a time */
#define SN_MEM_PORT             0xB1FE0000  /* the data of the host's memory reads and writes */
#define SN_REG_RESULT           0xB1FFFFDC  /* a host call's result */
#define SN_REG_COMMAND          0xB1FFFFE0  /* a host call's command (SN_PC_*); the host clears it when done */
#define SN_REG_ARG1             0xB1FFFFE4
#define SN_REG_ARG2             0xB1FFFFE8
#define SN_REG_ARG3             0xB1FFFFEC
#define SN_REG_HOST_CMD         0xB1FFFFF0  /* a command from the host (sn_debug_command_loop) */
#define SN_REG_HOST_ADDR        0xB1FFFFF4  /* its address */
#define SN_REG_HOST_SIZE        0xB1FFFFF8  /* its size */
#define SN_REG_ACK              0xB1FFFFFC  /* the console's answer to it */
/* Host calls: the PC file functions. */
#define SN_PC_INIT              0x101
#define SN_PC_OPEN              0x202
#define SN_PC_CREAT             0x303
#define SN_PC_CLOSE             0x404
#define SN_PC_READ              0x505
#define SN_PC_WRITE             0x606
#define SN_PC_LSEEK             0x707
/* sSnMemRegions: {last address, flags} pairs, by address, that say what the host may access. */
#define SN_MEM_READ             1
#define SN_MEM_WRITE            2
#define SN_MEM_PI               4           /* PI space: copied with PI reads and writes */
#define SN_MEM_WORDS            8           /* written a whole word at a time (RSP memory) */

/* src/boot/boot_80000450.c */
void boot_main(void);
void boot_idle(s32 arg);

/* src/boot/boot_80000824.c */
void boot_int_to_str(s32 n, char *s);

/* src/boot/boot_8000095C.c */
void boot_log_write(u8 *buf, u32 size);
void boot_log_print(u8 *str);

/* src/boot/boot_80000B30.c */
void boot_dma_update(void);
s32 boot_dma_init(void);

/* src/boot/boot_80000E40.c */
void boot_draw_char(s32 c, s32 x, s32 y, u16 *fb, s32 color);
void boot_draw_string(u8 *s, s32 x, s32 y, u16 *fb, s32 color);
void boot_draw_progress_bar(s32 left, s32 right, s32 top);
void boot_thread_main(void);
void boot_relocate_stub(u32 *from, u32 size, u32 *dst);
s32 boot_init_controllers(void);
void boot_init_prenmi_queue(void);
void boot_check_reset(void);

/* src/boot/boot_80001AC4.c */
void boot_fade_image(u16 *dst, u16 *src, s32 level);

/* src/boot/boot_80001B50.c */
u32 sn_disable_ints(void);
void sn_restore_ints(u32 ie);
u32 sn_restore_ints_2(u32);
void sn_pi_write_word(s32 *addr, s32 value);
u32 sn_pi_read_word(u32 *addr);
u32 sn_pi_read_byte(u32 addr);
u32 sn_pi_read_byte_alt(u32 addr);
void sn_pi_read_bytes(u32 src, u8 *dst, u32 n);
void sn_pi_read_bytes_wordwise(u32 src, u32 dst, u32 count);
void sn_pi_write_byte(u32 addr, u8 value);
void sn_pi_write_byte_alt(u32 addr, u8 value);
void sn_pi_fill(u8 *dst, u32 value, u32 count);
void sn_pi_copy_bytes(u8 *dst, u8 *src, s32 n);
void sn_host_copy_rom(void);
s32 sn_strlen(char *s);
void sn_host_call(s32 cmd, s32 arg1, s32 arg2, s32 arg3);
u32 sn_host_call_2(u32, u32, u32, u32);
s32 sn_host_call_3(s32 cmd, s32 arg1, s32 arg2, s32 arg3);
u32 sn_host_call_4(s32 cmd, s32 arg1, u32 arg2, s32 arg3);
s32 sn_host_call_5(s32 cmd, s32 arg1, u32 arg2, s32 arg3);
s32 sn_pc_open(u8 *name, s32 mode);
s32 sn_pc_creat(u8 *name, s32 mode);
s32 sn_pc_read(s32 fd, u8 *buf, u32 size);
s32 sn_pc_write(s32 fd, u8 *buf, u32 size);
u32 sn_pc_lseek(u32 fd, u32 offset, u32 whence);

/* src/boot/boot_80001C2C.c */
void sn_delay(s32 count);
void sn_write_byte_wordwise(u32 addr, u32 val);
void sn_pi_write_bytes(u8 *dst, u8 *src, u32 n);
void sn_host_mem_access(u32 mode);
void sn_debug_init(void);
void sn_debug_command_loop(void);
u32 sn_pc_init(void);
u32 sn_pc_close(u32 fd);

/* the boot loader's copies of game functions (built from the game's C) */
void boot_debug_printf(s32 fmt, ...);
u16 boot_isqrt_u32(u32 x);
s32 boot_char_to_upper(s32 c);
s32 boot_char_to_lower(s32 c);
void boot_mem_fill(u8 *dst, u8 value, s32 len);
void boot_str_cat(char *dst, char *src);
void boot_str_copy_n(u8 *dst, u8 *src, s32 size);
s32 boot_str_icmp(u8 *a, u8 *b);
void boot_func_80000814(void);
void boot_func_8000081C(void);
void boot_str_upper(u8 *s);
void boot_str_copy(char *dst, char *src);
s32 boot_dma_start(s32 handle, s32 direction, s32 devAddr, s32 dramAddr, s32 size);
s32 boot_dma_get_status(s32 slot);
void boot_dma_release(s32 slot);
void boot_dma_wait(s32 slot);

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
#if VERSION == VERSION_EU
extern OSViMode sBootViModePal;
#else
extern OSViMode sBootViModeNtsc;
extern OSViMode sBootViModeMpal;
#endif
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
extern s32 sBootDmaStartedCount;
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
