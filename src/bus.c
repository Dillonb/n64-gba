#include "bus.h"

#include <libdragon.h>

#include "gba.h"
#include "ioreg_util.h"
#include "ioreg_names.h"
#include "ppu.h"

#define REGION_BIOS       0x00
#define REGION_EWRAM      0x02
#define REGION_IWRAM      0x03
#define REGION_IOREG      0x04
#define REGION_PRAM       0x05
#define REGION_VRAM       0x06
#define REGION_OAM        0x07
#define REGION_GAMEPAK0_L 0x08
#define REGION_GAMEPAK0_H 0x09
#define REGION_GAMEPAK1_L 0x0A
#define REGION_GAMEPAK1_H 0x0B
#define REGION_GAMEPAK2_L 0x0C
#define REGION_GAMEPAK2_H 0x0D
#define REGION_SRAM       0x0E
#define REGION_SRAM_MIRR  0x0F

INLINE byte read_byte_ioreg(word addr) {
    switch (addr & 0xFFF) {
        case IO_POSTFLG:
            logwarn("Ignoring read from POSTFLG reg. Returning 0")
            return 0;
        default:
            logfatal("Reading byte ioreg at 0x%08lX", addr)
    }
}

INLINE half* get_half_ioreg_ptr(word addr, bool write) {
    word regnum = addr & 0xFFF;
    switch (regnum) {
        case IO_DISPCNT: return &ppu.DISPCNT.raw;
        case IO_DISPSTAT: return &ppu.DISPSTAT.raw;
        case IO_BG0CNT: logfatal("return &ppu->BG0CNT.raw;")
        case IO_BG1CNT: logfatal("return &ppu->BG1CNT.raw;")
        case IO_BG2CNT: logfatal("return &ppu->BG2CNT.raw;")
        case IO_BG3CNT: logfatal("return &ppu->BG3CNT.raw;")
        case IO_BG0HOFS: logfatal("return &ppu->BG0HOFS.raw;")
        case IO_BG1HOFS: logfatal("return &ppu->BG1HOFS.raw;")
        case IO_BG2HOFS: logfatal("return &ppu->BG2HOFS.raw;")
        case IO_BG3HOFS: logfatal("return &ppu->BG3HOFS.raw;")
        case IO_BG0VOFS: logfatal("return &ppu->BG0VOFS.raw;")
        case IO_BG1VOFS: logfatal("return &ppu->BG1VOFS.raw;")
        case IO_BG2VOFS: logfatal("return &ppu->BG2VOFS.raw;")
        case IO_BG3VOFS: logfatal("return &ppu->BG3VOFS.raw;")
        case IO_BG2PA: logfatal("return &ppu->BG2PA.raw;")
        case IO_BG2PB: logfatal("return &ppu->BG2PB.raw;")
        case IO_BG2PC: logfatal("return &ppu->BG2PC.raw;")
        case IO_BG2PD: logfatal("return &ppu->BG2PD.raw;")
        case IO_BG3PA: logfatal("return &ppu->BG3PA.raw;")
        case IO_BG3PB: logfatal("return &ppu->BG3PB.raw;")
        case IO_BG3PC: logfatal("return &ppu->BG3PC.raw;")
        case IO_BG3PD: logfatal("return &ppu->BG3PD.raw;")
        case IO_WIN0H: logfatal("return &ppu->WIN0H.raw;")
        case IO_WIN1H: logfatal("return &ppu->WIN1H.raw;")
        case IO_WIN0V: logfatal("return &ppu->WIN0V.raw;")
        case IO_WIN1V: logfatal("return &ppu->WIN1V.raw;")
        case IO_WININ: logfatal("return &ppu->WININ.raw;")
        case IO_WINOUT: logfatal("return &ppu->WINOUT.raw;")
        case IO_MOSAIC: logfatal("return &ppu->MOSAIC.raw;")
        case IO_BLDCNT: logfatal("return &ppu->BLDCNT.raw;")
        case IO_BLDALPHA: logfatal("return &ppu->BLDALPHA.raw;")
        case IO_BLDY: logfatal("return &ppu->BLDY.raw;")
        case IO_IE: return &bus.interrupt_enable.raw;
        case IO_DMA0CNT_L: logfatal("return &bus->DMA0CNT_L.raw;")
        case IO_DMA0CNT_H: logfatal("return &bus->DMA0CNT_H.raw;")
        case IO_DMA1CNT_L: logfatal("return &bus->DMA1CNT_L.raw;")
        case IO_DMA1CNT_H: logfatal("return &bus->DMA1CNT_H.raw;")
        case IO_DMA2CNT_L: logfatal("return &bus->DMA2CNT_L.raw;")
        case IO_DMA2CNT_H: logfatal("return &bus->DMA2CNT_H.raw;")
        case IO_DMA3CNT_L: logfatal("return &bus->DMA3CNT_L.raw;")
        case IO_DMA3CNT_H: logfatal("return &bus->DMA3CNT_H.raw;")
        case IO_KEYINPUT: return &bus.KEYINPUT.raw;
        case IO_RCNT: logfatal("return &bus->RCNT.raw;")
        case IO_JOYCNT: logfatal("return &bus->JOYCNT.raw;")
        case IO_IME: return &bus.interrupt_master_enable.raw;
        case IO_SOUNDBIAS: logfatal("return &bus->SOUNDBIAS.raw;")
        case IO_TM0CNT_L: {
            if (write) {
                logfatal("return &bus->TMCNT_L[0].raw;")
            } else {
                logfatal("return &bus->TMINT[0].value;")
            }
        }
        case IO_TM0CNT_H: logfatal("return &bus->TMCNT_H[0].raw;")
        case IO_TM1CNT_L: {
            if (write) {
                logfatal("return &bus->TMCNT_L[1].raw;")
            } else {
                logfatal("return &bus->TMINT[1].value;")
            }
        }
        case IO_TM1CNT_H: logfatal("return &bus->TMCNT_H[1].raw;")
        case IO_TM2CNT_L: {
            if (write) {
                logfatal("return &bus->TMCNT_L[2].raw;")
            } else {
                logfatal("return &bus->TMINT[2].value;")
            }
        }
        case IO_TM2CNT_H: logfatal("return &bus->TMCNT_H[2].raw;")
        case IO_TM3CNT_L: {
            if (write) {
                logfatal("return &bus->TMCNT_L[3].raw;")
            } else {
                logfatal("return &bus->TMINT[3].value;")
            }
        }
        case IO_TM3CNT_H: logfatal("return &bus->TMCNT_H[3].raw;")
        case IO_KEYCNT: logfatal("return &bus.KEYCNT.raw;")
        case IO_IF: return &bus.IF.raw;
        case IO_WAITCNT:
            logfatal("return &bus->WAITCNT.raw;")
        case IO_UNDOCUMENTED_GREEN_SWAP:
            logwarn("Ignoring access to Green Swap register")
            return NULL;
        case IO_VCOUNT: logfatal("return &ppu->y;")
        case IO_SOUND1CNT_L:
        case IO_SOUND1CNT_H:
        case IO_SOUND1CNT_X:
        case IO_SOUND2CNT_L:
        case IO_SOUND2CNT_H:
        case IO_SOUND3CNT_L:
        case IO_SOUND3CNT_H:
        case IO_SOUND3CNT_X:
        case IO_SOUND4CNT_L:
        case IO_SOUND4CNT_H:
        case IO_SOUNDCNT_L:
        case IO_SOUNDCNT_H:
        case IO_SOUNDCNT_X:
        case WAVE_RAM0_L:
        case WAVE_RAM0_H:
        case WAVE_RAM1_L:
        case WAVE_RAM1_H:
        case WAVE_RAM2_L:
        case WAVE_RAM2_H:
        case WAVE_RAM3_L:
        case WAVE_RAM3_H:
            logwarn("Ignoring access to sound register: 0x%03lX", regnum)
            return NULL;

        case IO_SIOCNT:
        case IO_SIOMULTI0:
        case IO_SIOMULTI1:
        case IO_SIOMULTI2:
        case IO_SIOMULTI3:
        case IO_SIOMLT_SEND:
        case IO_JOY_RECV:
        case IO_JOY_TRANS:
        case IO_JOYSTAT:
            logwarn("Ignoring access to SIO register: 0x%03lX", regnum)
            return NULL;

        default:
            logfatal("Access to unknown (but valid) half ioreg addr 0x%08lX", addr)
    }
}

