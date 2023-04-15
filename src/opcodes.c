/**
 * Cheap-8: a chip-8 emulator
 * 
 * File: opcodes.c
 * License: DWYW - "Do Whatever You Want"
 * 
 * Opcode handlers and definition of all all opcodes' formats
 */

#include "opcodes.h"

const Opcode OPCODES[OPCODE_COUNT] = {
	{0xFFFF, 0x00E0, PARAMETER_UNUSED, PARAMETER_UNUSED, PARAMETER_UNUSED, handle_OP_CLEAR_SCREEN, 
            "clr", "Clear screen"},
	{0xFFFF, 0x00EE, PARAMETER_UNUSED, PARAMETER_UNUSED, PARAMETER_UNUSED, handle_OP_RETURN, 
            "ret", "Return from subroutine"},
	{0xF000, 0x1000, PARAMETER_UNUSED, PARAMETER_UNUSED, 0x0FFF, handle_OP_JUMP, 
            "jmp", "Jump to address NNN"},
	{0xF000, 0x2000, PARAMETER_UNUSED, PARAMETER_UNUSED, 0x0FFF, handle_OP_CALL_SUBR, 
            "call", "Call subroutine at address NNN"},
	{0xF000, 0x3000, 0x0F00, PARAMETER_UNUSED, 0x00FF, handle_OP_SKIP_EQ, 
            "seq", "Skip next instruction if VX == NN"},
	{0xF000, 0x4000, 0x0F00, PARAMETER_UNUSED, 0x00FF, handle_OP_SKIP_NEQ, 
            "sneq", "Skip next instruction if VX != NN"},
	{0xF00F, 0x5000, 0x0F00, 0x00F0, PARAMETER_UNUSED, handle_OP_SKIP_EQ_REG, 
            "vseq", "Skip next instruction if VX == VY"},
	{0xF000, 0x6000, 0x0F00, PARAMETER_UNUSED, 0x00FF, handle_OP_SET_CONST, 
            "set", "Set VX to NN"},
	{0xF000, 0x7000, 0x0F00, PARAMETER_UNUSED, 0x00FF, handle_OP_ADD_CONST, 
            "add", "Adds NN to VX"},
	{0xF00F, 0x8000, 0x0F00, 0x00F0, PARAMETER_UNUSED, handle_OP_SET_REG, 
            "vset", "Sets VX to VY"},
	{0xF00F, 0x8001, 0x0F00, 0x00F0, PARAMETER_UNUSED, handle_OP_OR_REG, 
            "vor", "Sets VX |= VY"},
	{0xF00F, 0x8002, 0x0F00, 0x00F0, PARAMETER_UNUSED, handle_OP_AND_REG, 
            "vand", "Sets VX &= VY"},
	{0xF00F, 0x8003, 0x0F00, 0x00F0, PARAMETER_UNUSED, handle_OP_XOR_REG, 
            "vxor", "Sets VX ^= VY"},
	{0xF00F, 0x8004, 0x0F00, 0x00F0, PARAMETER_UNUSED, handle_OP_ADD_REG, 
            "add", "Adds VY to VX"},
	{0xF00F, 0x8005, 0x0F00, 0x00F0, PARAMETER_UNUSED, handle_OP_SUB_REG, 
            "sub", "Subtracts VY from VX"},
	{0xF00F, 0x8006, 0x0F00, 0x00F0, PARAMETER_UNUSED, handle_OP_SHRIGHT_1, 
            "shr", "Stores LSB of VX in VF and shifts VX right by VY"},
	{0xF00F, 0x8007, 0x0F00, 0x00F0, PARAMETER_UNUSED, handle_OP_REV_SUB_REG, 
            "subr", "Sets VX = VY - VX"},
	{0xF00F, 0x800E, 0x0F00, 0x00F0, PARAMETER_UNUSED, handle_OP_SHLEFT_1, 
            "shl", "Stores MSB of VX in VF and shifts VX left by VY"},
	{0xF00F, 0x9000, 0x0F00, 0x00F0, PARAMETER_UNUSED, handle_OP_SKIP_NEQ_REG, 
            "vsneq", "Skip instruction if VX != VY"},
	{0xF000, 0xA000, PARAMETER_UNUSED, PARAMETER_UNUSED, 0x0FFF, handle_OP_SET_IDX, 
            "iset", "Sets I = NNN"},
	{0xF000, 0xB000, PARAMETER_UNUSED, PARAMETER_UNUSED, 0x0FFF, handle_OP_JUMP_FROM_V0, 
            "vjmp", "Jumps to address V0 + NNN"},
	{0xF000, 0xC000, 0x0F00, PARAMETER_UNUSED, 0x00FF, handle_OP_SET_RANDOM, 
            "rand", "Sets VX = rand() & NN"},
	{0xF000, 0xD000, 0x0F00, 0x00F0, 0x000F, handle_OP_DRAW, 
            "draw", "Draws a sprite at coordinate VX, VY with a width of 8 and height of N"},
	{0xF0FF, 0xE09E, 0x0F00, PARAMETER_UNUSED, PARAMETER_UNUSED, handle_OP_SKIP_KPRESS, 
            "skey", "Skip next insturction if a key stored in VX is pressed"},
	{0xF0FF, 0xE0A1, 0x0F00, PARAMETER_UNUSED, PARAMETER_UNUSED, handle_OP_SKIP_NKPRESS, 
            "snkey", "Skip next insturction if a key stored in VX is NOT pressed"},
	{0xF0FF, 0xF007, 0x0F00, PARAMETER_UNUSED, PARAMETER_UNUSED, handle_OP_SAVE_DELAY, 
            "vdly", "Set VX to the value of delay timer"},
	{0xF0FF, 0xF00A, 0x0F00, PARAMETER_UNUSED, PARAMETER_UNUSED, handle_OP_WAIT_KEY, 
            "vkey", "Wait for key press and store the pressed key"},
	{0xF0FF, 0xF015, 0x0F00, PARAMETER_UNUSED, PARAMETER_UNUSED, handle_OP_SET_DELAY, 
            "vdset", "Set delay timer to VX"},
	{0xF0FF, 0xF018, 0x0F00, PARAMETER_UNUSED, PARAMETER_UNUSED, handle_OP_SET_SOUND, 
            "vsset", "Set sound timer to VX"},
	{0xF0FF, 0xF01E, 0x0F00, PARAMETER_UNUSED, PARAMETER_UNUSED, handle_OP_ADD_IDX, 
            "viadd", "Sets I += VX"},
	{0xF0FF, 0xF029, 0x0F00, PARAMETER_UNUSED, PARAMETER_UNUSED, handle_OP_SET_IDX_SPRITE, 
            "vispr", "Sets I to the location of the sprite for the character in VX"},
	{0xF0FF, 0xF033, 0x0F00, PARAMETER_UNUSED, PARAMETER_UNUSED, handle_OP_SET_BCD, 
            "bcd", "Save binary-coded decimal of VX at I, I+1, I+2"},
	{0xF0FF, 0xF055, 0x0F00, PARAMETER_UNUSED, PARAMETER_UNUSED, handle_OP_DUMP_REGS, 
            "save", "Stores V0 to VX (including VX) in memory starting at address I"},
	{0xF0FF, 0xF065, 0x0F00, PARAMETER_UNUSED, PARAMETER_UNUSED, handle_OP_LOAD_REGS, 
            "load", "Loads V0 to VX from memory starting from address I"},
	{0xFFFF, 0x0000, PARAMETER_UNUSED, PARAMETER_UNUSED, PARAMETER_UNUSED, handle_OP_CALL_MCR, 
            "nop", "Calls machine code routine NNN"}
};

