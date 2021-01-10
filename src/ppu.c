#include "ppu.h"

ppu_t ppu;

INLINE bool is_vblank() {
    return ppu.y > GBA_SCREEN_Y && ppu.y != 227;
}

void render_line() {
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
}
