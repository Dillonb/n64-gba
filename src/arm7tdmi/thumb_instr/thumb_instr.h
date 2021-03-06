#ifndef THUMB_INSTR_H
#define THUMB_INSTR_H
#include <stdbool.h>
#include "../../util.h"
#include "../arm_instr/arm_instr.h"

typedef enum thumb_instr_type {
    MOVE_SHIFTED_REGISTER,
    ADD_SUBTRACT,
    IMMEDIATE_OPERATIONS,
    ALU_OPERATIONS,
    HIGH_REGISTER_OPERATIONS,
    PC_RELATIVE_LOAD,
    LOAD_STORE_RO,
    LOAD_STORE_BYTE_HALFWORD, // Sign extended
    LOAD_STORE_IO,
    LOAD_STORE_HALFWORD,
    SP_RELATIVE_LOAD_STORE,
    LOAD_ADDRESS,
    ADD_OFFSET_TO_STACK_POINTER,
    PUSH_POP_REGISTERS,
    MULTIPLE_LOAD_STORE,
    CONDITIONAL_BRANCH,
    THUMB_SOFTWARE_INTERRUPT,
    UNCONDITIONAL_BRANCH,
    LONG_BRANCH_LINK,
    THUMB_UNDEFINED
} thumb_instr_type_t;

typedef struct move_shifted_register {
#ifdef GBA_LITTLE_ENDIAN
    unsigned rd:3;
    unsigned rs:3;
    unsigned offset:5;
    unsigned opcode:2;
    unsigned:3;
#else
    unsigned:3;
    unsigned opcode:2;
    unsigned offset:5;
    unsigned rs:3;
    unsigned rd:3;
#endif
} move_shifted_register_t;

typedef struct add_subtract {
#ifdef GBA_LITTLE_ENDIAN
    unsigned rd:3;
    unsigned rs:3;
    unsigned rn_or_offset:3;
    bool op:1;
    bool i:1;
    unsigned:5;
#else
    unsigned:5;
    bool i:1;
    bool op:1;
    unsigned rn_or_offset:3;
    unsigned rs:3;
    unsigned rd:3;
#endif
} add_subtract_t;

typedef struct immediate_operations {
#ifdef GBA_LITTLE_ENDIAN
    unsigned offset:8;
    unsigned rd:3;
    unsigned opcode:2;
    unsigned:3;
#else
    unsigned:3;
    unsigned opcode:2;
    unsigned rd:3;
    unsigned offset:8;
#endif
} immediate_operations_t;

typedef struct alu_operations {
#ifdef GBA_LITTLE_ENDIAN
    unsigned rd:3;
    unsigned rs:3;
    unsigned opcode:4;
    unsigned:6;
#else
    unsigned:6;
    unsigned opcode:4;
    unsigned rs:3;
    unsigned rd:3;
#endif
} alu_operations_t;

typedef struct high_register_operations {
#ifdef GBA_LITTLE_ENDIAN
    unsigned rdhd:3;
    unsigned rshs:3;
    bool h2:1;
    bool h1:1;
    unsigned opcode:2;
    unsigned:6;
#else
    unsigned:6;
    unsigned opcode:2;
    bool h1:1;
    bool h2:1;
    unsigned rshs:3;
    unsigned rdhd:3;
#endif
} high_register_operations_t;

typedef struct pc_relative_load {
#ifdef GBA_LITTLE_ENDIAN
    unsigned word8:8;
    unsigned rd:3;
    unsigned:5;
#else
    unsigned:5;
    unsigned rd:3;
    unsigned word8:8;
#endif
} pc_relative_load_t;

typedef struct load_store_ro {
#ifdef GBA_LITTLE_ENDIAN
    unsigned rd:3;
    unsigned rb:3;
    unsigned ro:3;
    unsigned:1;
    bool b:1;
    bool l:1;
    unsigned:4;
#else
    unsigned:4;
    bool l:1;
    bool b:1;
    unsigned:1;
    unsigned ro:3;
    unsigned rb:3;
    unsigned rd:3;
#endif
} load_store_ro_t;

typedef struct load_store_byte_halfword {
#ifdef GBA_LITTLE_ENDIAN
    unsigned rd:3;
    unsigned rb:3;
    unsigned ro:3;
    unsigned:1;
    bool s:1;
    bool h:1;
    unsigned:4;
#else
    unsigned:4;
    bool h:1;
    bool s:1;
    unsigned:1;
    unsigned ro:3;
    unsigned rb:3;
    unsigned rd:3;
#endif
} load_store_byte_halfword_t;

typedef struct load_store_io {
#ifdef GBA_LITTLE_ENDIAN
    unsigned rd:3;
    unsigned rb:3;
    unsigned offset:5;
    bool l:1;
    bool b:1;
    unsigned:3;
#else
    unsigned:3;
    bool b:1;
    bool l:1;
    unsigned offset:5;
    unsigned rb:3;
    unsigned rd:3;
#endif
} load_store_io_t;

