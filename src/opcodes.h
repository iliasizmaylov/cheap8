/**
 * Cheap-8: a chip-8 emulator
 * 
 * File: opcodes.h
 * License: DWYW - "Do Whatever You Want"
 * 
 * Declaration of all opcode handlers and opcode codes (no pun intended)
 * and also an Opcode data structure and a couple of constant values
 */

#ifndef _OPCODES_H_
#define _OPCODES_H_

#include "c8core.h"

#define OPCODE_SIZE			sizeof(WORD)
#define PARAMETER_UNUSED	0xFFFF

// Function pointer type for an opcode handler
// which is a type of function that would emulate each opcode
typedef void (*OPCODE_HANDLER)(C8core*, BYTE, BYTE, WORD);

// Data structure holding all data related to a single opcode
typedef struct _Opcode {
	WORD opcodeMask;        // Bitmask that can be used to identify each single opcode
	WORD opcodeId;          // Id of an opcode
	WORD xParamMask;        // Bitmask that's used to extract the X (register 1 number) parameter
	WORD yParamMask;        // Bitmask that's used to extract the Y (register 2 number) parameter
	WORD nParamMask;        // Bitmask that's used to extract the N (constant) parameter
	OPCODE_HANDLER handler; // Function pointer to an opcode handling function

    // Just a bunch of text describing an opcode briefly
    // Used in a debugger
    const char *description;
} Opcode;

typedef enum {
	OP_CALL_MCR = 0,        // 0NNN; Calls machine code routine NNN
	OP_CLEAR_SCREEN,        // 00E0; Clear screen
	OP_RETURN,              // 00EE; Return from subroutine
	OP_JUMP,                // 1NNN; Jump to address NNN
	OP_CALL_SUBR,           // 2NNN; Call subroutine at address NNN
	OP_SKIP_EQ,             // 3XNN; Skip next instruction if VX == NN
	OP_SKIP_NEQ,            // 4XNN; Skip next instruction if VX != NN
	OP_SKIP_EQ_REG,         // 5XY0; Skip next instruction if VX == VY
	OP_SET_CONST,           // 6XNN; Set VX to NN
	OP_ADD_CONST,           // 7XNN; Adds NN to VX
	OP_SET_REG,             // 8XY0; Sets VX to VY
	OP_OR_REG,              // 8XY1; Sets VX |= VY
	OP_AND_REG,             // 8XY2; Sets VX &= VY
	OP_XOR_REG,             // 8XY3; Sets VX ^= VY
	OP_ADD_REG,             // 8XY4; Adds VY to VX
	OP_SUB_REG,             // 8XY5; Subtracts VY from VX
	OP_SHRIGHT_1,           // 8XY6; Stores LSB of VX in VF and shifts VX right by 1
	OP_REV_SUB_REG,         // 8XY7; Sets VX = VY - VX; 
	OP_SHLEFT_1,            // 8XYE; Stores MSB of VX in VF and shifts VX left by 1
	OP_SKIP_NEQ_REG,        // 9XY0; Skip instruction if VX != VY
	OP_SET_IDX,             // ANNN; Sets I = NNN
	OP_JUMP_FROM_V0,        // BNNN; Jumps to address V0 + NNN
	OP_SET_RANDOM,          // CXNN; Sets VX = rand() & NN
	OP_DRAW,                // DXYN; Draws a sprite at coordinate VX, VY with a width of 8 and height of N
	OP_SKIP_KPRESS,         // EX9E; Skip next insturction if a key stored in VX is pressed
	OP_SKIP_NKPRESS,        // EXA1; Skip next insturction if a key stored in VX is NOT pressed
	OP_SAVE_DELAY,          // FX07; Set VX to the value of delay timer
	OP_WAIT_KEY,            // FX0A; Wait for key press and store the pressed key
	OP_SET_DELAY,           // FX15; Set delay timer to VX
	OP_SET_SOUND,           // FX18; Set sound timer to VX
	OP_ADD_IDX,             // FX1E; Sets I += VX
	OP_SET_IDX_SPRITE,      // FX29; Sets I to the location of the sprite for the character in VX
	OP_SET_BCD,             // FX33; Save binary-coded decimal of VX at I, I+1, I+2
	OP_DUMP_REGS,           // FX55; Stores V0 to VX (including VX) in memory starting at address I
	OP_LOAD_REGS,           // FX65; Loads V0 to VX from memory starting from address I
	OPCODE_COUNT
} OpcodeDescription;