// ========================================================================================================

// Converts and opcode mask into an opcode id from OpcodeDescription enum
inline BYTE getOpcodeIndex(WORD raw) {
    for (BYTE i = 0; i < OPCODE_COUNT; i++) {
		if ((raw & OPCODES[i].opcodeMask) == OPCODES[i].opcodeId) {
            return i;
		}
	}
    return OP_CALL_MCR;
}

// Applies all necessary masks to a current opcode (i.e. the one pointed
// to by core->PC register) and then calls this opcode's handler
void processOpcode(C8core *core) {
	WORD xParam = PARAMETER_UNUSED;
	WORD yParam = PARAMETER_UNUSED;
	WORD nParam = PARAMETER_UNUSED;
	const Opcode *opcode = NULL;
	WORD opcodeRaw = core->opcode;

    BYTE idx = getOpcodeIndex(opcodeRaw);
    opcode = &OPCODES[idx];

	if (opcode == NULL) {
		SET_CUSTOM_FLAG(core, CUSTOM_FLAG_BAD_OPCODE);
		return;
	}

	xParam = opcode->xParamMask != PARAMETER_UNUSED ? (opcodeRaw & opcode->xParamMask) >> 8 : PARAMETER_UNUSED;
	yParam = opcode->yParamMask != PARAMETER_UNUSED ? (opcodeRaw & opcode->yParamMask) >> 4 : PARAMETER_UNUSED;
	nParam = opcode->nParamMask != PARAMETER_UNUSED ? (opcodeRaw & opcode->nParamMask) : PARAMETER_UNUSED;

	core->xParam = xParam;
	core->yParam = yParam;
	core->nParam = nParam;

	core->PC += OPCODE_SIZE;
	opcode->handler(core, (BYTE) xParam, (BYTE) yParam, nParam);
}