INLINE word* get_word_ioreg_ptr(word addr) {
    unimplemented(get_ioreg_size_for_addr(addr) != sizeof(word), "Trying to get the address of a wrong-sized word ioreg")
    switch (addr & 0xFFF) {
        case IO_BG2X:     logfatal("return &ppu->BG2X.initial.raw;")
        case IO_BG2Y:     logfatal("return &ppu->BG2Y.initial.raw;")
        case IO_BG3X:     logfatal("return &ppu->BG3X.initial.raw;")
        case IO_BG3Y:     logfatal("return &ppu->BG3Y.initial.raw;")
        case IO_DMA0SAD:  logfatal("return &bus->DMA0SAD.raw;")
        case IO_DMA0DAD:  logfatal("return &bus->DMA0DAD.raw;")
        case IO_DMA1SAD:  logfatal("return &bus->DMA1SAD.raw;")
        case IO_DMA1DAD:  logfatal("return &bus->DMA1DAD.raw;")
        case IO_DMA2SAD:  logfatal("return &bus->DMA2SAD.raw;")
        case IO_DMA2DAD:  logfatal("return &bus->DMA2DAD.raw;")
        case IO_DMA3SAD:  logfatal("return &bus->DMA3SAD.raw;")
        case IO_DMA3DAD:  logfatal("return &bus->DMA3DAD.raw;")
        case IO_FIFO_A:
        case IO_FIFO_B:
        case IO_JOY_RECV:
        case IO_JOY_TRANS:
        case IO_IMEM_CTRL:
            return NULL;
        default: logfatal("Tried to get the address of an unknown (but valid) word ioreg addr: 0x%08lX", addr)
    }
}

