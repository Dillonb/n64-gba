#include <stdlib.h>
#include <stdbool.h>
#include "arm7tdmi.h"

#include "arm_instr/arm_instr.h"

#include "thumb_instr/thumb_instr.h"

static arminstr_handler_t arm_lut[4096];
static thminstr_handler_t thm_lut[1024];

const char MODE_NAMES[32][11] = {
"UNKNOWN",    // 0b00000
"UNKNOWN",    // 0b00001
"UNKNOWN",    // 0b00010
"UNKNOWN",    // 0b00011
"UNKNOWN",    // 0b00100
"UNKNOWN",    // 0b00101
"UNKNOWN",    // 0b00110
"UNKNOWN",    // 0b00111
"UNKNOWN",    // 0b01000
"UNKNOWN",    // 0b01001
"UNKNOWN",    // 0b01010
"UNKNOWN",    // 0b01011
"UNKNOWN",    // 0b01100
"UNKNOWN",    // 0b01101
"UNKNOWN",    // 0b01110
"UNKNOWN",    // 0b01111
"USER",       // 0b10000
"FIQ",        // 0b10001
"IRQ",        // 0b10010
"SUPERVISOR", // 0b10011
"UNKNOWN",    // 0b10100
"UNKNOWN",    // 0b10101
"UNKNOWN",    // 0b10110
"ABORT",      // 0b10111
"UNKNOWN",    // 0b11000
"UNKNOWN",    // 0b11001
"UNKNOWN",    // 0b11010
"UNDEFINED",  // 0b11011
"UNKNOWN",    // 0b11100
"UNKNOWN",    // 0b11101
"UNKNOWN",    // 0b11110
"SYS",        // 0b11111
};

void fill_pipe(arm7tdmi_t* state) {
    if (state->cpsr.thumb) {
        state->pipeline[0] = state->read_half(state->pc, ACCESS_NONSEQUENTIAL);
        state->pc += 2;
        state->pipeline[1] = state->read_half(state->pc, ACCESS_SEQUENTIAL);

        logdebug("[THM] Filling the instruction pipeline: 0x%08X = 0x%04X / 0x%08X = 0x%04X",
                 state->pc - 2,
                 state->pipeline[0],
                 state->pc,
                 state->pipeline[1])
    } else {
        state->pipeline[0] = state->read_word(state->pc, ACCESS_NONSEQUENTIAL);
        state->pc += 4;
        state->pipeline[1] = state->read_word(state->pc, ACCESS_SEQUENTIAL);

        logdebug("[ARM] Filling the instruction pipeline: 0x%08X = 0x%08X / 0x%08X = 0x%08X",
                 state->pc - 4,
                 state->pipeline[0],
                 state->pc,
                 state->pipeline[1])
    }
}

// Sets the PC, fills the pipeline, and allows mode switching.
void set_pc(arm7tdmi_t* state, word new_pc) {
    if (new_pc & 1u) {
        state->cpsr.thumb = true;
        new_pc &= ~1u; // Unset thumb bit as it's a flag, not really part of the address
    } else if (state->cpsr.thumb && (new_pc & 1u) == 0u) {
        state->cpsr.thumb = false;
    }

    if (state->cpsr.thumb && new_pc % 2 != 0) {
        logfatal("Attempted to jump in THUMB mode to a non-half aligned address 0x%08X!", new_pc)
    } else if (!state->cpsr.thumb && new_pc % 4 != 0) {
        logwarn("Attempted to jump in ARM mode to a non-word aligned address 0x%08X!", new_pc)
        new_pc &= 0xFFFFFFFC; // Correct alignment
        logwarn("Corrected it to 0x%08X!", new_pc)
    }

    state->pc = new_pc;
    fill_pipe(state);
}

// Replace me with actual idle cycle handling
void temp_noop(int cycles) { }