// ========================================================================================================

void handle_OP_CALL_MCR(C8core *core, BYTE xParam, BYTE yParam, WORD nParam) {
	// XXX: We are no implementing a call to machine code routine because:
	// 		a) I have no clue how to properly do it
	// 		b) This opcode is unused by a good majority of ROMs

	// If we encounter this opcode then we set a custom flag indicating that we indeed encountered it
	SET_CUSTOM_FLAG(core, CUSTOM_FLAG_CRITICAL_ERROR);
}

void handle_OP_CLEAR_SCREEN(C8core *core, BYTE xParam, BYTE yParam, WORD nParam) {
	memset(core->gfx, 0, sizeof(QWORD) * SCREEN_RESOLUTION_HEIGHT);
    memset(core->gfx_upd, 0, sizeof(QWORD) * SCREEN_RESOLUTION_HEIGHT);
	SET_CUSTOM_FLAG(core, CUSTOM_FLAG_CLEAR_SCREEN);
}

void handle_OP_RETURN(C8core *core, BYTE xParam, BYTE yParam, WORD nParam) {
	if (core->SP > 0) {
		core->SP -= 1;
		core->PC = core->stack[core->SP];
	} else {
		SET_CUSTOM_FLAG(core, CUSTOM_FLAG_BAD_SP);
	}
}

void handle_OP_JUMP(C8core *core, BYTE xParam, BYTE yParam, WORD nParam) {
	if (nParam < MEMORY_RANGE_PROGRAM_MIN) {
		SET_CUSTOM_FLAG(core, CUSTOM_FLAG_BAD_MEMORY);
		return;
	}

	core->PC = nParam;
}

void handle_OP_CALL_SUBR(C8core *core, BYTE xParam, BYTE yParam, WORD nParam) {
	if (nParam < MEMORY_RANGE_PROGRAM_MIN) {
		SET_CUSTOM_FLAG(core, CUSTOM_FLAG_BAD_MEMORY);
	}

	core->stack[core->SP] = core->PC;
	core->SP += 1;
	core->PC = nParam;
}

void handle_OP_SKIP_EQ(C8core *core, BYTE xParam, BYTE yParam, WORD nParam) {
	if (core->reg[xParam] == nParam) {
		if (core->PC + OPCODE_SIZE > MEMORY_SIZE) {
			SET_CUSTOM_FLAG(core, CUSTOM_FLAG_BAD_MEMORY);
			return;
		}
		
		core->PC += OPCODE_SIZE;
	}
}

