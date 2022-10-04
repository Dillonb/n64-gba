#include <stdio.h>
#include <malloc.h>

#include <libdragon.h>

#include "gba.h"

static resolution_t res = RESOLUTION_320x240;
static bitdepth_t bit = DEPTH_16_BPP;

const char* rom_menu() {
    // TODO actually display a menu
    return "armwrestler.gba";
}

int main(void) {
    debug_init_isviewer();

    const char* rom = rom_menu();
    debugf("Loading %s\n", rom);

    // Initialize peripherals
    controller_init();
    display_init( res, bit, 2, GAMMA_NONE, ANTIALIAS_RESAMPLE );

    // Load rom
    gba_init(rom);

    // Loop
    gba_system_loop();

    return 0;
}