INLINE word read_word_ioreg(word addr) {
    if (!is_ioreg_readable(addr)) {
        logwarn("Returning 0 (UNREADABLE BUT VALID WORD IOREG 0x%08lX)", addr)
        return 0;
    }
    word* ioreg = get_word_ioreg_ptr(addr);
    if (ioreg) {
        return *ioreg;
    } else {
        logwarn("Ignoring read from word ioreg at 0x%08lX and returning 0.", addr)
        return 0;
    }
}

INLINE half read_half_ioreg(word addr) {
    if (!is_ioreg_readable(addr)) {
        logwarn("Returning 0 (UNREADABLE BUT VALID HALF IOREG 0x%08lX)", addr)
        return 0;
    }
    half* ioreg = get_half_ioreg_ptr(addr, false);
    if (ioreg) {
        return *ioreg;
    } else {
        logwarn("Ignoring read from half ioreg at 0x%08lX and returning 0.", addr)
        return 0;
    }
}


INLINE void write_word_ioreg_masked(word addr, word value, word mask) {
    word maskedaddr = addr & 0xFFF;
    switch (maskedaddr) {
        case IO_FIFO_A:
            //write_fifo(apu, 0, value, mask);
            break;
        case IO_FIFO_B:
            //write_fifo(apu, 1, value, mask);
            break;
        case IO_BG2X:
        case IO_BG2Y:
        case IO_BG3X:
        case IO_BG3Y: {
            /*
            word* current = NULL;
            word* initial = NULL;
            switch (maskedaddr) {
                case IO_BG2X:
                    current = &ppu->BG2X.current.raw;
                    initial = &ppu->BG2X.initial.raw;
                    break;
                case IO_BG2Y:
                    current = &ppu->BG2Y.current.raw;
                    initial = &ppu->BG2Y.initial.raw;
                    break;
                case IO_BG3X:
                    current = &ppu->BG3X.current.raw;
                    initial = &ppu->BG3X.initial.raw;
                    break;
                case IO_BG3Y:
                    current = &ppu->BG3Y.current.raw;
                    initial = &ppu->BG3Y.initial.raw;
                    break;
                default:
                    logfatal("Fatal error- should not have ended up here.")
            }
            // Only update "current" if is VBlank
            if (!is_vblank(ppu)) {
                *current &= (~mask);
                *current |= (value & mask);
            }
            *initial &= (~mask);
            *initial |= (value & mask);
             */
            break;
        }
        default:
            if (!is_ioreg_writable(addr)) {
                logwarn("Ignoring write to unwriteable word ioreg 0x%08lX", addr)
                return;
            }
            word* ioreg = get_word_ioreg_ptr(addr);
            if (ioreg) {
                *ioreg &= (~mask);
                *ioreg |= (value & mask);
            } else {
                logwarn("Ignoring write to word ioreg 0x%08lX with mask 0x%08lX", addr, mask)
            }
    }
}

