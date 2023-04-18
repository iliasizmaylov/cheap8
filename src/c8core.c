/**
 * Cheap-8: a chip-8 emulator
 *
 * File: c8core.c
 * License: DWYW - "Do Whatever You Want"
 *
 * Function definitions from c8core.h
 */

#include "c8core.h"

/** loadROM
 *
 * @param: C8core *core
 *  Pointer to C8core struct for which to load
 *  a given ROM filed
 * @param: FILE *ROM
 *  File handler for a ROM file to be loaded
 *
 * Loads contents of a ROM file into core memory
 * Called from initCore() function
 */
VM_RESULT loadROM(C8core *core, FILE *ROM) {
	WORD loadCursor = MEMORY_RANGE_PROGRAM_MIN;
	BYTE nextByte = 0;

    // Loading all memory from ROM straign into an
    // array in core struct
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

/** initCore
 *
 * @param: C8core **m_core
 *  C8Core struct pointer passed by refernce
 *  so it could be allocated and initialized
 * @param: FILE *ROM
 *  ROM file to be used when initializing a C8core struct
 */
VM_RESULT initCore(C8core **m_core, FILE *ROM) {
	*m_core = (C8core*) malloc(sizeof(C8core));
	C8core *core = *m_core;

	VM_ASSERT(core == NULL);

    // Zero all the memory first
	for (WORD i = 0; i < MEMORY_SIZE; i++) {
		core->memory[i] = 0;
	}

    // Load fonts into fontset part of the core memory
	for (WORD i = MEMORY_RANGE_FONTSET_MIN, j = 0; i <= MEMORY_RANGE_FONTSET_MAX; i++, j++) {
		core->memory[i] = fontset[j];
	}

    // Zero all the core registers
	for (BYTE i = 0; i < GENERAL_PURPOSE_REGISTERS; i++) {
		core->reg[i] = 0;
	}

    // Put program counter at the beginning of a code segment
	core->PC = MEMORY_RANGE_PROGRAM_MIN;

    // Zero all current status params and registers
	core->opcode = 0;
	core->I = 0;
	core->SP = 0;
	core->tDelay = 0;
	core->tSound = 0;
	core->keypadState = 0;

    // Zero system clock (in ticks)
    // TODO: When adding ncurses implementation this needs to change
	core->prevCycleTicks = SDL_GetTicks64();
	core->prevTimerTicks = SDL_GetTicks64();

    // Zero custom flags
	core->customFlags = 0;

    // Clear the screen (set all pixels to black)
	for (WORD i = 0; i < SCREEN_RESOLUTION_HEIGHT; i++)
		core->gfx[i] = 0;

    // Load ROM file into memory
	VM_ASSERT(loadROM(core, ROM) != VM_RESULT_SUCCESS);

	return VM_RESULT_SUCCESS;
}

// Free memory allocated for core struct
// No elaborate description needed
VM_RESULT destroyCore(C8core **m_core) {
	VM_ASSERT(*m_core == NULL);
	free(*m_core);

	return VM_RESULT_SUCCESS;
}