arm7tdmi_t* init_arm7tdmi(byte (*read_byte)(word, access_type_t),
                          half (*read_half)(word, access_type_t),
                          word (*read_word)(word, access_type_t),
                          void (*write_byte)(word, byte, access_type_t),
                          void (*write_half)(word, half, access_type_t),
                          void (*write_word)(word, word, access_type_t)) {
    fill_arm_lut(&arm_lut);
    fill_thm_lut(&thm_lut);
    arm7tdmi_t* state = malloc(sizeof(arm7tdmi_t));

    state->read_byte  = read_byte;
    state->read_half  = read_half;
    state->read_word  = read_word;
    state->write_byte = write_byte;
    state->write_half = write_half;
    state->write_word = write_word;

    state->cpu_idle = &temp_noop;

    state->pc       = 0x00000000;
    state->sp       = 0x03007F00;
    state->lr       = 0x08000000;
    state->cpsr.raw = 0x0000005F;

    for (int r = 0; r < 13; r++) {
        state->r[r] = 0;
    }

    state->sp_fiq = 0x00000000;
    state->sp_svc = 0x00000000;
    state->sp_abt = 0x00000000;
    state->sp_irq = 0x00000000;
    state->sp_und = 0x00000000;

    state->lr_fiq = 0x00000000;
    state->lr_svc = 0x00000000;
    state->lr_abt = 0x00000000;
    state->lr_irq = 0x00000000;
    state->lr_und = 0x00000000;

    state->highreg_fiq[0] = 0;
    state->highreg_fiq[1] = 0;
    state->highreg_fiq[2] = 0;
    state->highreg_fiq[3] = 0;
    state->highreg_fiq[4] = 0;

    state->irq = false;
    state->halt = false;

    return state;
}

// EQ, NE, CS, CC, MI, PL, VS, VC, HI, LS, GE, LT, GT, LE, AL, NV
INLINE bool check_cond(arm7tdmi_t* state, arminstr_t* instr) {
    bool passed = false;
    switch (instr->parsed.cond) {
        case EQ:
            passed = state->cpsr.Z == 1;
            break;
        case NE:
            passed = state->cpsr.Z == 0;
            break;
        case CS:
            passed = state->cpsr.C == 1;
            break;
        case CC:
            passed = state->cpsr.C == 0;
            break;
        case MI:
            passed = state->cpsr.N == 1;
            break;
        case PL:
            passed = state->cpsr.N == 0;
            break;
        case VS:
            passed = state->cpsr.V == 1;
            break;
        case VC:
            passed = state->cpsr.V == 0;
            break;
        case HI:
            passed = state->cpsr.C == 1 && state->cpsr.Z == 0;
            break;
        case LS:
            passed = state->cpsr.C == 0 || state->cpsr.Z == 1;
            break;
        case GE:
            passed = (!state->cpsr.N == !state->cpsr.V);
            break;
        case LT:
            passed = (!state->cpsr.N != !state->cpsr.V);
            break;
        case GT:
            passed = (!state->cpsr.Z && !state->cpsr.N == !state->cpsr.V);
            break;
        case LE:
            passed = (state->cpsr.Z || !state->cpsr.N != !state->cpsr.V);
            break;
        case AL:
            passed = true;
            break;
        case NV:
            passed = false;
            break;
        default:
            logfatal("Unimplemented COND: %d", instr->parsed.cond)
    }
    return passed;
}

INLINE arminstr_t next_arm_instr(arm7tdmi_t* state) {
    arminstr_t instr;
    instr.raw = state->pipeline[0];
    state->pipeline[0] = state->pipeline[1];
    state->pc += 4;
    state->pipeline[1] = state->read_word(state->pc, ACCESS_NONSEQUENTIAL);

    return instr;
}

INLINE thumbinstr_t next_thumb_instr(arm7tdmi_t* state) {
    thumbinstr_t instr;
    instr.raw = state->pipeline[0];
    state->pipeline[0] = state->pipeline[1];
    state->pc += 2;
    state->pipeline[1] = state->read_half(state->pc, ACCESS_NONSEQUENTIAL);

    return instr;
}

#define cpsrflag(f, c) (f == 1?c:"-")

INLINE int arm_mode_step(arm7tdmi_t* state, arminstr_t* instr) {
    logdebug("cond: %d", instr->parsed.cond)
    if (check_cond(state, instr)) {
        arm_lut[hash_arm_instr(instr->raw)](state, instr);
    }
    else { // Cond told us not to execute this instruction
        logdebug("Skipping instr because cond %d was not met.", instr->parsed.cond)
        state->this_step_ticks += 1;
    }
    return state->this_step_ticks;
}

INLINE int thumb_mode_step(arm7tdmi_t* state, thumbinstr_t* instr) {
    thm_lut[hash_thm_instr(instr->raw)](state, instr);
    return state->this_step_ticks;
}