typedef struct load_store_halfword {
#ifdef GBA_LITTLE_ENDIAN
    unsigned rd:3;
    unsigned rb:3;
    unsigned offset:5;
    bool l:1;
    unsigned:4;
#else
    unsigned:4;
    bool l:1;
    unsigned offset:5;
    unsigned rb:3;
    unsigned rd:3;
#endif
} load_store_halfword_t;

typedef struct sp_relative_load_store {
#ifdef GBA_LITTLE_ENDIAN
    unsigned word8:8;
    unsigned rd:3;
    bool l:1;
    unsigned:4;
#else
    unsigned:4;
    bool l:1;
    unsigned rd:3;
    unsigned word8:8;
#endif
} sp_relative_load_store_t;

typedef struct load_address {
#ifdef GBA_LITTLE_ENDIAN
    unsigned word8:8;
    unsigned rd:3;
    bool sp:1;
    unsigned:4;
#else
    unsigned:4;
    bool sp:1;
    unsigned rd:3;
    unsigned word8:8;
#endif
} load_address_t;

typedef struct add_offset_to_stack_pointer {
#ifdef GBA_LITTLE_ENDIAN
    unsigned sword7:7;
    bool s:1;
    unsigned:8;
#else
    unsigned:8;
    bool s:1;
    unsigned sword7:7;
#endif
} add_offset_to_stack_pointer_t;

typedef struct push_pop_registers {
#ifdef GBA_LITTLE_ENDIAN
    unsigned rlist:8;
    bool r:1;
    unsigned:2;
    bool l:1;
    unsigned:4;
#else
    unsigned:4;
    bool l:1;
    unsigned:2;
    bool r:1;
    unsigned rlist:8;
#endif
} push_pop_registers_t;

typedef struct multiple_load_store {
#ifdef GBA_LITTLE_ENDIAN
    unsigned rlist:8;
    unsigned rb:3;
    bool l:1;
    unsigned:4;
#else
    unsigned:4;
    bool l:1;
    unsigned rb:3;
    unsigned rlist:8;
#endif
} multiple_load_store_t;

typedef struct conditional_branch {
#ifdef GBA_LITTLE_ENDIAN
    unsigned soffset:8;
    arm_cond_t cond:4;
    unsigned:4;
#else
    unsigned:4;
    arm_cond_t cond:4;
    unsigned soffset:8;
#endif
} conditional_branch_t;

typedef struct thumb_software_interrupt {
#ifdef GBA_LITTLE_ENDIAN
    unsigned value:8;
    unsigned:8;
#else
    unsigned:8;
    unsigned value:8;
#endif
} thumb_software_interrupt_t;

typedef struct unconditional_branch {
#ifdef GBA_LITTLE_ENDIAN
    unsigned offset:11;
    unsigned:5;
#else
    unsigned:5;
    unsigned offset:11;
#endif
} unconditional_branch_t;

typedef struct long_branch_link {
#ifdef GBA_LITTLE_ENDIAN
    unsigned offset:11;
    bool h:1;
    unsigned:4;
#else
    unsigned:4;
    bool h:1;
    unsigned offset:11;
#endif
} long_branch_link_t;

typedef struct thumb_undefined {
#ifdef GBA_LITTLE_ENDIAN
    unsigned todo:16;
#else
    unsigned todo:16;
#endif
} thumb_undefined_t;

typedef union thumbinstr {
    move_shifted_register_t MOVE_SHIFTED_REGISTER;
    add_subtract_t ADD_SUBTRACT;
    immediate_operations_t IMMEDIATE_OPERATIONS;
    alu_operations_t ALU_OPERATIONS;
    high_register_operations_t HIGH_REGISTER_OPERATIONS;
    pc_relative_load_t PC_RELATIVE_LOAD;
    load_store_ro_t LOAD_STORE_RO;
    load_store_byte_halfword_t LOAD_STORE_BYTE_HALFWORD;
    load_store_io_t LOAD_STORE_IO;
    load_store_halfword_t LOAD_STORE_HALFWORD;
    sp_relative_load_store_t SP_RELATIVE_LOAD_STORE;
    load_address_t LOAD_ADDRESS;
    add_offset_to_stack_pointer_t ADD_OFFSET_TO_STACK_POINTER;
    push_pop_registers_t PUSH_POP_REGISTERS;
    multiple_load_store_t MULTIPLE_LOAD_STORE;
    conditional_branch_t CONDITIONAL_BRANCH;
    thumb_software_interrupt_t THUMB_SOFTWARE_INTERRUPT;
    unconditional_branch_t UNCONDITIONAL_BRANCH;
    long_branch_link_t LONG_BRANCH_LINK;
    thumb_undefined_t THUMB_UNDEFINED;
    half raw;
} thumbinstr_t;

#define hash_thm_instr(instr) ((instr) >> 6)
typedef void(*thminstr_handler_t)(arm7tdmi_t*, thumbinstr_t*);
void fill_thm_lut(thminstr_handler_t (*lut)[1024]);
#endif
