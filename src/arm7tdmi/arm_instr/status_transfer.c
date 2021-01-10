#include "status_transfer.h"

typedef union field_masks {
    struct {
#ifdef GBA_LITTLE_ENDIAN
        bool c:1; // Write to control field
        bool x:1; // Write to extension field
        bool s:1; // Write to status field
        bool f:1; // Write to flags field
        byte:4;
#else
        byte:4;
        bool f:1; // Write to flags field
        bool s:1; // Write to status field
        bool x:1; // Write to extension field
        bool c:1; // Write to control field
#endif
    };
    byte raw;
} field_masks_t;
ASSERTBYTE(field_masks_t);

word get_mask(field_masks_t* masks) {
    word mask = 0;
    if (masks->f) mask |= 0xFF000000u;
    if (masks->s) mask |= 0x00FF0000u;
    if (masks->x) mask |= 0x0000FF00u;
    if (masks->c) mask |= 0x000000FFu;
    return mask;
}

typedef union msr_immediate_flags {
    struct {
#ifdef GBA_LITTLE_ENDIAN
        unsigned imm:8;
        unsigned shift:4;
        unsigned:4;
#else
        unsigned:4;
        unsigned shift:4;
        unsigned imm:8;
#endif
    } PACKED parsed;
    half raw;
} msr_immediate_flags_t;
ASSERTHALF(msr_immediate_flags_t);

// http://problemkaputt.de/gbatek.htm#armopcodespsrtransfermrsmsr
void psr_transfer(arm7tdmi_t* state, arminstr_t* arminstr) {
    bool immediate = arminstr->parsed.DATA_PROCESSING.immediate;
    unsigned int dt_opcode = arminstr->parsed.DATA_PROCESSING.opcode;
    unsigned int dt_rn = arminstr->parsed.DATA_PROCESSING.rn;
    unsigned int dt_rd = arminstr->parsed.DATA_PROCESSING.rd;
    unsigned int dt_operand2 = arminstr->parsed.DATA_PROCESSING.operand2;
    union {
        struct {
#ifdef GBA_LITTLE_ENDIAN
            bool msr:1; // if 0, mrs, if 1, msr
            bool spsr:1; // if 0, cpsr, if 1, spsr_current mode
            unsigned:6;
#else
            unsigned:6;
            bool spsr:1; // if 0, cpsr, if 1, spsr_current mode
            bool msr:1; // if 0, mrs, if 1, msr
#endif
        };
        byte raw;
    } opcode;

    ASSERTBYTE(opcode);

    opcode.raw = dt_opcode;

    if (opcode.msr) {
        field_masks_t field_masks;
        field_masks.raw = dt_rn; // field masks come from the "rn" field in data processing
        word mask = get_mask(&field_masks);

        word source_data;

        if (immediate) {
            msr_immediate_flags_t flags;
            flags.raw = dt_operand2;
            source_data = flags.parsed.imm;
            word shift = flags.parsed.shift * 2;

            shift &= 31u;
            source_data = (source_data >> shift) | (source_data << (-shift & 31u));
        }
        else {
            unsigned int source_register = dt_operand2 & 0b1111u;
            source_data = get_register(state, source_register);
        }

        logdebug("Source data: 0x%08X", source_data)
        source_data &= mask;
        logdebug("Mask: 0x%08X", mask)
        logdebug("Source data masked: 0x%08X", source_data)
        if (opcode.spsr) { // SPSR
            word spsr = get_spsr(state)->raw;
            spsr = (spsr & ~mask) | source_data;
            set_spsr(state, spsr);
        }
        else { // CPSR
            word psr = get_psr(state)->raw;
            psr = (psr & ~mask) | source_data;
            set_psr(state, psr);
        }
    }
    else {
        // MRS
        status_register_t* psr = opcode.spsr ? get_spsr(state) : get_psr(state);
        set_register(state, dt_rd, psr->raw);
    }

}

