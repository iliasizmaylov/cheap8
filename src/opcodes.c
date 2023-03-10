#include "opcodes.h"

const Opcode OPCODES[OPCODE_COUNT] = {
	{0xF000, 0x0000, PARAMETER_UNUSED, PARAMETER_UNUSED, PARAMETER_UNUSED, handle_OP_CALL_MCR, 
            "Calls machine code routine NNN"},
	{0xFFFF, 0x00E0, PARAMETER_UNUSED, PARAMETER_UNUSED, PARAMETER_UNUSED, handle_OP_CLEAR_SCREEN, 
            "Clear screen"},
	{0xFFFF, 0x00EE, PARAMETER_UNUSED, PARAMETER_UNUSED, PARAMETER_UNUSED, handle_OP_RETURN, 
            "Return from subroutine"},
	{0xF000, 0x1000, PARAMETER_UNUSED, PARAMETER_UNUSED, 0x0FFF, handle_OP_JUMP, 
            "Jump to address NNN"},
	{0xF000, 0x2000, PARAMETER_UNUSED, PARAMETER_UNUSED, 0x0FFF, handle_OP_CALL_SUBR, 
            "Call subroutine at address NNN"},
	{0xF000, 0x3000, 0x0F00, PARAMETER_UNUSED, 0x00FF, handle_OP_SKIP_EQ, 
            "Skip next instruction if VX == NN"},
	{0xF000, 0x4000, 0x0F00, PARAMETER_UNUSED, 0x00FF, handle_OP_SKIP_NEQ, 
            "Skip next instruction if VX != NN"},
	{0xF00F, 0x5000, 0x0F00, 0x00F0, PARAMETER_UNUSED, handle_OP_SKIP_EQ_REG, 
            "Skip next instruction if VX == VY"},
	{0xF000, 0x6000, 0x0F00, PARAMETER_UNUSED, 0x00FF, handle_OP_SET_CONST, 
            "Set VX to NN"},
	{0xF000, 0x7000, 0x0F00, PARAMETER_UNUSED, 0x00FF, handle_OP_ADD_CONST, 
            "Adds NN to VX"},
	{0xF00F, 0x8000, 0x0F00, 0x00F0, PARAMETER_UNUSED, handle_OP_SET_REG, 
            "Sets VX to VY"},
	{0xF00F, 0x8001, 0x0F00, 0x00F0, PARAMETER_UNUSED, handle_OP_OR_REG, 
            "Sets VX |= VY"},
	{0xF00F, 0x8002, 0x0F00, 0x00F0, PARAMETER_UNUSED, handle_OP_AND_REG, 
            "Sets VX &= VY"},
	{0xF00F, 0x8003, 0x0F00, 0x00F0, PARAMETER_UNUSED, handle_OP_XOR_REG, 
            "Sets VX ^= VY"},
	{0xF00F, 0x8004, 0x0F00, 0x00F0, PARAMETER_UNUSED, handle_OP_ADD_REG, 
            "Adds VY to VX"},
	{0xF00F, 0x8005, 0x0F00, 0x00F0, PARAMETER_UNUSED, handle_OP_SUB_REG, 
            "Subtracts VY from VX"},
	{0xF00F, 0x8006, 0x0F00, 0x00F0, PARAMETER_UNUSED, handle_OP_SHRIGHT_1, 
            "Stores LSB of VX in VF and shifts VX right by 1"},
	{0xF00F, 0x8007, 0x0F00, 0x00F0, PARAMETER_UNUSED, handle_OP_REV_SUB_REG, 
            "Sets VX = VY - VX"},
	{0xF00F, 0x800E, 0x0F00, 0x00F0, PARAMETER_UNUSED, handle_OP_SHLEFT_1, 
            "Stores MSB of VX in VF and shifts VX left by 1"},
	{0xF00F, 0x0000, 0x0F00, 0x00F0, PARAMETER_UNUSED, handle_OP_SKIP_NEQ_REG, 
            "Skip instruction if VX != VY"},
	{0xF000, 0xA000, PARAMETER_UNUSED, PARAMETER_UNUSED, 0x0FFF, handle_OP_SET_IDX, 
            "Sets I = NNN"},
	{0xF000, 0xB000, PARAMETER_UNUSED, PARAMETER_UNUSED, 0x0FFF, handle_OP_JUMP_FROM_V0, 
            "Jumps to address V0 + NNN"},
	{0xF000, 0xC000, 0x0F00, PARAMETER_UNUSED, 0x00FF, handle_OP_SET_RANDOM, 
            "Sets VX = rand() & NN"},
	{0xF000, 0xD000, 0x0F00, 0x00F0, 0x000F, handle_OP_DRAW, 
            "Draws a sprite at coordinate VX, VY with a width of 8 and height of N"},
	{0xF0FF, 0xE09E, 0x0F00, PARAMETER_UNUSED, PARAMETER_UNUSED, handle_OP_SKIP_KPRESS, 
            "Skip next insturction if a key stored in VX is pressed"},
	{0xF0FF, 0xE0A1, 0x0F00, PARAMETER_UNUSED, PARAMETER_UNUSED, handle_OP_SKIP_NKPRESS, 
            "Skip next insturction if a key stored in VX is NOT pressed"},
	{0xF0FF, 0xF007, 0x0F00, PARAMETER_UNUSED, PARAMETER_UNUSED, handle_OP_SAVE_DELAY, 
            "Set VX to the value of delay timer"},
	{0xF0FF, 0xF00A, 0x0F00, PARAMETER_UNUSED, PARAMETER_UNUSED, handle_OP_WAIT_KEY, 
            "Wait for key press and store the pressed key"},
	{0xF0FF, 0xF015, 0x0F00, PARAMETER_UNUSED, PARAMETER_UNUSED, handle_OP_SET_DELAY, 
            "Set delay timer to VX"},
	{0xF0FF, 0xF018, 0x0F00, PARAMETER_UNUSED, PARAMETER_UNUSED, handle_OP_SET_SOUND, 
            "Set sound timer to VX"},
	{0xF0FF, 0xF01E, 0x0F00, PARAMETER_UNUSED, PARAMETER_UNUSED, handle_OP_ADD_IDX, 
            "Sets I += VX"},
	{0xF0FF, 0xF029, 0x0F00, PARAMETER_UNUSED, PARAMETER_UNUSED, handle_OP_SET_IDX_SPRITE, 
            "Sets I to the location of the sprite for the character in VX"},
	{0xF0FF, 0xF033, 0x0F00, PARAMETER_UNUSED, PARAMETER_UNUSED, handle_OP_SET_BCD, 
            "Save binary-coded decimal of VX at I, I+1, I+2"},
	{0xF0FF, 0xF055, 0x0F00, PARAMETER_UNUSED, PARAMETER_UNUSED, handle_OP_DUMP_REGS, 
            "Stores V0 to VX (including VX) in memory starting at address I"},
	{0xF0FF, 0xF065, 0x0F00, PARAMETER_UNUSED, PARAMETER_UNUSED, handle_OP_LOAD_REGS, 
            "Loads V0 to VX from memory starting from address I"}
};