INLINE void write_half_ioreg_masked(word addr, half value, half mask) {
    half* ioreg = get_half_ioreg_ptr(addr, true);
    if (ioreg) {
        switch (addr & 0xFFF) {
            case IO_DISPSTAT:
                mask &= 0b1111111111111000; // Last 3 bits are read-only
                break;
            case IO_IME:
                mask = 0b1;
                break;
            case IO_IF: {
                bus.IF.raw &= ~value;
                return;
            }
            default:
                break; // No special case
        }
        //half old_value = *ioreg;
        *ioreg &= (~mask);
        *ioreg |= (value & mask);
        switch (addr & 0xFFF) {
            /*
            case IO_DMA0CNT_H:
                if (!bus->DMA0CNT_H.dma_enable) {
                    bus->DMA0INT.previously_enabled = false;
                }
                gba_dma();
                break;
            case IO_DMA1CNT_H:
                if (!bus->DMA1CNT_H.dma_enable) {
                    bus->DMA1INT.previously_enabled = false;
                }
                gba_dma();
                break;
            case IO_DMA2CNT_H:
                if (!bus->DMA2CNT_H.dma_enable) {
                    bus->DMA2INT.previously_enabled = false;
                }
                gba_dma();
                break;
            case IO_DMA3CNT_H:
                if (!bus->DMA3CNT_H.dma_enable) {
                    bus->DMA3INT.previously_enabled = false;
                }
                gba_dma();
                break;
            case IO_DMA0CNT_L:
            case IO_DMA1CNT_L:
            case IO_DMA2CNT_L:
            case IO_DMA3CNT_L:
                gba_dma();
                break;
            case IO_TM0CNT_H:
                tmcnth_write(0, old_value);
                break;
            case IO_TM1CNT_H:
                tmcnth_write(1, old_value);
                break;
            case IO_TM2CNT_H:
                tmcnth_write(2, old_value);
                break;
            case IO_TM3CNT_H:
                tmcnth_write(3, old_value);
                break;
            case IO_WAITCNT:
                on_waitcnt_updated();
                break;
                */
        }
    } else {
        logwarn("Ignoring write to half ioreg 0x%08lX", addr)
    }
}

INLINE void write_half_ioreg(word addr, half value) {
    if (!is_ioreg_writable(addr)) {
        logwarn("Ignoring write to unwriteable half ioreg 0x%08lX", addr)
        return;
    }
    // Write to the whole thing
    write_half_ioreg_masked(addr, value, 0xFFFF);
}

