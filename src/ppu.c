#include "ppu.h"

#include <libdragon.h>
#include "bus.h"

ppu_t ppu;

#define PALETTE_BANK_BACKGROUND 0

// from libdragon's display.c
extern uintptr_t __safe_buffer[3];

INLINE bool is_vblank() {
    return ppu.y > GBA_SCREEN_Y && ppu.y != 227;
}

typedef union gba_color {
    struct {
#ifdef GBA_LITTLE_ENDIAN
        unsigned r:5;
        unsigned g:5;
        unsigned b:5;
        bool transparent:1;
#else
        unsigned:1;
        unsigned b:5;
        unsigned g:5;
        unsigned r:5;
#endif
    } PACKED;
    half raw;
} gba_color_t;

typedef union n64_color {
    struct {
#ifdef GBA_LITTLE_ENDIAN
        unsigned a:1;
        unsigned b:5;
        unsigned g:5;
        unsigned r:5;
#else
        unsigned r:5;
        unsigned g:5;
        unsigned b:5;
        unsigned a:1;
#endif
    } PACKED;
    half raw;
} n64_color_t;

half gba_to_n64_color(half gba) {
    gba_color_t gba_color;
    gba_color.raw = gba;

    n64_color_t n64_color;
    n64_color.r = gba_color.r;
    n64_color.g = gba_color.g;
    n64_color.b = gba_color.b;

    return n64_color.raw;
}

INLINE half* get_buffer_ptr(display_context_t context) {
    return (half*)__safe_buffer[context - 1];
}

void render_line_mode4() {
    half* buf = get_buffer_ptr(ppu.display);
    if (buf == NULL) {
        while(true) {
            buf++;
        }
    }
    half transparent = gba_to_n64_color(half_from_byte_array(ppu.pram, 0));

    if (ppu.DISPCNT.screen_display_bg2) {
        for (int x = 0; x < GBA_SCREEN_X; x++) {
            int offset = x + (ppu.y * GBA_SCREEN_X);
            int index = ppu.DISPCNT.display_frame_select * 0xA000 + offset;
            int tile = ppu.vram[index];
            if (tile == 0) {
                buf[ppu.y * 320 + x] = transparent;
            } else {
                buf[ppu.y * 320 + x] = gba_to_n64_color(half_from_byte_array(ppu.pram, (0x20 * PALETTE_BANK_BACKGROUND + 2 * tile)) & 0x7FFF);
            }
        }
    } else {
        for (int x = 0; x < GBA_SCREEN_X; x++) {
            buf[ppu.y * 320 + x] = transparent;
        }
    }
}

void render_line() {
    switch (ppu.DISPCNT.mode) {
        case 0: // probably still booting
            break;
        case 4:
            render_line_mode4();
        default:
            break;
            // shrug
    }
    // NOOP
}

void ppu_hblank() {
    /*
    if (!is_vblank()) {
        dma_start_trigger(HBlank);
    }
    if (ppu->DISPSTAT.hblank_irq_enable) {
        request_interrupt(IRQ_HBLANK);
    }
     */
    ppu.DISPSTAT.hblank = true;
    if (ppu.y < GBA_SCREEN_Y && !ppu.DISPCNT.forced_blank) { // i.e. not VBlank
        render_line();
    }
}

void ppu_vblank() {
    /*
    dma_start_trigger(VBlank);
    if (ppu->DISPSTAT.vblank_irq_enable) {
        request_interrupt(IRQ_VBLANK);
    }
     */
    ppu.DISPSTAT.vblank = true;
    //render_screen(&ppu->screen);
}

void check_vcount() {
    if (ppu.y == ppu.DISPSTAT.vcount_setting) {
        ppu.DISPSTAT.vcount = true;
        /*
        if (ppu.DISPSTAT.vcount_irq_enable) {
            request_interrupt(IRQ_VCOUNT);
        }
         */
    } else {
        ppu.DISPSTAT.vcount = false;
    }
}

void ppu_end_hblank() {
    /*
    ppu->BG2X.current.sraw += ppu->BG2PB.sraw;
    ppu->BG3X.current.sraw += ppu->BG3PB.sraw;
    ppu->BG2Y.current.sraw += ppu->BG2PD.sraw;
    ppu->BG3Y.current.sraw += ppu->BG3PD.sraw;
     */

    ppu.DISPSTAT.hblank = false;
    ppu.y++;

    check_vcount(ppu);
}

void ppu_end_vblank() {
    /*
    ppu->BG2X.current.raw = ppu->BG2X.initial.raw;
    ppu->BG3X.current.raw = ppu->BG3X.initial.raw;
    ppu->BG2Y.current.raw = ppu->BG2Y.initial.raw;
    ppu->BG3Y.current.raw = ppu->BG3Y.initial.raw;
     */

    ppu.y = 0;
    check_vcount(ppu);
    ppu.DISPSTAT.vblank = false;

    display_show(ppu.display);
    handle_input();
    ACQUIRE_DISPLAY();
}