void handle_irq(arm7tdmi_t* state) {
    logwarn("IRQ!")
    status_register_t cpsr = state->cpsr;
    state->halt = false;
    state->cpsr.mode = MODE_IRQ;
    set_spsr(state, cpsr.raw);
    state->cpsr.thumb = 0;
    state->cpsr.disable_irq = 1;
    state->lr_irq = state->pc - (cpsr.thumb ? 2 : 4) + 4;
    set_pc(state, 0x18); // IRQ handler
}
int arm7tdmi_step(arm7tdmi_t* state) {
    if (state->irq && !state->cpsr.disable_irq) {
        handle_irq(state);
    }

    state->this_step_ticks = 0;
    logdebug("r0:  %08X   r1: %08X   r2: %08X   r3: %08X", get_register(state, 0), get_register(state, 1), get_register(state, 2), get_register(state, 3))
    logdebug("r4:  %08X   r5: %08X   r6: %08X   r7: %08X", get_register(state, 4), get_register(state, 5), get_register(state, 6), get_register(state, 7))
    logdebug("r8:  %08X   r9: %08X  r10: %08X  r11: %08X", get_register(state, 8), get_register(state, 9), get_register(state, 10), get_register(state, 11))
    logdebug("r12: %08X  r13: %08X  r14: %08X  r15: %08X", get_register(state, 12), get_register(state, 13), get_register(state, 14), get_register(state, 15))
    logdebug("cpsr: %08X [%s%s%s%s%s%s%s]", state->cpsr.raw, cpsrflag(state->cpsr.N, "N"), cpsrflag(state->cpsr.Z, "Z"),
             cpsrflag(state->cpsr.C, "C"), cpsrflag(state->cpsr.V, "V"), cpsrflag(state->cpsr.disable_irq, "I"),
             cpsrflag(state->cpsr.disable_fiq, "F"), cpsrflag(state->cpsr.thumb, "T"))

     int cycles;
     if (state->cpsr.thumb) {
         thumbinstr_t instr = next_thumb_instr(state);
         state->instr = instr.raw;
         word adjusted_pc = state->pc - 4;
         if (gba_log_verbosity >= LOG_VERBOSITY_INFO) {
             loginfo("[THM] [%s] 0x%08X: [    0x%04X]", MODE_NAMES[state->cpsr.mode], adjusted_pc, instr.raw)
         }
         cycles = thumb_mode_step(state, &instr);
     } else {
         arminstr_t instr = next_arm_instr(state);
         state->instr = instr.raw;
         word adjusted_pc = state->pc - 8;
         if (gba_log_verbosity >= LOG_VERBOSITY_INFO) {
             loginfo("[ARM] [%s] 0x%08X: [0x%08X]", MODE_NAMES[state->cpsr.mode], adjusted_pc, instr.raw)
         }
         cycles = arm_mode_step(state, &instr);
     }

    // Assume a step of 0 cycles is an unknown number of cycles. Consider it 1 cycle
     return cycles == 0 ? 1 : cycles;
}

status_register_t* get_psr(arm7tdmi_t* state) {
    return &state->cpsr;
}

void set_psr(arm7tdmi_t* state, word value) {
    state->cpsr.raw = value;
}

status_register_t* get_spsr(arm7tdmi_t* state) {
    switch (state->cpsr.mode) {
        case MODE_FIQ:
            return &state->spsr_fiq;
        case MODE_SUPERVISOR:
            return &state->spsr_svc;
        case MODE_ABORT:
            return &state->spsr_abt;
        case MODE_IRQ:
            return &state->spsr_irq;
        case MODE_UNDEFINED:
            return &state->spsr_und;
        default:
            return &state->spsr;
    }
}

void set_spsr(arm7tdmi_t* state, word value) {
    status_register_t* spsr = get_spsr(state);
    spsr->raw = value;
}

void skip_bios(arm7tdmi_t* state) {
    set_register(state, 0, 0x08000000);
    set_register(state, 1, 0x000000EA);
    set_register(state, 2, 0x00000000);
    set_register(state, 3, 0x00000000);
    set_register(state, 4, 0x00000000);
    set_register(state, 5, 0x00000000);
    set_register(state, 6, 0x00000000);
    set_register(state, 7, 0x00000000);
    set_register(state, 8, 0x00000000);
    set_register(state, 9, 0x00000000);
    set_register(state, 10, 0x00000000);
    set_register(state, 11, 0x00000000);
    set_register(state, 12, 0x00000000);
    set_register(state, REG_SP, 0x03007F00);
    state->sp_irq = 0x3007FA0;
    state->sp_svc = 0x3007FE0;
    set_register(state, REG_LR, 0x00000000);

    set_pc(state, 0x08000000);
    state->cpsr.raw = 0x6000001F;
}