INLINE void write_word_ioreg(word addr, word value) {
    // 0x04XX0800 is the only address that's mirrored.
    if ((addr & 0xFF00FFFFu) == 0x04000800u) {
        addr = 0x04000800;
    }

    if (!is_ioreg_writable(addr)) {
        logwarn("Ignoring write to unwriteable word ioreg 0x%08lX", addr)
    }

    write_word_ioreg_masked(addr, value, 0xFFFFFFFF);
}

INLINE void write_byte_ioreg(word addr, byte value) {
    if (!is_ioreg_writable(addr)) {
        logwarn("Ignoring write to unwriteable byte ioreg 0x%08lX", addr)
        return;
    }
    byte size = get_ioreg_size_for_addr(addr);
    if (size == sizeof(half)) {
        word offset = (addr % sizeof(half));
        half shifted_value = value;
        shifted_value <<= (offset * 8);
        write_half_ioreg_masked(addr - offset, shifted_value, 0xFF << (offset * 8));
    } else if (size == sizeof(word)) {
        word offset = (addr % sizeof(word));
        word shifted_value = value;
        shifted_value <<= (offset * 8);
        write_word_ioreg_masked(addr - offset, shifted_value, 0xFF << (offset * 8));
    } else {
        // Don't really need to do the get addr/write masked thing that the bigger ioregs do
        // Or do I?
        word regnum = addr & 0xFFF;
        switch (regnum) {
            case IO_HALTCNT: {
                if ((value & 1) == 0) {
                    logwarn("HALTING CPU!")
                    cpu->halt = true;
                } else {
                    logfatal("Wrote to HALTCNT with bit 0 being 1")
                }
                break;
            }
            case IO_POSTFLG:
                logwarn("Ignoring write to POSTFLG register")
                break;
            default:
                logfatal("Write to unknown (but valid) byte ioreg addr 0x%08lX == 0x%02X", addr, value)
        }
    }
}

byte gba_read_byte(word address, access_type_t access_type) {
    half region = address >> 24;
    switch (region) {
        case REGION_BIOS:
            logfatal("gba_read_byte REGION_BIOS 0x%08lX", address);
        case REGION_EWRAM: {
            word index = (address - 0x02000000) % 0x40000;
            return mem.ewram[BYTE_ADDRESS(index)];
        }
        case REGION_IWRAM: {
            word index = (address - 0x03000000) % 0x8000;
            return mem.iwram[BYTE_ADDRESS(index)];
        }
        case REGION_IOREG:
            logfatal("gba_read_byte REGION_IOREG 0x%08lX", address);
        case REGION_PRAM:
            logfatal("gba_read_byte REGION_PRAM 0x%08lX", address);
        case REGION_VRAM:
            logfatal("gba_read_byte REGION_VRAM 0x%08lX", address);
        case REGION_OAM:
            logfatal("gba_read_byte REGION_OAM 0x%08lX", address);
        case REGION_GAMEPAK0_L:
        case REGION_GAMEPAK0_H:
        case REGION_GAMEPAK1_L:
        case REGION_GAMEPAK1_H:
        case REGION_GAMEPAK2_L:
        case REGION_GAMEPAK2_H: {
            word index = BYTE_ADDRESS(address & 0x1FFFFFF);
            if (index < mem.rom_size) {
                u8 result;
                memcpy(&result, mem.rom + index, sizeof(u8));
                return result;
            } else {
                logfatal("open bus");
            }
        }
            logfatal("gba_read_byte REGION_GAMEPAK2_H 0x%08lX", address);
        case REGION_SRAM:
            logfatal("gba_read_byte REGION_SRAM 0x%08lX", address);
        case REGION_SRAM_MIRR:
            logfatal("gba_read_byte REGION_SRAM_MIRR 0x%08lX", address);
    }
    return 0;
}

