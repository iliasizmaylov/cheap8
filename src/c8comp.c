/**
 * Cheap-8: a chip-8 emulator
 * 
 * File: c8comp.c
 * License: DWYW - "Do Whatever You Want"
 * 
 * Functions and data structures for a cheap8 compiler
 */

#include "c8comp.h"

/**
 * Although maximum possible amount of instructions is definitely less
 * than MEMORY_RANGE_PROGRAM_MAX I decided to declare the array of
 * disassembled instructions exactly the size of 0 to max program memory 
 * so that it's easy to implement sort of a "goto addr" functionality
 *
 * Therefore, since each instruction is 2 bytes, this array will be traversed
 * via get/find/etc. function and will be static so as not be accessed directly 
 * fron the outside
 */
static Instruction *g_disasmem[MEMORY_RANGE_PROGRAM_MAX + 1];
static BYTE g_disas_init = 0;

void (* const optostrfn[OPCODE_COUNT])(Instruction*) = {
	optostr_CALL_MCR,
	optostr_CLEAR_SCREEN,
	optostr_RETURN,
	optostr_JUMP,
	optostr_CALL_SUBR,
	optostr_SKIP_EQ,
	optostr_SKIP_NEQ,
	optostr_SKIP_EQ_REG,
	optostr_SET_CONST,
	optostr_ADD_CONST,
	optostr_SET_REG,
	optostr_OR_REG,
	optostr_AND_REG,
	optostr_XOR_REG,
	optostr_ADD_REG,
	optostr_SUB_REG,
	optostr_SHRIGHT_1,
	optostr_REV_SUB_REG,
	optostr_SHLEFT_1,
	optostr_SKIP_NEQ_REG,
	optostr_SET_IDX,
	optostr_JUMP_FROM_V0,
	optostr_SET_RANDOM,
	optostr_DRAW,
	optostr_SKIP_KPRESS,
	optostr_SKIP_NKPRESS,
	optostr_SAVE_DELAY,
	optostr_WAIT_KEY,
	optostr_SET_DELAY,
	optostr_SET_SOUND,
	optostr_ADD_IDX,
	optostr_SET_IDX_SPRITE,
	optostr_SET_BCD,
	optostr_DUMP_REGS,
	optostr_LOAD_REGS
};

/**
 * An invalid instruction token
 * Is returned by various get instruction function variants when there is an error
 */
static const Instruction g_InvalidInstruction = {
    .op = NULL,
    .addr = MEMORY_SIZE, .raw = MEMORY_SIZE,
    .xParam = 0x00, .yParam = 0x00, .nParam = 0x00
};

#define ADDRESS_WITHIN_PROGRAM_MEMORY(addr) \
    (addr >= MEMORY_RANGE_PROGRAM_MIN && addr <= MEMORY_RANGE_PROGRAM_MAX)

/** alignAddress
 *
 * @param: inputAddress
 *  Memory address to be aligned
 * @description:
 *  Aligns given memory address to a beginning of a 2 byte instruction
 *  within it's vicinity
 *  If inputAddress is not within program memory range - returns 0
 */
static inline WORD alignAddress(WORD inputAddress) {
    if (!ADDRESS_WITHIN_PROGRAM_MEMORY(inputAddress))
        return 0;

    WORD addrNoOffset = inputAddress - MEMORY_RANGE_PROGRAM_MIN;
    WORD result = addrNoOffset % OPCODE_SIZE == 0 ? inputAddress : inputAddress - 1;
    return result;
}

/** getInstructionAt
 *
 * @param: addr
 *  Address at which to get instruction from
 * @description:
 *  Return an Instruction struct representing an instruction description
 *  that is at address addr
 *  Returns g_InvalidInstruction if specified address is wrong
 */
const Instruction *getInstructionAt(WORD addr) {
    addr = alignAddress(addr);

    if (addr == 0)
        return &g_InvalidInstruction;

    return g_disasmem[addr];
};