void handle_OP_SKIP_NEQ(C8core *core, BYTE xParam, BYTE yParam, WORD nParam) {
	if (core->reg[xParam] != nParam) {
		if (core->PC + OPCODE_SIZE > MEMORY_SIZE) {
			SET_CUSTOM_FLAG(core, CUSTOM_FLAG_BAD_MEMORY);
			return;
		}
		
		core->PC += OPCODE_SIZE;
	}
}

void handle_OP_SKIP_EQ_REG(C8core *core, BYTE xParam, BYTE yParam, WORD nParam) {
	if (core->reg[xParam] == core->reg[yParam]) {
		if (core->PC + OPCODE_SIZE > MEMORY_SIZE) {
			SET_CUSTOM_FLAG(core, CUSTOM_FLAG_BAD_MEMORY);
			return;
		}

		core->PC += OPCODE_SIZE;
	}
}

void handle_OP_SET_CONST(C8core *core, BYTE xParam, BYTE yParam, WORD nParam) {
	core->reg[xParam] = nParam;
}

void handle_OP_ADD_CONST(C8core *core, BYTE xParam, BYTE yParam, WORD nParam) {
	core->reg[xParam] += nParam;
}

void handle_OP_SET_REG(C8core *core, BYTE xParam, BYTE yParam, WORD nParam) {
	core->reg[xParam] = core->reg[yParam];
}

void handle_OP_OR_REG(C8core *core, BYTE xParam, BYTE yParam, WORD nParam) {
	core->reg[xParam] |= core->reg[yParam];
}

void handle_OP_AND_REG(C8core *core, BYTE xParam, BYTE yParam, WORD nParam) {
	core->reg[xParam] &= core->reg[yParam];
}

void handle_OP_XOR_REG(C8core *core, BYTE xParam, BYTE yParam, WORD nParam) {
	core->reg[xParam] ^= core->reg[yParam];
}

void handle_OP_ADD_REG(C8core *core, BYTE xParam, BYTE yParam, WORD nParam) {
	WORD result = core->reg[xParam] + core->reg[yParam];
	core->reg[REG_VF] = result >= (1 << 8) ? 1 : 0;
	core->reg[xParam] = result & 0xFF;
}

void handle_OP_SUB_REG(C8core *core, BYTE xParam, BYTE yParam, WORD nParam) {
	core->reg[REG_VF] = core->reg[xParam] > core->reg[yParam] ? 1 : 0;
	core->reg[xParam] -= core->reg[yParam];
}

void handle_OP_SHRIGHT_1(C8core *core, BYTE xParam, BYTE yParam, WORD nParam) {
	core->reg[REG_VF] = (core->reg[xParam] & (1 << 0));
	core->reg[xParam] >>= core->reg[yParam];
}

void handle_OP_REV_SUB_REG(C8core *core, BYTE xParam, BYTE yParam, WORD nParam) {
	core->reg[REG_VF] = core->reg[yParam] > core->reg[xParam] ? 1 : 0;
	core->reg[xParam] = core->reg[yParam] - core->reg[xParam];
}

void handle_OP_SHLEFT_1(C8core *core, BYTE xParam, BYTE yParam, WORD nParam) {
	core->reg[REG_VF] = (core->reg[xParam] & (1 << 7));
	core->reg[xParam] <<= core->reg[yParam];
}

void handle_OP_SKIP_NEQ_REG(C8core *core, BYTE xParam, BYTE yParam, WORD nParam) {
	if (core->reg[xParam] != core->reg[yParam]) {
		if ((core->PC + OPCODE_SIZE) > MEMORY_SIZE) {
			SET_CUSTOM_FLAG(core, CUSTOM_FLAG_BAD_MEMORY);
			return;
		}
		core->PC += OPCODE_SIZE;
	}
}

void handle_OP_SET_IDX(C8core *core, BYTE xParam, BYTE yParam, WORD nParam) {
	core->I = nParam;
}

