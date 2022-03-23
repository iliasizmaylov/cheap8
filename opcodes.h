#ifndef _OPCODES_H_
#define _OPCODES_H_

#include "c8core.h"

#define OPCODE_SIZE			sizeof(WORD)
#define PARAMETER_UNUSED	0xFFFF

typedef void (*OPCODE_HANDLER)(C8core*, BYTE, BYTE, WORD);

typedef struct _Opcode {
	WORD opcodeMask;
	WORD opcodeId;
	WORD xParamMask;
	WORD yParamMask;
	WORD nParamMask;
	OPCODE_HANDLER handler;
} Opcode;

typedef enum {
	OP_CALL_MCR = 0,	// 0NNN; Calls machine code routine NNN
	OP_CLEAR_SCREEN,	// 00E0; Clear screen
	OP_RETURN,			// 00EE; Return from subroutine
	OP_JUMP,			// 1NNN; Jump to address NNN
	OP_CALL_SUBR,		// 2NNN; Call subroutine at address NNN
	OP_SKIP_EQ,			// 3XNN; Skip next instruction if VX == NN
	OP_SKIP_NEQ,		// 4XNN; Skip next instruction if VX != NN
	OP_SKIP_EQ_REG,		// 5XY0; Skip next instruction if VX == VY
	OP_SET_CONST,		// 6XNN; Set VX to NN
	OP_ADD_CONST,		// 7XNN; Adds NN to VX
	OP_SET_REG,			// 8XY0; Sets VX to VY
	OP_OR_REG,			// 8XY1; Sets VX |= VY
	OP_AND_REG,			// 8XY2; Sets VX &= VY
	OP_XOR_REG,			// 8XY3; Sets VX ^= VY
	OP_ADD_REG,			// 8XY4; Adds VY to VX
	OP_SUB_REG,			// 8XY5; Subtracts VY from VX
	OP_SHRIGHT_1,		// 8XY6; Stores LSB of VX in VF and shifts VX right by 1
	OP_REV_SUB_REG,		// 8XY7; Sets VX = VY - VX; 
	OP_SHLEFT_1,		// 8XYE; Stores MSB of VX in VF and shifts VX left by 1
	OP_SKIP_NEQ_REG,	// 9XY0; Skip instruction if VX != VY
	OP_SET_IDX,			// ANNN; Sets I = NNN
	OP_JUMP_FROM_V0,	// BNNN; Jumps to address V0 + NNN
	OP_SET_RANDOM,		// CXNN; Sets VX = rand() & NN
	OP_DRAW,			// DXYN; Draws a sprite at coordinate VX, VY with a width of 8 and height of N
	OP_SKIP_KPRESS,		// EX9E; Skip next insturction if a key stored in VX is pressed
	OP_SKIP_NKPRESS,	// EXA1; Skip next insturction if a key stored in VX is NOT pressed
	OP_SAVE_DELAY,		// FX07; Set VX to the value of delay timer
	OP_WAIT_KEY,		// FX0A; Wait for key press and store the pressed key
	OP_SET_DELAY,		// FX15; Set delay timer to VX
	OP_SET_SOUND,		// FX18; Set sound timer to VX
	OP_ADD_IDX,			// FX1E; Sets I += VX
	OP_SET_IDX_SPRITE,	// FX29; Sets I to the location of the sprite for the character in VX
	OP_SET_BCD,			// FX33; Save binary-coded decimal of VX at I, I+1, I+2
	OP_DUMP_REGS,		// FX55; Stores V0 to VX (including VX) in memory starting at address I
	OP_LOAD_REGS,		// FX65; Loads V0 to VX from memory starting from address I
	OPCODE_COUNT
} OpcodeDescription;

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

