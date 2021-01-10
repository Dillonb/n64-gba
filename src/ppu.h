#ifndef N64_GBA_PPU_H
#define N64_GBA_PPU_H

#include "util.h"

#include <libdragon.h>

#define PRAM_SIZE  0x400
#define VRAM_SIZE  0x18000
#define OAM_SIZE   0x400

#define GBA_SCREEN_X 240
#define GBA_SCREEN_HBLANK 68
#define GBA_SCREEN_Y 160
#define GBA_SCREEN_VBLANK 68

typedef union DISPCNT {
    struct {
#ifdef GBA_LITTLE_ENDIAN
        unsigned mode:3;
        bool cgbmode:1;
        bool display_frame_select:1;
        bool hblank_interval_free:1;
        bool obj_character_vram_mapping:1;
        bool forced_blank:1;
        bool screen_display_bg0:1;
        bool screen_display_bg1:1;
        bool screen_display_bg2:1;
        bool screen_display_bg3:1;
        bool screen_display_obj:1;
        bool window0_display:1;
        bool window1_display:1;
        bool obj_window_display:1;
#else
        bool obj_window_display:1;
        bool window1_display:1;
        bool window0_display:1;
        bool screen_display_obj:1;
        bool screen_display_bg3:1;
        bool screen_display_bg2:1;
        bool screen_display_bg1:1;
        bool screen_display_bg0:1;
        bool forced_blank:1;
        bool obj_character_vram_mapping:1;
        bool hblank_interval_free:1;
        bool display_frame_select:1;
        bool cgbmode:1;
        unsigned mode:3;
#endif
    } PACKED;
    half raw;
} DISPCNT_t;
ASSERTHALF(DISPCNT_t);

typedef union DISPSTAT {
    struct {
#ifdef GBA_LITTLE_ENDIAN
        // Read only
        bool vblank:1;
        bool hblank:1;
        bool vcount:1;

        // R/W
        bool vblank_irq_enable:1;
        bool hblank_irq_enable:1;
        bool vcount_irq_enable:1;
        unsigned:1; // Unused: DSi LCD initialization ready (RO)
        unsigned:1; // Unused: NDS: MSB of v-vcount setting
        unsigned vcount_setting:8;
#else
        // R/W
        unsigned vcount_setting:8;
        unsigned:1; // Unused: NDS: MSB of v-vcount setting
        unsigned:1; // Unused: DSi LCD initialization ready (RO)
        bool vcount_irq_enable:1;
        bool hblank_irq_enable:1;
        bool vblank_irq_enable:1;

        // Read only
        bool vcount:1;
        bool hblank:1;
        bool vblank:1;
#endif
    } PACKED;
    half raw;
} DISPSTAT_t;
ASSERTHALF(DISPSTAT_t);

typedef struct ppu {
    half y;
    DISPCNT_t DISPCNT;
    DISPSTAT_t DISPSTAT;

    display_context_t display;

    byte pram[PRAM_SIZE];
    byte vram[VRAM_SIZE];
    byte oam[OAM_SIZE];
} ppu_t;

extern ppu_t ppu;

#define ACQUIRE_DISPLAY() do { while (!(ppu.display = display_lock())) {} } while(0)

void ppu_hblank();
void ppu_vblank();
void ppu_end_hblank();
void ppu_end_vblank();

#endif //N64_GBA_PPU_H
