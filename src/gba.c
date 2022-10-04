#include "gba.h"
#include <libdragon.h>
#include <stdio.h>
#include <string.h>

#include "bus.h"
#include "ppu.h"

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
            ppu_hblank();
            extra = run_system(HBLANK_CYCLES + extra);
            ppu_end_hblank();
        }
        ppu_vblank(ppu);
        for (int line = 0; line < VBLANK_LINES; line++) {
            extra = run_system(VISIBLE_CYCLES + extra);
            ppu_hblank();
            extra = run_system(HBLANK_CYCLES + extra);
            ppu_end_hblank();
        }
        ppu_end_vblank();
    }
}

const uintptr_t VIRTUAL_ROM_ADDRESS = 0x40000000; // 1GiB into KUSEG. should be very safe to use this while still catching all null dereferences

// 128KiB
#define GBA_ROM_PAGE_SIZE 0x20000
// To map 128KiB pages on the TLB, we need to map 2x 64KiB pages in entry_lo0 and entry_lo1
#define GBA_ROM_HALF_PAGE_SIZE (GBA_ROM_PAGE_SIZE >> 1)
// 32 TLB slots, 32 pages loaded at once. 4MiB.
#define GBA_ROM_NUM_PAGES 32

//#define PAGEMASK_4K (0b0000000000 << 13)
//#define PAGEMASK_16K (0b0000000011 << 13)
#define PAGEMASK_64K (0b0000001111 << 13)
//#define PAGEMASK_256K (0b0000111111 << 13)
//#define PAGEMASK_1M (0b0011111111 << 13)
//#define PAGEMASK_4M (0b1111111111 << 13)

u8* loaded_pages; //[GBA_ROM_NUM_PAGES][GBA_ROM_PAGE_SIZE];

u64 get_vpn(u64 address, u32 page_mask_raw) {
    u64 tmp = page_mask_raw | 0x1FFF;
    // bits 40 and 41: bits 62 and 63 of the address, the "region"
    // bits 0 - 39: the low 40 bits of the address, the actual location being accessed.
    u64 vpn = (address & 0xFFFFFFFFFF) | ((address >> 22) & 0x30000000000);

    // This function is also called for entry_hi, the low 8 bits of which are the ASID
    // this is fine, this mask will take care of that.
    vpn &= ~tmp;
    return vpn;
}

u32 get_entry_lo(bool valid, bool dirty, u32 c, u32 pfn) {
    u32 entry_lo = 0;
    debugf("Building entry_lo. valid: %d, dirty: %d, c: %lu, PFN: %08lX\n", valid, dirty, c, pfn);
    entry_lo |= (valid & 1) << 1;
    entry_lo |= (dirty & 1) << 2;
    entry_lo |= (c & 0b111) << 3;
    entry_lo |= (pfn & 0xFFFFF) << 6;
    return entry_lo;
}

void gba_load_rom_page(word addr) {
    static int tlb_page = 0;
    int rom_page = (addr - VIRTUAL_ROM_ADDRESS) / GBA_ROM_PAGE_SIZE;

    uintptr_t rom_page_offset = rom_page * GBA_ROM_PAGE_SIZE;
    uintptr_t virtual_rom_page_base = VIRTUAL_ROM_ADDRESS + rom_page_offset;
    uintptr_t ram_addr = (uintptr_t)loaded_pages + rom_page_offset;
    uintptr_t ram_addr_physical = ram_addr & 0x1FFFFFFF;
    uintptr_t rom_addr = (DFS_DEFAULT_LOCATION + rom_page_offset) & 0x1FFFFFFF;

    debugf("Need to load page containing address %08lX\n", addr);
    debugf("Trying to map %08X to %08X\n", virtual_rom_page_base, ram_addr_physical);


    dma_read((u8*)ram_addr, rom_addr, GBA_ROM_PAGE_SIZE);

    C0_WRITE_INDEX(tlb_page);

    u32 entry_hi = get_vpn((uintptr_t) virtual_rom_page_base, PAGEMASK_64K);

    u32 even_paddr = (ram_addr_physical) >> 12;
    u32 odd_paddr  = (ram_addr_physical + GBA_ROM_HALF_PAGE_SIZE) >> 12;

    u32 entry_lo0 = get_entry_lo(true, false, 2 /* don't use cache */, even_paddr);
    u32 entry_lo1 = get_entry_lo(true, false, 2 /* don't use cache */, odd_paddr);

    _Static_assert(GBA_ROM_PAGE_SIZE == 0x20000, "If the rom page size changes, remember to change these values too");
    C0_WRITE_PAGEMASK(PAGEMASK_64K); // 64k pages:
    C0_WRITE_ENTRYHI(entry_hi);
    C0_WRITE_ENTRYLO0(entry_lo0);
    C0_WRITE_ENTRYLO1(entry_lo1);

    debugf("Writing TLB entry:");
    debugf("Index    %d\n", tlb_page);
    debugf("PageMask %08X\n", PAGEMASK_64K);
    debugf("EntryHi  %08lX\n", entry_hi);
    debugf("EntryLo0 %08lX\n", entry_lo0);
    debugf("EntryLo1 %08lX\n", entry_lo1);

    C0_TLBWI();

    tlb_page++;
    if (tlb_page >= GBA_ROM_NUM_PAGES) {
        tlb_page = 0;
    }
}

void gba_init(const char* rom) {
    memset(&mem, 0, sizeof(mem));
    memset(&bus, 0, sizeof(bus));
    memset(&ppu, 0, sizeof(ppu));

    mem.rom_size = 16936; // armwrestler.gba
    mem.rom = (u8*)VIRTUAL_ROM_ADDRESS;

    loaded_pages = malloc_uncached_aligned(GBA_ROM_PAGE_SIZE, GBA_ROM_PAGE_SIZE * GBA_ROM_NUM_PAGES);
    if (loaded_pages == NULL) {
        debugf("Unable to allocate\n");
        while (true) {}
    }
    debugf("Allocated %d bytes at %08X\n", GBA_ROM_PAGE_SIZE * GBA_ROM_NUM_PAGES, (uintptr_t)loaded_pages);

    bus.KEYINPUT.raw = 0x03FF;

    // Initialize the CPU, hook it up to the GBA bus
    cpu = init_arm7tdmi(gba_read_byte,
                        gba_read_half,
                        gba_read_word,
                        gba_write_byte,
                        gba_write_half,
                        gba_write_word);
    skip_bios(cpu);
    ACQUIRE_DISPLAY();
}
