#ifndef N64_GBA_GBA_H
#define N64_GBA_GBA_H

#include "util.h"
#include "arm7tdmi/arm7tdmi.h"

#define EWRAM_SIZE 0x40000
#define IWRAM_SIZE 0x8000


typedef struct gbamem {
    int rom; // file handle
    int rom_size;
    u8 ewram[EWRAM_SIZE];
    u8 iwram[IWRAM_SIZE];
} gbamem_t;

typedef union interrupt_master_enable {
    struct {
#ifdef GBA_LITTLE_ENDIAN
        bool enable:1;
        unsigned:15;
#else
        unsigned:15;
        bool enable:1;
#endif
    };
    half raw;
} interrupt_master_enable_t;

typedef union interrupt_enable {
    struct {
#ifdef GBA_LITTLE_ENDIAN
        bool lcd_vblank:1;
        bool lcd_hblank:1;
        bool lcd_vcounter_match:1;
        bool timer0_overflow:1;
        bool timer1_overflow:1;
        bool timer2_overflow:1;
        bool timer3_overflow:1;
        bool serial_communication:1;
        bool dma_0:1;
        bool dma_1:1;
        bool dma_2:1;
        bool dma_3:1;
        bool keypad:1;
        bool gamepak:1;
        unsigned:2; // Unused
#else
        unsigned:2; // Unused
        bool gamepak:1;
        bool keypad:1;
        bool dma_3:1;
        bool dma_2:1;
        bool dma_1:1;
        bool dma_0:1;
        bool serial_communication:1;
        bool timer3_overflow:1;
        bool timer2_overflow:1;
        bool timer1_overflow:1;
        bool timer0_overflow:1;
        bool lcd_vcounter_match:1;
        bool lcd_hblank:1;
        bool lcd_vblank:1;
#endif
    };
    half raw;
} interrupt_enable_t;

typedef union IF {
    struct {
#ifdef GBA_LITTLE_ENDIAN
        bool vblank:1;
        bool hblank:1;
        bool vcount:1;
        bool timer0:1;
        bool timer1:1;
        bool timer2:1;
        bool timer3:1;
        bool sio:1;
        bool dma0:1;
        bool dma1:1;
        bool dma2:1;
        bool dma3:1;
        bool keypad:1;
        bool gamepak:1;
        unsigned:2;
#else
        unsigned:2;
        bool gamepak:1;
        bool keypad:1;
        bool dma3:1;
        bool dma2:1;
        bool dma1:1;
        bool dma0:1;
        bool sio:1;
        bool timer3:1;
        bool timer2:1;
        bool timer1:1;
        bool timer0:1;
        bool vcount:1;
        bool hblank:1;
        bool vblank:1;
#endif
    };
    half raw;
} IF_t;

typedef union KEYINPUT {
    struct {
#ifdef GBA_LITTLE_ENDIAN
        bool a:1;
        bool b:1;
        bool select:1;
        bool start:1;
        bool right:1;
        bool left:1;
        bool up:1;
        bool down:1;
        bool r:1;
        bool l:1;
        unsigned:6;
#else
        unsigned:6;
        bool l:1;
        bool r:1;
        bool down:1;
        bool up:1;
        bool left:1;
        bool right:1;
        bool start:1;
        bool select:1;
        bool b:1;
        bool a:1;
#endif
    };
    uint16_t raw;
} KEYINPUT_t;

typedef struct gbabus {
    interrupt_master_enable_t interrupt_master_enable;
    interrupt_master_enable_t ime_temp;
    interrupt_enable_t interrupt_enable;
    IF_t IF;
    KEYINPUT_t KEYINPUT;

    /*
    // DMA
    int current_active_dma;
    addr_27b_t DMA0SAD;
    addr_27b_t DMA0DAD;
    wc_14b_t   DMA0CNT_L;
    DMACNTH_t  DMA0CNT_H;
    DMAINT_t   DMA0INT;

    addr_28b_t DMA1SAD;
    addr_27b_t DMA1DAD;
    wc_14b_t   DMA1CNT_L;
    DMACNTH_t  DMA1CNT_H;
    DMAINT_t   DMA1INT;

    addr_28b_t DMA2SAD;
    addr_27b_t DMA2DAD;
    wc_14b_t   DMA2CNT_L;
    DMACNTH_t  DMA2CNT_H;
    DMAINT_t   DMA2INT;

    addr_28b_t DMA3SAD;
    addr_28b_t DMA3DAD;
    wc_16b_t   DMA3CNT_L;
    DMACNTH_t  DMA3CNT_H;
    DMAINT_t   DMA3INT;

    RCNT_t RCNT;
    JOYCNT_t JOYCNT;
    KEYCNT_t KEYCNT;

    SOUNDBIAS_t SOUNDBIAS;

    bool TMSTART[4];
    byte TMACTIVE[4];
    byte num_active_timers;
    TMCNT_L_t TMCNT_L[4];
    TMCNT_H_t TMCNT_H[4];
    TMINT_t TMINT[4];

    WAITCNT_t WAITCNT;

    backup_type_t backup_type;
    byte gpio_read_mask;
    byte gpio_write_mask;
    bool allow_gpio_read;
    gpio_device_t gpio_device;
    gpio_port_t port;
    rtc_t rtc;
    */
} gbabus_t;

extern gbamem_t mem;
extern gbabus_t bus;
extern arm7tdmi_t* cpu;

void gba_system_loop();
void gba_init(const char* rom);
#endif //N64_GBA_GBA_H