BYTE getOpcodeIndex(WORD raw);

void processOpcode(C8core *core);

void handle_OP_CALL_MCR(C8core *core, BYTE xParam, BYTE yParam, WORD nParam);
void handle_OP_CLEAR_SCREEN(C8core *core, BYTE xParam, BYTE yParam, WORD nParam);
void handle_OP_RETURN(C8core *core, BYTE xParam, BYTE yParam, WORD nParam);
void handle_OP_JUMP(C8core *core,  BYTE xParam, BYTE yParam, WORD nParam);
void handle_OP_CALL_SUBR(C8core *core, BYTE xParam, BYTE yParam, WORD nParam);
void handle_OP_SKIP_EQ(C8core *core, BYTE xParam, BYTE yParam, WORD nParam);
void handle_OP_SKIP_NEQ(C8core *core, BYTE xParam, BYTE yParam, WORD nParam);
void handle_OP_SKIP_EQ_REG(C8core *core, BYTE xParam, BYTE yParam, WORD nParam);
void handle_OP_SET_CONST(C8core *core, BYTE xParam, BYTE yParam, WORD nParam);
void handle_OP_ADD_CONST(C8core *core, BYTE xParam, BYTE yParam, WORD nParam);
void handle_OP_SET_REG(C8core *core, BYTE xParam, BYTE yParam, WORD nParam);
void handle_OP_OR_REG(C8core *core, BYTE xParam, BYTE yParam, WORD nParam);
void handle_OP_AND_REG(C8core *core, BYTE xParam, BYTE yParam, WORD nParam);
void handle_OP_XOR_REG(C8core *core, BYTE xParam, BYTE yParam, WORD nParam);
void handle_OP_ADD_REG(C8core *core, BYTE xParam, BYTE yParam, WORD nParam);
void handle_OP_SUB_REG(C8core *core, BYTE xParam, BYTE yParam, WORD nParam);
void handle_OP_SHRIGHT_1(C8core *core, BYTE xParam, BYTE yParam, WORD nParam);
void handle_OP_REV_SUB_REG(C8core *core, BYTE xParam, BYTE yParam, WORD nParam);
void handle_OP_SHLEFT_1(C8core *core, BYTE xParam, BYTE yParam, WORD nParam);
void handle_OP_SKIP_NEQ_REG(C8core *core, BYTE xParam, BYTE yParam, WORD nParam);
void handle_OP_SET_IDX(C8core *core, BYTE xParam, BYTE yParam, WORD nParam);
void handle_OP_JUMP_FROM_V0(C8core *core, BYTE xParam, BYTE yParam, WORD nParam);
void handle_OP_SET_RANDOM(C8core *core, BYTE xParam, BYTE yParam, WORD nParam);
void handle_OP_SET_RANDOM(C8core *core, BYTE xParam, BYTE yParam, WORD nParam);
void handle_OP_DRAW(C8core *core, BYTE xParam, BYTE yParam, WORD nParam);
void handle_OP_SKIP_KPRESS(C8core *core, BYTE xParam, BYTE yParam, WORD nParam);
void handle_OP_SKIP_NKPRESS(C8core *core, BYTE xParam, BYTE yParam, WORD nParam);
void handle_OP_SAVE_DELAY(C8core *core, BYTE xParam, BYTE yParam, WORD nParam);
void handle_OP_WAIT_KEY(C8core *core, BYTE xParam, BYTE yParam, WORD nParam);
void handle_OP_SET_DELAY(C8core *core, BYTE xParam, BYTE yParam, WORD nParam);
void handle_OP_SET_SOUND(C8core *core, BYTE xParam, BYTE yParam, WORD nParam);
void handle_OP_ADD_IDX(C8core *core, BYTE xParam, BYTE yParam, WORD nParam);
void handle_OP_SET_IDX_SPRITE(C8core *core, BYTE xParam, BYTE yParam, WORD nParam);
void handle_OP_SET_BCD(C8core *core, BYTE xParam, BYTE yParam, WORD nParam);
void handle_OP_DUMP_REGS(C8core *core, BYTE xParam, BYTE yParam, WORD nParam);
void handle_OP_LOAD_REGS(C8core *core, BYTE xParam, BYTE yParam, WORD nParam);

extern const Opcode OPCODES[OPCODE_COUNT];

#endif  /* _OPCODES_H_ */