void handle_OP_JUMP_FROM_V0(C8core *core, BYTE xParam, BYTE yParam, WORD nParam) {
	WORD targetAddr = core->reg[REG_V0] + nParam;
	if (targetAddr > MEMORY_SIZE) {
		SET_CUSTOM_FLAG(core, CUSTOM_FLAG_BAD_MEMORY);
		return;
	}
	core->PC = targetAddr;
}

void handle_OP_SET_RANDOM(C8core *core, BYTE xParam, BYTE yParam, WORD nParam) {
	core->reg[xParam] = (rand() % (1 << 7)) & nParam;
}

/** handle_OP_DRAW
 *
 * In this implementation of a chip-8 system emulator the screen is percieved by
 * the program as an array of uint64 so therefore each bit (set or unset) represents
 * a pixel on a 32 by 64 screen (white or black)
 *
 * That's why the logic might seem counterintuitive at the first glance
 */
void handle_OP_DRAW(C8core *core, BYTE xParam, BYTE yParam, WORD nParam) {
	if (nParam == 0) {
		return;
	}

    // Getting x and y coordinates for a sprite to be painted at
    // The remainder div is used just in case if the coordinates being supplied
    // overflow the screen dimensions
	BYTE x = core->reg[xParam] % SCREEN_RESOLUTION_WIDTH;
	BYTE y = core->reg[yParam] % SCREEN_RESOLUTION_HEIGHT;

	BYTE height = y + nParam;

    // This flag signalizes if the portion (or the whole) sprite is overlapping the screen
    // height, in which case we would paint it appropriately (which is overlapping the screen)
	BYTE hasVerticalOverlap = (height - 1) > SCREEN_RESOLUTION_HEIGHT ? 1 : 0;

	if (hasVerticalOverlap) {
		height = height - 1 - SCREEN_RESOLUTION_HEIGHT;
	}

	core->reg[REG_VF] = 0;

    // Iterating over bytes representing sprite pixel rows
	for (BYTE sprite = 0; sprite < nParam; sprite++) {
		QWORD newScreenRow = 0;
        QWORD newUpdateRow = 0;
        QWORD qsprite = core->memory[core->I + sprite];
        
        // If we overlap the screen vertically
		if (x + 8 > SCREEN_RESOLUTION_WIDTH) {
            // Then draw the remainder starting from the left (overlap it visually)
			newScreenRow |= qsprite >> (x + 8 - SCREEN_RESOLUTION_WIDTH);
            newUpdateRow |= (QWORD)0xFF >> (x + 8 - SCREEN_RESOLUTION_WIDTH);
		} else { 
            // Otherwise draw the sprite row normally
			newScreenRow |= qsprite << (SCREEN_RESOLUTION_WIDTH - x - 8);
            newUpdateRow |= (QWORD)0xFF << (SCREEN_RESOLUTION_WIDTH - x - 8);
		}
        
		QWORD savedScreenRow = core->gfx[y];
		core->gfx[y] ^= newScreenRow;
        core->gfx_upd[y] |= newUpdateRow;

        // If any of the bits on the screen that the sprite was drawn over were set
		if ((savedScreenRow | newScreenRow) != core->gfx[y]) {
            // then we need to set VF register
			core->reg[REG_VF] = 1;
		}
		
		y += 1;
		if (y == height) {
			y = 0;
			if (hasVerticalOverlap) {
				height = nParam - height;
			}
		}
	}

	SET_CUSTOM_FLAG(core, CUSTOM_FLAG_REDRAW_PENDING);
}

void handle_OP_SKIP_KPRESS(C8core *core, BYTE xParam, BYTE yParam, WORD nParam) {
	if (core->reg[xParam] >= 16) {
		SET_CUSTOM_FLAG(core, CUSTOM_FLAG_BAD_INPUT);
		return;
	}
	if ((core->keypadState & (1 << core->reg[xParam])) == 1) {
		if ((core->PC + OPCODE_SIZE) > MEMORY_SIZE) {
			SET_CUSTOM_FLAG(core, CUSTOM_FLAG_BAD_MEMORY);
			return;
		}
		core->PC += OPCODE_SIZE;
	}
}

