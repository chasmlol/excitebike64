#include "boot.h"

void boot_main(void)
{
    __osInitialize_common();
    __osInitialize_autodetect();
    osCreateThread(&sBootIdleThread, 1, boot_idle, NULL, &sBootMainThread, 8);
    osStartThread(&sBootIdleThread);
    for (;;) {
    }
}

void boot_idle(s32 param_1) {
    osCreateThread(&sBootMainThread, 3, (void *)boot_thread_main, param_1, &sBootMainThreadStackEnd, 10);
    osStartThread(&sBootMainThread);
    osSetThreadPri((void *)0, 0);
    while (1) {
    }
}
