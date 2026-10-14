#include "boot.h"

/* The boot loader's main, called by boot_entry: initializes libultra and starts the idle thread (id 1, priority 8;
   its stack ends where sBootMainThread starts). */
void boot_main(void)
{
    __osInitialize_common();
    __osInitialize_autodetect();
    osCreateThread(&sBootIdleThread, 1, boot_idle, NULL, &sBootMainThread, 8);
    osStartThread(&sBootIdleThread);
    for (;;) {
    }
}

/* The idle thread: starts boot_thread_main (id 3, priority 10) and drops to the idle priority. */
void boot_idle(s32 arg)
{
    osCreateThread(&sBootMainThread, 3, (void *)boot_thread_main, arg, &sBootMainThreadStackEnd, 10);
    osStartThread(&sBootMainThread);
    osSetThreadPri((void *)0, OS_PRIORITY_IDLE);
    while (1) {
    }
}