half gba_read_half(word address, access_type_t access_type) {
    address &= ~(sizeof(half) - 1);
    half region = address >> 24;
    switch (region) {
        case REGION_BIOS:
            logfatal("gba_read_half REGION_BIOS 0x%08lX", address);
        case REGION_EWRAM: {
            word index = (address - 0x02000000) % 0x40000;
            return half_from_byte_array(mem.ewram, index);
        }
        case REGION_IWRAM: {
            word index = (address - 0x03000000) % 0x8000;
            return half_from_byte_array(mem.iwram, index);
        }
        case REGION_IOREG: {
            if (address < 0x04000400) {
                byte size = get_ioreg_size_for_addr(address);
                switch (size) {
                    case 0:
                        logfatal("Returning open bus (UNUSED HALF IOREG 0x%08lX)", address)
                    case sizeof(byte):
                        return read_byte_ioreg(address) | (read_byte_ioreg(address + 1) << 8);
                    case sizeof(half):
                        return read_half_ioreg(address);
                    case sizeof(word): {
                        byte ofs = address % 4;
                        return (read_word_ioreg(address) >> (ofs * 8)) & 0xFFFF;
                        default:
                            logfatal("Unrecognized size in read_half_ioreg!")
                    }
                }
            } else {
                logfatal("Open bus")
            }
        }
            logfatal("gba_read_half REGION_IOREG 0x%08lX", address);
        case REGION_PRAM:
            logfatal("gba_read_half REGION_PRAM 0x%08lX", address);
        case REGION_VRAM:
            logfatal("gba_read_half REGION_VRAM 0x%08lX", address);
        case REGION_OAM:
            logfatal("gba_read_half REGION_OAM 0x%08lX", address);
        case REGION_GAMEPAK0_L:
        case REGION_GAMEPAK0_H:
        case REGION_GAMEPAK1_L:
        case REGION_GAMEPAK1_H:
        case REGION_GAMEPAK2_L:
        case REGION_GAMEPAK2_H: {
            word index = HALF_ADDRESS(address & 0x1FFFFFF);
            if (index < mem.rom_size) {
                u16 result;
                memcpy(&result, mem.rom + index, sizeof(u16));
                return result;
            } else {
                logfatal("open bus");
            }
        }
            logfatal("gba_read_half REGION_GAMEPAK2_H 0x%08lX", address);
        case REGION_SRAM:
            logfatal("gba_read_half REGION_SRAM 0x%08lX", address);
        case REGION_SRAM_MIRR:
            logfatal("gba_read_half REGION_SRAM_MIRR 0x%08lX", address);
    }
    return 0;
}

word gba_read_word(word address, access_type_t access_type) {
    address &= ~(sizeof(word) - 1);
    half region = address >> 24;
    switch (region) {
        case REGION_BIOS:
            logfatal("gba_read_word REGION_BIOS 0x%08lX", address);
        case REGION_EWRAM: {
            word index = (address - 0x02000000) % 0x40000;
            return word_from_byte_array(mem.ewram, index);
        }
        case REGION_IWRAM: {
            word index = (address - 0x03000000) % 0x8000;
            return word_from_byte_array(mem.iwram, index);
        }
        case REGION_IOREG:
            logfatal("gba_read_word REGION_IOREG 0x%08lX", address);
        case REGION_PRAM:
            logfatal("gba_read_word REGION_PRAM 0x%08lX", address);
        case REGION_VRAM:
            logfatal("gba_read_word REGION_VRAM 0x%08lX", address);
        case REGION_OAM:
            logfatal("gba_read_word REGION_OAM 0x%08lX", address);
        case REGION_GAMEPAK0_L:
        case REGION_GAMEPAK0_H:
        case REGION_GAMEPAK1_L:
        case REGION_GAMEPAK1_H:
        case REGION_GAMEPAK2_L:
        case REGION_GAMEPAK2_H: {
            word index = WORD_ADDRESS(address & 0x1FFFFFF);
            if (index < mem.rom_size) {
                u32 result;
                memcpy(&result, mem.rom + index, sizeof(u32));
                return result;
            } else {
                logfatal("open bus");
            }
        }
        case REGION_SRAM:
            logfatal("gba_read_word REGION_SRAM 0x%08lX", address);
        case REGION_SRAM_MIRR:
            logfatal("gba_read_word REGION_SRAM_MIRR 0x%08lX", address);
    }
    return 0;
}

