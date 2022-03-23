#include "c8core.h"

VM_RESULT loadROM(C8core *core, FILE *ROM) {
	WORD loadCursor = MEMORY_RANGE_PROGRAM_MIN;
	BYTE nextByte = 0;

	while (loadCursor <= MEMORY_RANGE_PROGRAM_MAX) {
		nextByte = fgetc(ROM);

		if (feof(ROM)) {
			break;
		}

		core->memory[loadCursor] = (BYTE) nextByte;
		loadCursor += 1;
	}

	return VM_RESULT_SUCCESS;
}

VM_RESULT initCore(C8core **m_core, FILE *ROM) {
	*m_core = (C8core*) malloc(sizeof(C8core));
	C8core *core = *m_core;

	VM_ASSERT(core == NULL);

	for (WORD i = 0; i < MEMORY_SIZE; i++) {
		core->memory[i] = 0;
	}

	for (WORD i = MEMORY_RANGE_FONTSET_MIN, j = 0; i <= MEMORY_RANGE_FONTSET_MAX; i++, j++) {
		core->memory[i] = fontset[j];
	}

	for (BYTE i = 0; i < GENERAL_PURPOSE_REGISTERS; i++) {
		core->reg[i] = 0;
	}

	core->PC = MEMORY_RANGE_PROGRAM_MIN;
	core->opcode = 0;
	core->I = 0;
	core->SP = 0;
	core->tDelay = 0;
	core->tSound = 0;
	core->keypadState = 0;

	core->prevCycleTicks = SDL_GetTicks64();
	core->prevTimerTicks = SDL_GetTicks64();

	core->customFlags = 0;

	for (WORD i = 0; i < SCREEN_RESOLUTION_HEIGHT; i++) {
		core->gfx[i] = 0;
	}

	VM_ASSERT(loadROM(core, ROM) != VM_RESULT_SUCCESS);

	return VM_RESULT_SUCCESS;
}

VM_RESULT destroyCore(C8core **m_core) {
	VM_ASSERT(*m_core == NULL);
	free(*m_core);

	return VM_RESULT_SUCCESS;
}
