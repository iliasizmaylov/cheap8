/*
 * Cheap-8: a chip-8 emulator
 * 
 * File: c8debug.h
 * License: DWYW - "Do Whatever You Want"
 * 
 * Functions and data structures for a cheap8 debugger
 */

#ifndef _C8DEBUG_H_
#define _C8DEBUG_H_

#include "c8core.h"
#include "opcodes.h"

#include <ncurses.h>
#include <stdarg.h>

// Colors for ncurses interface
// TODO: Probably it's best to represent these in a struct
#define WINDOW_TITLE_COLOR				1
#define WINDOW_MEMORY_END_COLOR			2
#define WINDOW_MEMORY_OPCODE_COLOR		3
#define WINDOW_FLAGS_ERROR_COLOR		4
#define WINDOW_FLAGS_NORMAL_COLOR		5

// Offsets for drawing windows and text
// TODO: Also probably better to store it in structs to be more tidy
#define WINDOW_TITLE_OFFSET				4
#define WINDOW_CONTENT_X_OFFSET			2
#define WINDOW_CONTENT_Y_OFFSET			2

// Offset for a memory window content
// TODO: You guessed it... (move to struct)
#define WINDOW_MEMORY_COLUMN_OFFSET		4

// Special characters for drawing memory window
// TODO: Aaaaand y'guessed it yet again...
#define WINDOW_MEMORY_NO_MEM_CHAR		"~~ "
#define WINDOW_MEMORY_END_CHAR			"END"

// Enum for a more convenient way to identify and refer to debugger TUI windows
typedef enum {
	DEBUG_WINDOW_CURRENT_OPCODE,
	DEBUG_WINDOW_REGISTERS,
	DEBUG_WINDOW_MEMORY,
	DEBUG_WINDOW_CUSTOM_FLAGS,
    DEBUG_WINDOW_DISASM,
	DEBUG_WINDOW_COUNT
} DebuggerWindows;

typedef struct _DebuggerWindow DebuggerWindow;
typedef struct _Debugger Debugger;
typedef void (*WINDOW_UPDATE)(Debugger*, DebuggerWindow*, const C8core*);

void drawWindowBox(DebuggerWindow *window);

void updateCurrentOpcode(Debugger *dbg, DebuggerWindow *window, const C8core *core);
void updateRegisters(Debugger *dbg, DebuggerWindow *window, const C8core *core);
void updateMemory(Debugger *dbg, DebuggerWindow *window, const C8core *core);
void updateCustomFlags(Debugger *dbg, DebuggerWindow *window, const C8core *core);
void updateDisasm(Debugger *dbg, DebuggerWindow *window, const C8core *core);

static const WINDOW_UPDATE g_debuggerUpdaters[DEBUG_WINDOW_COUNT] = {
	updateCurrentOpcode,
	updateRegisters,
	updateMemory,
	updateCustomFlags,
    updateDisasm
};

typedef void (*WINDOW_INIT)(DebuggerWindow*);

void initCurrentOpcode(DebuggerWindow *window);
void initRegisters(DebuggerWindow *window);
void initMemory(DebuggerWindow *window);
void initCustomFlags(DebuggerWindow *window);
void initDisasm(DebuggerWindow *window);

static const WINDOW_INIT g_debuggerInits[DEBUG_WINDOW_COUNT] = {
	initCurrentOpcode,
	initRegisters,
	initMemory,
	initCustomFlags,
    initDisasm
};

typedef struct _DebuggerWindow {
	int lines;
	int columns;
	int xPos;
	int yPos;

	int textLines;

	const char *title;

	WINDOW *win;

	WINDOW_UPDATE updateHandler;
	WINDOW_INIT initHandler;
} DebuggerWindow;

typedef struct _Debugger {
	DebuggerWindow *windows[DEBUG_WINDOW_COUNT];
	const C8core *core;
	BYTE isStepMode;
} Debugger;

VM_RESULT updateDebugger(Debugger *dbg);
VM_RESULT initDebugger(Debugger **m_dbg, const C8core *_core);
VM_RESULT destroyDebugger(Debugger **m_dbg);

#endif
