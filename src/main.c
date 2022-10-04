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

void exception_handler(exception_t* ex) {
    switch (ex->type) {
        case EXCEPTION_CODE_TLB_LOAD_I_MISS:
        case EXCEPTION_CODE_TLB_MODIFICATION:
        case EXCEPTION_CODE_TLB_STORE_MISS:
            gba_load_rom_page(C0_BADVADDR());
            break;
        default:
            exception_default_handler(ex);
    }
}

void require_expansion_pack() {
    if (!is_memory_expanded()) {
        console_init();
        console_set_render_mode(RENDER_MANUAL);
        printf("This ROM requires the expansion pack!\n");

        while (true) {
            console_render();
        }
    }
}

int main(void) {
    debug_init_isviewer();
    register_exception_handler(exception_handler);

    require_expansion_pack();

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
