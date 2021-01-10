#include "gba.h"
#include <libdragon.h>
#include <stdio.h>
#include <string.h>

#include "bus.h"

gbamem_t mem;
gbabus_t bus;
arm7tdmi_t* cpu;
bool should_quit = false;

#define VISIBLE_CYCLES 960
#define HBLANK_CYCLES 272
#define VISIBLE_LINES 160
#define VBLANK_LINES 68

INLINE int inline_gba_cpu_step() {
    cpu->irq = (bus.interrupt_enable.raw & bus.IF.raw) != 0;
    if (cpu->halt && !cpu->irq) {
        return 1;
    } else {
        return arm7tdmi_step(cpu);
    }
}

INLINE int run_system(int for_cycles) {
    while (for_cycles > 0) {
        int ran = inline_gba_cpu_step();
        /*
        timer_tick(ran);
        for (int i = 0; i < ran; i++) {
            apu_tick(apu);
        }
         */
        for_cycles -= ran;
    }
    return for_cycles;
}

void gba_system_loop() {
    int extra = 0;
    while (!should_quit) {
        for (int line = 0; line < VISIBLE_LINES; line++) {
            extra = run_system(VISIBLE_CYCLES + extra);
            //ppu_hblank(ppu);
            extra = run_system(HBLANK_CYCLES + extra);
            //ppu_end_hblank(ppu);
        }
        //ppu_vblank(ppu);
        for (int line = 0; line < VBLANK_LINES; line++) {
            extra = run_system(VISIBLE_CYCLES + extra);
            //ppu_hblank(ppu);
            extra = run_system(HBLANK_CYCLES + extra);
            //ppu_end_hblank(ppu);
        }
        //ppu_end_vblank(ppu);
    }
}

void gba_init(const char* rom) {
    memset(&mem, 0, sizeof(mem));
    memset(&bus, 0, sizeof(bus));

    mem.rom = dfs_open(rom);
    mem.rom_size = dfs_size(mem.rom);

    // Initialize the CPU, hook it up to the GBA bus
    cpu = init_arm7tdmi(gba_read_byte,
                        gba_read_half,
                        gba_read_word,
                        gba_write_byte,
                        gba_write_half,
                        gba_write_word);
    skip_bios(cpu);
}