void handle_OP_SKIP_NKPRESS(C8core *core, BYTE xParam, BYTE yParam, WORD nParam) {
	if (core->reg[xParam] >= 16) {
		SET_CUSTOM_FLAG(core, CUSTOM_FLAG_BAD_INPUT);
		return;
	}
	if ((core->keypadState & (1 << core->reg[xParam])) == 0) {
		if ((core->PC + OPCODE_SIZE) > MEMORY_SIZE) {
			SET_CUSTOM_FLAG(core, CUSTOM_FLAG_BAD_MEMORY);
			return;
		}
		core->PC += OPCODE_SIZE;
	}
}

void handle_OP_SAVE_DELAY(C8core *core, BYTE xParam, BYTE yParam, WORD nParam) {
	core->reg[xParam] = core->tDelay;
}

void handle_OP_WAIT_KEY(C8core *core, BYTE xParam, BYTE yParam, WORD nParam) {
	for (int i = 15; i >= 0; i--) {
		if (core->keypadState & (1 << i)) {
			core->reg[xParam] = i;
			return;
		}
	}

	if (core->PC - OPCODE_SIZE < MEMORY_RANGE_PROGRAM_MIN) {
		SET_CUSTOM_FLAG(core, CUSTOM_FLAG_BAD_MEMORY);
		return;
	}

	core->PC -= OPCODE_SIZE;
}

void handle_OP_SET_DELAY(C8core *core, BYTE xParam, BYTE yParam, WORD nParam) {
	core->tDelay = core->reg[xParam];
}

void handle_OP_SET_SOUND(C8core *core, BYTE xParam, BYTE yParam, WORD nParam) {
	core->tSound = core->reg[xParam];
}

void handle_OP_ADD_IDX(C8core *core, BYTE xParam, BYTE yParam, WORD nParam) {
	if (core->I + core->reg[xParam] > MEMORY_SIZE) {
		SET_CUSTOM_FLAG(core, CUSTOM_FLAG_BAD_MEMORY);
		return;
	}

	core->I += core->reg[xParam];
}

void handle_OP_SET_IDX_SPRITE(C8core *core, BYTE xParam, BYTE yParam, WORD nParam) {
	core->I = MEMORY_RANGE_FONTSET_MIN + (FONT_ENTITY_SIZE * core->reg[xParam]);
}

void handle_OP_SET_BCD(C8core *core, BYTE xParam, BYTE yParam, WORD nParam) {
	if (core->I > (MEMORY_SIZE - 2)) {
		SET_CUSTOM_FLAG(core, CUSTOM_FLAG_BAD_MEMORY);
		return;
	}

	BYTE val = core->reg[xParam];

	core->memory[core->I + 2] = val % 10;
	val /= 10;
	core->memory[core->I + 1] = val % 10;
	val /= 10;
	core->memory[core->I] = val % 10;
}

void handle_OP_DUMP_REGS(C8core *core, BYTE xParam, BYTE yParam, WORD nParam) {
	if (core->I > (MEMORY_SIZE - GENERAL_PURPOSE_REGISTERS)) {
		SET_CUSTOM_FLAG(core, CUSTOM_FLAG_BAD_MEMORY);
		return;
	}
	
	for (int i = 0; i < GENERAL_PURPOSE_REGISTERS; i++) {
		core->memory[core->I + i] = core->reg[i];
	}
}

void handle_OP_LOAD_REGS(C8core *core, BYTE xParam, BYTE yParam, WORD nParam) {
	if (core->I > (MEMORY_SIZE - GENERAL_PURPOSE_REGISTERS)) {
		SET_CUSTOM_FLAG(core, CUSTOM_FLAG_BAD_MEMORY);
		return;
	}
	
	for (int i = 0; i < GENERAL_PURPOSE_REGISTERS; i++) {
		core->reg[i] = core->memory[core->I + i];
	}
}