INLINE bool is_bitmap() {
    int mode = ppu.DISPCNT.mode;
    return mode >= 3 && mode < 6;
}

INLINE bool is_bg(word address) {
    if (is_bitmap()) {
        return address >= 0x6000000 && address <= 0x6013FFF;
    } else {
        return address >= 0x6000000 && address <= 0x600FFFF;
    }
}

void gba_write_byte(word address, byte value, access_type_t access_type) {
    half region = address >> 24;
    switch (region) {
        case REGION_BIOS:
            logfatal("gba_write_byte REGION_BIOS 0x%08lX", address);
        case REGION_EWRAM: {
            word index = (address - 0x02000000) % 0x40000;
            mem.ewram[BYTE_ADDRESS(index)] = value;
            break;
        }
        case REGION_IWRAM: {
            word index = (address - 0x03000000) % 0x8000;
            mem.iwram[BYTE_ADDRESS(index)] = value;
            break;
        }
        case REGION_IOREG:
            logfatal("gba_write_byte REGION_IOREG 0x%08lX", address);
            break;
        case REGION_PRAM: {
            word index = (address - 0x5000000) % 0x400;
            word lower_index = index & 0xFFFFFFFE;
            word upper_index = lower_index + 1;
            ppu.pram[BYTE_ADDRESS(lower_index)] = value;
            ppu.pram[BYTE_ADDRESS(upper_index)] = value;
            break;
        }
        case REGION_VRAM: {
            word index = address - 0x06000000;
            index %= VRAM_SIZE;
            // Special case for single byte writes to VRAM, OBJ writes are ignored.
            if (is_bg(address)) {
                word lower_index = index & 0xFFFFFFFE;
                word upper_index = lower_index + 1;
                ppu.vram[BYTE_ADDRESS(lower_index)] = value;
                ppu.vram[BYTE_ADDRESS(upper_index)] = value;
            }
            break;
        }
        case REGION_OAM:
            logfatal("gba_write_byte REGION_OAM 0x%08lX", address);
        case REGION_GAMEPAK0_L:
        case REGION_GAMEPAK0_H:
        case REGION_GAMEPAK1_L:
        case REGION_GAMEPAK1_H:
        case REGION_GAMEPAK2_L:
        case REGION_GAMEPAK2_H:
            logfatal("gba_write_byte GAMEPAK 0x%08lX", address);
        case REGION_SRAM:
            logfatal("gba_write_byte REGION_SRAM 0x%08lX", address);
        case REGION_SRAM_MIRR:
            logfatal("gba_write_byte REGION_SRAM_MIRR 0x%08lX", address);
    }

}

void gba_write_half(word address, half value, access_type_t access_type) {
    address &= ~(sizeof(half) - 1);
    half region = address >> 24;
    switch (region) {
        case REGION_BIOS:
            logfatal("gba_write_half REGION_BIOS 0x%08lX", address);
        case REGION_EWRAM: {
            word index = (address - 0x02000000) % 0x40000;
            half_to_byte_array(mem.ewram, HALF_ADDRESS(index), value);
            break;
        }
        case REGION_IWRAM: {
            word index = (address - 0x03000000) % 0x8000;
            half_to_byte_array(mem.iwram, HALF_ADDRESS(index), value);
            break;
        }
        case REGION_IOREG:
            logfatal("gba_write_half REGION_IOREG 0x%08lX", address);
            break;
        case REGION_PRAM: {
            word index = (address - 0x5000000) % 0x400;
            half_to_byte_array(ppu.pram, HALF_ADDRESS(index), value);
            break;
        }
        case REGION_VRAM: {
            word index = address - 0x06000000;
            index %= VRAM_SIZE;
            half_to_byte_array(ppu.vram, HALF_ADDRESS(index), value);
            break;
        }
        case REGION_OAM:
            logfatal("gba_write_half REGION_OAM 0x%08lX", address);
        case REGION_GAMEPAK0_L:
        case REGION_GAMEPAK0_H:
        case REGION_GAMEPAK1_L:
        case REGION_GAMEPAK1_H:
        case REGION_GAMEPAK2_L:
        case REGION_GAMEPAK2_H:
            logfatal("gba_write_half GAMEPAK 0x%08lX", address);
        case REGION_SRAM:
            logfatal("gba_write_half REGION_SRAM 0x%08lX", address);
        case REGION_SRAM_MIRR:
            logfatal("gba_write_half REGION_SRAM_MIRR 0x%08lX", address);
    }
}

