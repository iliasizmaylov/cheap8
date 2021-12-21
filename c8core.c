#include "c8core.h"

VM_RESULT loadROM(C8core *core, FILE *ROM) {
	WORD loadCursor = MEMORY_RANGE_PROGRAM_MIN;
	BYTE nextByte = 0;

	while (((nextByte = fgetc(ROM)) != (BYTE)EOF) && loadCursor <= MEMORY_RANGE_PROGRAM_MAX) {
		core->memory[loadCursor] = (BYTE) nextByte;
		loadCursor += 1;
	}

	return VM_RESULT_SUCCESS;
}

VM_RESULT initCore(C8core **m_core, FILE *ROM) {
	*m_core = (C8core*) malloc(sizeof(C8core));
	C8core *core = *m_core;

	VM_ASSERT(core == NULL);

	for (WORD i = MEMORY_RANGE_FONTSET_MIN, j = 0; i < MEMORY_RANGE_FONTSET_MAX; i++, j++)
		core->memory[i] = fontset[j];

	for (WORD i = MEMORY_RANGE_PROGRAM_MIN; i < MEMORY_RANGE_FONTSET_MAX; i++)
		core->memory[i] = 0;

	core->PC = MEMORY_RANGE_PROGRAM_MIN;
	core->I = 0;
	core->SP = 0;
	core->tDelay = 0;
	core->tSound = 0;
	core->keypadState = 0;

	core->prevCycleTicks = SDL_GetTicks64();
	core->prevTimerTicks = SDL_GetTicks64();

	for (WORD i = 0; i < SCREEN_RESOLUTION_HEIGHT; i++)
		core->gfx[i] = 0;

	VM_ASSERT(loadROM(core, ROM) != VM_RESULT_SUCCESS);

	return VM_RESULT_SUCCESS;
}

VM_RESULT dumpCustomFlags(C8core *core) {
	VM_ASSERT(core == NULL);

	printf("\nCustom core flags:\n");
	printf("REDRAW_PENDING	: %d\n", core->customFlags & CUSTOM_FLAG_REDRAW_PENDING ? 1 : 0);
	printf("BAD_INPUT		: %d\n", core->customFlags & CUSTOM_FLAG_BAD_INPUT ? 1 : 0);
	printf("BAD_STACK		: %d\n", core->customFlags & CUSTOM_FLAG_BAD_STACK ? 1 : 0);
	printf("BAD_OPCODE		: %d\n", core->customFlags & CUSTOM_FLAG_BAD_OPCODE ? 1 : 0);
	printf("BAD_MEMORY		: %d\n", core->customFlags & CUSTOM_FLAG_BAD_MEMORY ? 1 : 0);
	printf("BAD_SP			: %d\n", core->customFlags & CUSTOM_FLAG_BAD_SP ? 1 : 0);
	printf("MCR_OPCODE		: %d\n", core->customFlags & CUSTOM_FLAG_MCR_OPCODE ? 1 : 0);
	printf("CRITICAL_ERROR	: %d\n\n", core->customFlags & CUSTOM_FLAG_CRITICAL_ERROR ? 1 : 0);

	return VM_RESULT_SUCCESS;
}