static const Opcode OPCODES[OPCODE_COUNT] = {
	{0xF000, 0x0000, PARAMETER_UNUSED, PARAMETER_UNUSED, PARAMETER_UNUSED, handle_OP_CALL_MCR},
	{0xFFFF, 0x00E0, PARAMETER_UNUSED, PARAMETER_UNUSED, PARAMETER_UNUSED, handle_OP_CLEAR_SCREEN},
	{0xFFFF, 0x00EE, PARAMETER_UNUSED, PARAMETER_UNUSED, PARAMETER_UNUSED, handle_OP_RETURN},
	{0xF000, 0x1000, PARAMETER_UNUSED, PARAMETER_UNUSED, 0x0FFF, handle_OP_JUMP},
	{0xF000, 0x2000, PARAMETER_UNUSED, PARAMETER_UNUSED, 0x0FFF, handle_OP_CALL_SUBR},
	{0xF000, 0x3000, 0x0F00, PARAMETER_UNUSED, 0x00FF, handle_OP_SKIP_EQ},
	{0xF000, 0x4000, 0x0F00, PARAMETER_UNUSED, 0x00FF, handle_OP_SKIP_NEQ},
	{0xF00F, 0x5000, 0x0F00, 0x00F0, PARAMETER_UNUSED, handle_OP_SKIP_EQ_REG},
	{0xF000, 0x6000, 0x0F00, PARAMETER_UNUSED, 0x00FF, handle_OP_SET_CONST},
	{0xF000, 0x7000, 0x0F00, PARAMETER_UNUSED, 0x00FF, handle_OP_ADD_CONST},
	{0xF00F, 0x8000, 0x0F00, 0x00F0, PARAMETER_UNUSED, handle_OP_SET_REG},
	{0xF00F, 0x8001, 0x0F00, 0x00F0, PARAMETER_UNUSED, handle_OP_OR_REG},
	{0xF00F, 0x8002, 0x0F00, 0x00F0, PARAMETER_UNUSED, handle_OP_AND_REG},
	{0xF00F, 0x8003, 0x0F00, 0x00F0, PARAMETER_UNUSED, handle_OP_XOR_REG},
	{0xF00F, 0x8004, 0x0F00, 0x00F0, PARAMETER_UNUSED, handle_OP_ADD_REG},
	{0xF00F, 0x8005, 0x0F00, 0x00F0, PARAMETER_UNUSED, handle_OP_SUB_REG},
	{0xF00F, 0x8006, 0x0F00, 0x00F0, PARAMETER_UNUSED, handle_OP_SHRIGHT_1},
	{0xF00F, 0x8007, 0x0F00, 0x00F0, PARAMETER_UNUSED, handle_OP_REV_SUB_REG},
	{0xF00F, 0x800E, 0x0F00, 0x00F0, PARAMETER_UNUSED, handle_OP_SHLEFT_1},
	{0xF00F, 0x0000, 0x0F00, 0x00F0, PARAMETER_UNUSED, handle_OP_SKIP_NEQ_REG},
	{0xF000, 0xA000, PARAMETER_UNUSED, PARAMETER_UNUSED, 0x0FFF, handle_OP_SET_IDX},
	{0xF000, 0xB000, PARAMETER_UNUSED, PARAMETER_UNUSED, 0x0FFF, handle_OP_JUMP_FROM_V0},
	{0xF000, 0xC000, 0x0F00, PARAMETER_UNUSED, 0x00FF, handle_OP_SET_RANDOM},
	{0xF000, 0xD000, 0x0F00, 0x00F0, 0x000F, handle_OP_DRAW},
	{0xF0FF, 0xE09E, 0x0F00, PARAMETER_UNUSED, PARAMETER_UNUSED, handle_OP_SKIP_KPRESS},
	{0xF0FF, 0xE0A1, 0x0F00, PARAMETER_UNUSED, PARAMETER_UNUSED, handle_OP_SKIP_NKPRESS},
	{0xF0FF, 0xF007, 0x0F00, PARAMETER_UNUSED, PARAMETER_UNUSED, handle_OP_SAVE_DELAY},
	{0xF0FF, 0xF00A, 0x0F00, PARAMETER_UNUSED, PARAMETER_UNUSED, handle_OP_WAIT_KEY},
	{0xF0FF, 0xF015, 0x0F00, PARAMETER_UNUSED, PARAMETER_UNUSED, handle_OP_SET_DELAY},
	{0xF0FF, 0xF018, 0x0F00, PARAMETER_UNUSED, PARAMETER_UNUSED, handle_OP_SET_SOUND},
	{0xF0FF, 0xF01E, 0x0F00, PARAMETER_UNUSED, PARAMETER_UNUSED, handle_OP_ADD_IDX},
	{0xF0FF, 0xF029, 0x0F00, PARAMETER_UNUSED, PARAMETER_UNUSED, handle_OP_SET_IDX_SPRITE},
	{0xF0FF, 0xF033, 0x0F00, PARAMETER_UNUSED, PARAMETER_UNUSED, handle_OP_SET_BCD},
	{0xF0FF, 0xF055, 0x0F00, PARAMETER_UNUSED, PARAMETER_UNUSED, handle_OP_DUMP_REGS},
	{0xF0FF, 0xF065, 0x0F00, PARAMETER_UNUSED, PARAMETER_UNUSED, handle_OP_LOAD_REGS}
};

#endif