void gba_write_word(word address, word value, access_type_t access_type) {
    address &= ~(sizeof(word) - 1);
    half region = address >> 24;
    switch (region) {
        case REGION_BIOS:
            logfatal("gba_write_word REGION_BIOS 0x%08lX", address);
        case REGION_EWRAM: {
            word index = (address - 0x02000000) % 0x40000;
            word_to_byte_array(mem.ewram, WORD_ADDRESS(index), value);
            break;
        }
        case REGION_IWRAM: {
            word index = (address - 0x03000000) % 0x8000;
            word_to_byte_array(mem.iwram, WORD_ADDRESS(index), value);
            break;
        }
        case REGION_IOREG:
            if (address < 0x04000400) {
                byte size = get_ioreg_size_for_addr(address);
                switch (size) {
                    case 0:
                        return;
                    case sizeof(byte):
                        write_byte_ioreg(address, value & 0xFF);
                        write_byte_ioreg(address + 1, (value >> 8) & 0xFF);
                        write_byte_ioreg(address + 2, (value >> 16) & 0xFF);
                        write_byte_ioreg(address + 3, (value >> 24) & 0xFF);
                        break;
                    case sizeof(half):
                        write_half_ioreg(address, value & 0xFFFF);
                        write_half_ioreg(address + 2, (value >> 16) & 0xFFFF);
                        break;
                    case sizeof(word):
                        write_word_ioreg(address, value);
                }
            }
            break;
        case REGION_PRAM:
            logfatal("gba_write_word REGION_PRAM 0x%08lX", address);
        case REGION_VRAM: {
            word index = address - 0x06000000;
            index %= VRAM_SIZE;
            word_to_byte_array(ppu.vram, WORD_ADDRESS(index), value);
            break;
        }
        case REGION_OAM:
            logfatal("gba_write_word REGION_OAM 0x%08lX", address);
        case REGION_GAMEPAK0_L:
        case REGION_GAMEPAK0_H:
        case REGION_GAMEPAK1_L:
        case REGION_GAMEPAK1_H:
        case REGION_GAMEPAK2_L:
        case REGION_GAMEPAK2_H:
            logfatal("gba_write_word GAMEPAK 0x%08lX", address);
        case REGION_SRAM:
            logfatal("gba_write_word REGION_SRAM 0x%08lX", address);
        case REGION_SRAM_MIRR:
            logfatal("gba_write_word REGION_SRAM_MIRR 0x%08lX", address);
    }
}

void handle_input() {
    controller_scan();
    struct controller_data keys = get_keys_pressed();

    bus.KEYINPUT.a =      !keys.c[0].A;
    bus.KEYINPUT.b =      !keys.c[0].B;
    bus.KEYINPUT.select = !keys.c[0].C_down;
    bus.KEYINPUT.start =  !keys.c[0].start;
    bus.KEYINPUT.right =  !keys.c[0].right;
    bus.KEYINPUT.left =   !keys.c[0].left;
    bus.KEYINPUT.up =     !keys.c[0].up;
    bus.KEYINPUT.down =   !keys.c[0].down;
    bus.KEYINPUT.r =      !keys.c[0].R;
    bus.KEYINPUT.l =      !keys.c[0].L;
}
