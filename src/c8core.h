/**
 * Cheap-8: a chip-8 emulator
 * 
 * File: c8core.h
 * License: DWYW - "Do Whatever You Want"
 *
 * Declaring and describing all functions 
 * and data structures for a Cheap8 core
 */

#ifndef _C8CORE_H_
#define _C8CORE_H_

#include "input.h"

// Total memory (should be 4K or 4096 bytes)
#define MEMORY_SIZE					(1 << 12)

// Memory range that contains instructions
#define MEMORY_RANGE_PROGRAM_MIN	0x200
#define MEMORY_RANGE_PROGRAM_MAX	0xFFF

// Memory range that contains a chip 8 fontset
#define MEMORY_RANGE_FONTSET_MIN	0x050
#define MEMORY_RANGE_FONTSET_MAX	0x0A0

#define FONTSET_SIZE				80

// Screen parameterss
#define SCREEN_RESOLUTION_WIDTH		64
#define SCREEN_RESOLUTION_HEIGHT	32
#define SCREEN_TOTAL_PIXELS			SCREEN_RESOLUTION_WIDTH * SCREEN_RESOLUTION_HEIGHT
#define SCREEN_ARRAY_SIZE			SCREEN_TOTAL_PIXELS >> 3

// Maximum stack depth
#define STACK_SIZE					16

// Some constants to control execution speed
// according to (approximately) how it was in the original machine
// TODO: probably also needs to change after adding ncurses display support
#define CPU_INSTRUCTIONS_PER_SECOND	720
#define TIMER_DECREASE_FREQUENCY	60

// Number of ticks per cycle and formula to get ticks per timer hit
// given CORE_TICKS_PER_CYCLE
#define CORE_TICKS_PER_CYCLE		10
#define CORE_TICKS_PER_TIMER		CORE_TICKS_PER_CYCLE * (CPU_INSTRUCTIONS_PER_SECOND / TIMER_DECREASE_FREQUENCY)

// Enum to easily access and identifiy register
// in a register array in the C8core struct
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
    
    // Token enum entry that holds a count of all registers available
	GENERAL_PURPOSE_REGISTERS
} Regname;

// Size (in bytes) of a font entry in a fontset in core memory
#define FONT_ENTITY_SIZE		5

/**
 * The default fontset
 * Each row represents 5 lines of 8 bits each
 * where each bit corresponds to a set or unset pixel on the screen
 */
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

// Custom flags (mostly unused at this point)
#define CUSTOM_FLAG_REDRAW_PENDING	1 << 0
#define CUSTOM_FLAG_BAD_INPUT		1 << 1
#define CUSTOM_FLAG_BAD_STACK		1 << 2
#define CUSTOM_FLAG_BAD_OPCODE		1 << 3
#define CUSTOM_FLAG_BAD_MEMORY		1 << 4
#define CUSTOM_FLAG_BAD_SP			1 << 5
#define CUSTOM_FLAG_MCR_OPCODE		1 << 6
#define CUSTOM_FLAG_CRITICAL_ERROR	1 << 7

// If any of the custom flags are set under this mask
// then it means that something went wrong
#define CUSTOM_FLAG_MASK_ERROR		0x7F

// Various macroses (again, mostly unused) to manipulate custom flags
#define SET_CUSTOM_FLAG(dest, source) 	dest->customFlags |= source
#define UNSET_CUSTOM_FLAG(dest, source)	dest->customFlags &= ~(source)
#define CHECK_CUSTOM_FLAG(dest, source)	dest->customFlags & source

// Macro to concatenate to bytes into one word
#define GET_WORD(byte1, byte2)			(0 | (byte1 << 8) | byte2)

// Macro to get a certain bit from any integer value
#define GET_BIT(val, bitidx) \
   ((val >> bitidx) & 1)

// Macro to get a bit from a value in big endian fashion (in reverse)
#define GET_BIT_BE(val, bitidx) \
    ((val >> ((sizeof(val)*8) - bitidx - 1)) & 1)

// C8core struct representing all core parameters and elements
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

    BYTE instrToTimer;
	Uint64 prevTimerTicks;					// Ticks (milliseconds) since last timer decrease
} C8core;

// Load a ROM file into core memory
VM_RESULT loadROM(C8core *core, FILE *ROM);

// Initialize core struct with a given ROM file
VM_RESULT initCore(C8core **m_core, FILE *ROM);

// Free struct C8core
VM_RESULT destroyCore(C8core **m_core);

#endif  /* _C8CORE_H_ */