// ========================================================================================================

inline BYTE getOpcodeIndex(WORD raw) {
    for (BYTE i = 0; i < OPCODE_COUNT; i++) {
		if ((raw & OPCODES[i].opcodeMask) == OPCODES[i].opcodeId) {
            return i;
		}
	}
    return OP_CALL_MCR;
}

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
	SET_CUSTOM_FLAG(core, CUSTOM_FLAG_MCR_OPCODE);
}

void handle_OP_CLEAR_SCREEN(C8core *core, BYTE xParam, BYTE yParam, WORD nParam) {
	memset(core->gfx, 0, sizeof(QWORD) * SCREEN_RESOLUTION_HEIGHT);
	SET_CUSTOM_FLAG(core, CUSTOM_FLAG_REDRAW_PENDING);
}

void handle_OP_RETURN(C8core *core, BYTE xParam, BYTE yParam, WORD nParam) {
	if (core->SP > 0) {
		core->PC = core->stack[core->SP];
		core->SP -= 1;
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
	core->reg[xParam] >>= 1;
}

void handle_OP_REV_SUB_REG(C8core *core, BYTE xParam, BYTE yParam, WORD nParam) {
	core->reg[REG_VF] = core->reg[yParam] > core->reg[xParam] ? 1 : 0;
	core->reg[xParam] = core->reg[yParam] - core->reg[xParam];
}

void handle_OP_SHLEFT_1(C8core *core, BYTE xParam, BYTE yParam, WORD nParam) {
	core->reg[REG_VF] = (core->reg[xParam] & (1 << 7));
	core->reg[xParam] <<= 1;
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

void handle_OP_DRAW(C8core *core, BYTE xParam, BYTE yParam, WORD nParam) {
	if (nParam == 0) {
		return;
	}

	BYTE x = core->reg[xParam] % SCREEN_RESOLUTION_WIDTH;
	BYTE y = core->reg[yParam] % SCREEN_RESOLUTION_HEIGHT;

	BYTE height = y + nParam;
	BYTE hasVerticalOverlap = (height - 1) > SCREEN_RESOLUTION_HEIGHT ? 1 : 0;

	if (hasVerticalOverlap) {
		height = height - 1 - SCREEN_RESOLUTION_HEIGHT;
	}

	core->reg[REG_VF] = 0;

	for (BYTE sprite = 0; sprite < nParam; sprite++) {
		QWORD newScreenRow = 0;
        QWORD qsprite = core->memory[core->I + sprite];
		if (x + 8 > SCREEN_RESOLUTION_WIDTH) {
			newScreenRow |= qsprite >> (x + 8 - SCREEN_RESOLUTION_WIDTH);
		} else {
			newScreenRow |= qsprite << (SCREEN_RESOLUTION_WIDTH - x - 8);
		}
        
		QWORD savedScreenRow = core->gfx[y];
		core->gfx[y] ^= newScreenRow;
		if ((savedScreenRow | newScreenRow) != core->gfx[y]) {
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