static void setAsmstr(Instruction *instr) {
    if (instr->op == NULL)
        return;

    char xstr[6] = "";
    char ystr[6] = "";
    char nstr[8] = "";

    if (instr->xParam != PARAMETER_UNUSED)
        sprintf(xstr, " V%u", instr->xParam);
    if (instr->yParam != PARAMETER_UNUSED)
        sprintf(ystr, " V%u", instr->yParam);
    if (instr->nParam != PARAMETER_UNUSED)
        sprintf(nstr, " 0x%03X", instr->nParam);

   sprintf(instr->asmstr, "%-5s%s%s%s", instr->op->opasm, xstr, ystr, nstr);
}

/** rawToInstruction
 *
 * @param: raw
 *  Raw representation (2 bytes) of an instruction
 * @param: out
 *  Pointer to Instruction struct that is to be populated from 
 *  raw data
 * @description:
 *  Populates out pointer with data that describes a current instruction
 *  in a more human readable format
 */
VM_RESULT rawToInstruction(WORD raw, Instruction *out) {
    BYTE idx = getOpcodeIndex(raw);
    const Opcode *opcode = &OPCODES[idx];

	VM_ASSERT(opcode == NULL);

	WORD xParam = opcode->xParamMask != PARAMETER_UNUSED ? (raw & opcode->xParamMask) >> 8 : PARAMETER_UNUSED;
	WORD yParam = opcode->yParamMask != PARAMETER_UNUSED ? (raw & opcode->yParamMask) >> 4 : PARAMETER_UNUSED;
	WORD nParam = opcode->nParamMask != PARAMETER_UNUSED ? (raw & opcode->nParamMask) : PARAMETER_UNUSED;

    out->op = opcode;
    out->opidx = idx;
    out->raw = raw;
    out->xParam = xParam;
    out->yParam = yParam;
    out->nParam = nParam;

    setAsmstr(out);
    optostrfn[idx](out);

    return VM_RESULT_SUCCESS;
}

/** disassemble
 *
 * @param: core
 *  Pointer to C8core representing chip-8 system core
 * @description:
 *  Traverses chip-8 memory and disassembles all instructions from memory
 *  Saves the result in a global static array g_disasmem
 */
VM_RESULT disassemble(const C8core *core) {
    VM_ASSERT(core == NULL);

    for (int i = 0; i <= MEMORY_RANGE_PROGRAM_MAX; i++) {
        if (alignAddress(i) == 0 || alignAddress(i) != i) {
            g_disasmem[i] = NULL;
        } else {
            g_disasmem[i] = malloc(sizeof(Instruction));
            g_disasmem[i]->addr = i;
            rawToInstruction(GET_WORD(core->memory[i], core->memory[i] + 1), g_disasmem[i]);
        }
    }

    g_disas_init = 1;
    return VM_RESULT_SUCCESS;
}

/** destroyDisassembler
 *
 * @descrption:
 *  Frees memory allocated for g_disasmem (if it has been allocated)
 */
VM_RESULT destroyDisassembler() {
    if (!g_disas_init)
        return VM_RESULT_SUCCESS;

    for (int i = MEMORY_RANGE_PROGRAM_MIN; i <= MEMORY_RANGE_PROGRAM_MAX; i += OPCODE_SIZE) {
        free(g_disasmem[i]); 
    }

    return VM_RESULT_SUCCESS;
}

/* =================== OPCODE TO STRING FUNCTIONS =================== */

void optostr_CALL_MCR(Instruction *instr) {
    sprintf(instr->readable, "NONE / CALL MCR");
}

void optostr_CLEAR_SCREEN(Instruction *instr) {
    sprintf(instr->readable, "clear");
}

void optostr_RETURN(Instruction *instr) {
    sprintf(instr->readable, "return");
}

void optostr_JUMP(Instruction *instr) {
    sprintf(instr->readable, "jump 0x%03X", instr->nParam);
}

void optostr_CALL_SUBR(Instruction *instr) {
    sprintf(instr->readable, "call 0x%03X", instr->nParam);
}

void optostr_SKIP_EQ(Instruction *instr) {
    sprintf(instr->readable, "if V%u != %u then", instr->xParam, instr->nParam);
}

void optostr_SKIP_NEQ(Instruction *instr) {
    sprintf(instr->readable, "if V%u == %u then", instr->xParam, instr->nParam);
}

