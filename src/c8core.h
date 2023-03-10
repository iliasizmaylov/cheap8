// Implementation of Chip-8 core (cpu, memory, etc)
#ifndef _C8CORE_H_
#define _C8CORE_H_

#include "input.h"

#define MEMORY_SIZE					(1 << 12)

// Memory range that contains instructions
#define MEMORY_RANGE_PROGRAM_MIN	0x200
#define MEMORY_RANGE_PROGRAM_MAX	0xFFF
// Memory range that contains a chip 8 fontset
#define MEMORY_RANGE_FONTSET_MIN	0x050
#define MEMORY_RANGE_FONTSET_MAX	0x0A0

#define FONTSET_SIZE				80

#define SCREEN_RESOLUTION_WIDTH		64
#define SCREEN_RESOLUTION_HEIGHT	32
#define SCREEN_TOTAL_PIXELS			SCREEN_RESOLUTION_WIDTH * SCREEN_RESOLUTION_HEIGHT
#define SCREEN_ARRAY_SIZE			SCREEN_TOTAL_PIXELS >> 3

#define STACK_SIZE					16

#define CPU_INSTRUCTIONS_PER_SECOND	720
#define TIMER_DECREASE_FREQUENCY	60

// TODO: Calculate number of ticks per cycle
#define CORE_TICKS_PER_CYCLE		20
#define CORE_TICKS_PER_TIMER		CORE_TICKS_PER_CYCLE * (CPU_INSTRUCTIONS_PER_SECOND / TIMER_DECREASE_FREQUENCY)

typedef enum {
	REG_V0, 
	REG_V1, 
	REG_V2, 
	REG_V3, 
	REG_V4, 
	REG_V5, 
	REG_V6, 
	REG_V7, 
	REG_V8, 
	REG_V9, 
	REG_VA, 
	REG_VB, 
	REG_VC, 
	REG_VD, 
	REG_VE, 
	REG_VF,
	GENERAL_PURPOSE_REGISTERS
} Regname;

#define FONT_ENTITY_SIZE		5

static const unsigned char fontset[FONTSET_SIZE] = { 
  0xF0, 0x90, 0x90, 0x90, 0xF0,	// 0
  0x20, 0x60, 0x20, 0x20, 0x70, // 1
  0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
  0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
  0x90, 0x90, 0xF0, 0x10, 0x10, // 4
  0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
  0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
  0xF0, 0x10, 0x20, 0x40, 0x40, // 7
  0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
  0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
  0xF0, 0x90, 0xF0, 0x90, 0x90, // A
  0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
  0xF0, 0x80, 0x80, 0x80, 0xF0, // C
  0xE0, 0x90, 0x90, 0x90, 0xE0, // D
  0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
  0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

#define CUSTOM_FLAG_REDRAW_PENDING	1 << 0
#define CUSTOM_FLAG_BAD_INPUT		1 << 1
#define CUSTOM_FLAG_BAD_STACK		1 << 2
#define CUSTOM_FLAG_BAD_OPCODE		1 << 3
#define CUSTOM_FLAG_BAD_MEMORY		1 << 4
#define CUSTOM_FLAG_BAD_SP			1 << 5
#define CUSTOM_FLAG_MCR_OPCODE		1 << 6
#define CUSTOM_FLAG_CRITICAL_ERROR	1 << 7

#define CUSTOM_FLAG_MASK_ERROR		0x7F

#define SET_CUSTOM_FLAG(dest, source) 	dest->customFlags |= source
#define UNSET_CUSTOM_FLAG(dest, source)	dest->customFlags &= ~(source)
#define CHECK_CUSTOM_FLAG(dest, source)	dest->customFlags & source
#define GET_WORD(byte1, byte2)			0 | (byte1 << 8) | byte2

typedef struct _C8core {
	BYTE memory[MEMORY_SIZE];				// RAM

	BYTE reg[GENERAL_PURPOSE_REGISTERS];	// General purpose registers V0 - VF
	WORD I;									// Address register
	WORD PC;								// Program counter
	WORD SP;								// Stack pointer

	WORD opcode;							// Current opcode
	WORD xParam;							// X opcode parameter (first register number)
	WORD yParam;							// Y opcode parameter (second register number)
	WORD nParam;							// N opcode paramter (constant)

	WORD stack[STACK_SIZE];					// Stack

	QWORD gfx[SCREEN_RESOLUTION_HEIGHT];	// Array of pixels, pixels are either black or white (zero or one)

	BYTE tDelay;							// Delay 60Hz timer
	BYTE tSound;							// Sound 60Hz timer

	WORD keypadState;						// State of a 16 keys keypad (pressed/not pressed) 

	BYTE customFlags;						// Some custom flags that might come in handy (idk)

	Uint64 prevCycleTicks;					// Ticks (milliseconds) since start til previous cycle
	Uint64 prevTimerTicks;					// Ticks (milliseconds) since last timer decrease
} C8core;

VM_RESULT loadROM(C8core *core, FILE *ROM);
VM_RESULT initCore(C8core **m_core, FILE *ROM);
VM_RESULT destroyCore(C8core **m_core);

#endif
