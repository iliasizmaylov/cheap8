/**
 * Cheap-8: a chip-8 emulator
 * 
 * File: c8comp.h
 * License: DWYW - "Do Whatever You Want"
 * 
 * Functions and data structures for a cheap8 compiler
 */

#ifndef _C8COMP_H_
#define _C8COMP_H_

#include "opcodes.h"

#define MAX_INSTRUCTION_COUNT \
    ((MEMORY_RANGE_PROGRAM_MAX - MEMORY_RANGE_PROGRAM_MIN) / 2)

#define MAX_INSTR_READABLE_LENGTH   1 << 5
typedef struct _Instruction {
    const Opcode *op;
    BYTE opidx;
    WORD addr;
    WORD raw;
    WORD xParam;
    WORD yParam;
    WORD nParam;
    char readable[MAX_INSTR_READABLE_LENGTH];
} Instruction;

static inline WORD alignAddress(WORD inputAddress);

const Instruction *getInstructionAt(WORD addr);

VM_RESULT rawToInstruction(WORD raw, Instruction *out);

VM_RESULT disassemble(const C8core *core);

VM_RESULT destroyDisassembler();

/* =================== OPCODE TO STRING FUNCTIONS =================== */

void optostr_CALL_MCR(Instruction *instr);
void optostr_CLEAR_SCREEN(Instruction *instr);
void optostr_RETURN(Instruction *instr);
void optostr_JUMP(Instruction *instr);
void optostr_CALL_SUBR(Instruction *instr);
void optostr_SKIP_EQ(Instruction *instr);
void optostr_SKIP_NEQ(Instruction *instr);
void optostr_SKIP_EQ_REG(Instruction *instr);
void optostr_SET_CONST(Instruction *instr);
void optostr_ADD_CONST(Instruction *instr);
void optostr_SET_REG(Instruction *instr);
void optostr_OR_REG(Instruction *instr);
void optostr_AND_REG(Instruction *instr);
void optostr_XOR_REG(Instruction *instr);
void optostr_ADD_REG(Instruction *instr);
void optostr_SUB_REG(Instruction *instr);
void optostr_SHRIGHT_1(Instruction *instr);
void optostr_REV_SUB_REG(Instruction *instr);
void optostr_SHLEFT_1(Instruction *instr);
void optostr_SKIP_NEQ_REG(Instruction *instr);
void optostr_SET_IDX(Instruction *instr);
void optostr_JUMP_FROM_V0(Instruction *instr);
void optostr_SET_RANDOM(Instruction *instr);
void optostr_DRAW(Instruction *instr);
void optostr_SKIP_KPRESS(Instruction *instr);
void optostr_SKIP_NKPRESS(Instruction *instr);
void optostr_SAVE_DELAY(Instruction *instr);
void optostr_WAIT_KEY(Instruction *instr);
void optostr_SET_DELAY(Instruction *instr);
void optostr_SET_SOUND(Instruction *instr);
void optostr_ADD_IDX(Instruction *instr);
void optostr_SET_IDX_SPRITE(Instruction *instr);
void optostr_SET_BCD(Instruction *instr);
void optostr_DUMP_REGS(Instruction *instr);
void optostr_LOAD_REGS(Instruction *instr);

#endif  /* _C8COMP_H_ */