void optostr_SKIP_EQ_REG(Instruction *instr) {
    sprintf(instr->readable, "if V%u != V%u then", instr->xParam, instr->yParam);
}

void optostr_SET_CONST(Instruction *instr) {
    sprintf(instr->readable, "V%u := %u", instr->xParam, instr->nParam);
}

void optostr_ADD_CONST(Instruction *instr) {
    sprintf(instr->readable, "V%u += %u", instr->xParam, instr->nParam);
}

void optostr_SET_REG(Instruction *instr) {
    sprintf(instr->readable, "V%u := V%u", instr->xParam, instr->yParam);
}

void optostr_OR_REG(Instruction *instr) {
    sprintf(instr->readable, "V%u |= V%u", instr->xParam, instr->yParam);
}

void optostr_AND_REG(Instruction *instr) {
    sprintf(instr->readable, "V%u &= V%u", instr->xParam, instr->yParam);
}

void optostr_XOR_REG(Instruction *instr) {
    sprintf(instr->readable, "V%u ^= V%u", instr->xParam, instr->yParam);
}

void optostr_ADD_REG(Instruction *instr) {
    sprintf(instr->readable, "V%u += V%u", instr->xParam, instr->yParam);
}

void optostr_SUB_REG(Instruction *instr) {
    sprintf(instr->readable, "V%u -= V%u", instr->xParam, instr->yParam);
}

void optostr_SHRIGHT_1(Instruction *instr) {
    sprintf(instr->readable, "V%u >>= V%u", instr->xParam, instr->yParam);
}

void optostr_REV_SUB_REG(Instruction *instr) {
    sprintf(instr->readable, "V%u =- V%u", instr->xParam, instr->yParam);
}

void optostr_SHLEFT_1(Instruction *instr) {
    sprintf(instr->readable, "V%u <<= V%u", instr->xParam, instr->yParam);
}

void optostr_SKIP_NEQ_REG(Instruction *instr) {
    sprintf(instr->readable, "if V%u == V%u then", instr->xParam, instr->yParam);
}

void optostr_SET_IDX(Instruction *instr) {
    sprintf(instr->readable, "i := 0x%03X", instr->nParam);
}

void optostr_JUMP_FROM_V0(Instruction *instr) {
    sprintf(instr->readable, "jump0 0x%03X", instr->nParam);
}

void optostr_SET_RANDOM(Instruction *instr) {
    sprintf(instr->readable, "V%u := (random & %u)", instr->xParam, instr->nParam);
}

void optostr_DRAW(Instruction *instr) {
    sprintf(instr->readable, "sprite V%u V%u %u", instr->xParam, instr->yParam, instr->nParam);
}

void optostr_SKIP_KPRESS(Instruction *instr) {
    sprintf(instr->readable, "if !key(V%u) then", instr->xParam);
}

void optostr_SKIP_NKPRESS(Instruction *instr) {
    sprintf(instr->readable, "if key(V%u) then", instr->xParam);
}

void optostr_SAVE_DELAY(Instruction *instr) {
    sprintf(instr->readable, "V%u := delay", instr->xParam);
}

void optostr_WAIT_KEY(Instruction *instr) {
    sprintf(instr->readable, "V%u := keypress", instr->xParam);
}

void optostr_SET_DELAY(Instruction *instr) {
    sprintf(instr->readable, "delay := V%u", instr->xParam);
}

void optostr_SET_SOUND(Instruction *instr) {
    sprintf(instr->readable, "buzzer := V%u", instr->xParam);
}

void optostr_ADD_IDX(Instruction *instr) {
    sprintf(instr->readable, "i += V%u", instr->xParam);
}

void optostr_SET_IDX_SPRITE(Instruction *instr) {
    sprintf(instr->readable, "i := sprite(V%u)", instr->xParam);
}

void optostr_SET_BCD(Instruction *instr) {
    sprintf(instr->readable, "bcd V%u", instr->xParam);
}

void optostr_DUMP_REGS(Instruction *instr) {
    sprintf(instr->readable, "save V0-V%u", instr->xParam);
}

void optostr_LOAD_REGS(Instruction *instr) {
    sprintf(instr->readable, "load V0-V%u", instr->xParam);
}
