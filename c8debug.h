#ifndef _C8DEBUG_H_
#define _C8DEBUG_H_

#include "c8core.h"
#include <panel.h>
#include <ncurses.h>

#define WINDOW_TITLE_COLOR				1
#define WINDOW_MEMORY_END_COLOR			2
#define WINDOW_MEMORY_OPCODE_COLOR		3
#define WINDOW_FLAGS_ERROR_COLOR		4
#define WINDOW_FLAGS_NORMAL_COLOR		5

#define WINDOW_TITLE_OFFSET				4
#define WINDOW_CONTENT_X_OFFSET			1
#define WINDOW_CONTENT_Y_OFFSET			2

#define WINDOW_MEMORY_COLUMN_OFFSET		5

#define WINDOW_MEMORY_NO_MEM_CHAR		"~~"
#define WINDOW_MEMORY_END_CHAR			"END"

typedef enum {
	DEBUG_WINDOW_CURRENT_OPCODE,
	DEBUG_WINDOW_REGISTERS,
	DEBUG_WINDOW_MEMORY,
	DEBUG_WINDOW_CUSTOM_FLAGS,
	DEBUG_WINDOW_COUNT
} DebuggerWindows;

typedef struct _DebuggerWindow DebuggerWindow;
typedef void (*WINDOW_UPDATE)(DebuggerWindow*, const C8core*);

void drawWindowBox(DebuggerWindow *window);

void updateCurrentOpcode(DebuggerWindow *window, const C8core *core);
void updateRegisters(DebuggerWindow *window, const C8core *core);
void updateMemory(DebuggerWindow *window, const C8core *core);
void updateCustomFlags(DebuggerWindow *window, const C8core *core);

static const WINDOW_UPDATE g_debuggerUpdaters[DEBUG_WINDOW_COUNT] = {
	updateCurrentOpcode,
	updateRegisters,
	updateMemory,
	updateCustomFlags
};

typedef void (*WINDOW_INIT)(DebuggerWindow*);

void initCurrentOpcode(DebuggerWindow *window);
void initRegisters(DebuggerWindow *window);
void initMemory(DebuggerWindow *window);
void initCustomFlags(DebuggerWindow *window);

static const WINDOW_INIT g_debuggerInits[DEBUG_WINDOW_COUNT] = {
	initCurrentOpcode,
	initRegisters,
	initMemory,
	initCustomFlags
};

typedef struct _DebuggerWindow {
	int lines;
	int columns;
	int xPos;
	int yPos;

	int textLines;

	const char *title;

	WINDOW *win;
	PANEL *pan;

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
