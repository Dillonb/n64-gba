#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <stdint.h>

#include <libdragon.h>

#include "gba.h"

static resolution_t res = RESOLUTION_320x240;
static bitdepth_t bit = DEPTH_16_BPP;

typedef uint64_t dword;
typedef uint32_t word;
typedef uint16_t half;
typedef uint8_t  byte;

const char* rom_menu() {
    // TODO actually display a menu
    return "armwrestler.gba";
}

int main(void) {
    /* enable interrupts (on the CPU) */
    init_interrupts();

    /* Initialize peripherals */
    display_init( res, bit, 2, GAMMA_NONE, ANTIALIAS_RESAMPLE );
    controller_init();
    console_init();
    console_set_render_mode(RENDER_MANUAL);

    const char* rom = rom_menu();
    printf("Loading %s\n", rom);
    console_render();

    gba_init(rom);
    console_close();

    // re-init display so we're sure the resolution is set correctly
    display_init( res, bit, 2, GAMMA_NONE, ANTIALIAS_RESAMPLE );
    gba_system_loop();

    return 0;
}
