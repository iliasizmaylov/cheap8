/**
 * Cheap-8: a chip-8 emulator
 * 
 * File: c8debug.h
 * License: DWYW - "Do Whatever You Want"
 * 
 * Functions and data structures for a cheap8 debugger.
 * And I think a little more explanation would be merited.
 * 
 * Here is how debugger and it's interface is structured:
 * +-----------------------------+
 * |struct Debugger = {          |
 * |                             |
 * |  windows[] = {              |
 * |  1-+----------------------+ |              
 * |    |struct DebuggerWindow | |               (initializes the window)                                 
 * |    |   Handler function \ | |          /--> void initHandler(DebuggerWindow*)                        
 * |    |            pointer /-+-----------<
 * |    +----------------------+ |          \--> void updateHandler(Debugger*, DebuggerWindow*, C8core*)
 * |            [ ... ]          |               (processes updates to the window and draws them)
 * |  N-+----------------------+ |
 * |    |struct DebuggerWindow | |  
 * |    |   Handler function \ | |          /--> void initHandler(DebuggerWindow*)
 * |    |            pointer /-+-----------<
 * |    +----------------------+ |          \--> void updateHandler(Debugger*, DebuggerWindow*, C8core*)
 * |  }                          |                                                                      
 * |}                            |
 * +-----------------------------+
 */

#ifndef _C8DEBUG_H_
#define _C8DEBUG_H_

#include "c8core.h"
#include "opcodes.h"

#include <ncurses.h>
#include <locale.h>

#define SCREEN_CONTENT_X_OFFSET         1
#define SCREEN_CONTENT_Y_OFFSET         1

#define S_LINES \
    (LINES - SCREEN_CONTENT_Y_OFFSET * 2)

#define S_COLS \
    (COLS - SCREEN_CONTENT_X_OFFSET * 2)

// Colors for ncurses interface
enum debuggerColorPairs {
    WINDOW_COLOR_START_NR = 1,

    WINDOW_TITLE_COLOR = 1,
    WINDOW_MEMORY_END_COLOR,
    WINDOW_MEMORY_OPCODE_COLOR,
    WINDOW_FLAGS_ERROR_COLOR,
    WINDOW_FLAGS_NORMAL_COLOR,

    WINDOW_COLOR_END_NR
};

typedef struct _DebuggerColorPair {
    short fc;
    short bc;
} DebuggerColorPair;

// Offsets for drawing windows and text
#define WINDOW_TITLE_OFFSET				4
#define WINDOW_CONTENT_X_OFFSET			2
#define WINDOW_CONTENT_Y_OFFSET			2

// Offset for a memory window content
#define WINDOW_MEMORY_COLUMN_OFFSET		4

// Special characters for drawing memory window
#define WINDOW_MEMORY_NO_MEM_CHAR		"~~ "
#define WINDOW_MEMORY_END_CHAR			"END"

// Enum for a more convenient way to identify and refer to debugger TUI windows
typedef enum {
	DEBUG_WINDOW_CURRENT_OPCODE=0,  // Window that shows current opcode being executed
	DEBUG_WINDOW_REGISTERS,         // Window showing state of all registers
	DEBUG_WINDOW_CUSTOM_FLAGS,      // Window that shows the state of custom flags
	DEBUG_WINDOW_MEMORY,            // Window that acts as a memory explorer
    DEBUG_WINDOW_DISASM,            // Disassembler window
    DEBUG_WINDOW_VM,

	DEBUG_WINDOW_COUNT              // A placeholder that contains a count of all aforementioned windows
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
void updateVMdbg(Debugger *dbg, DebuggerWindow *window, const C8core *core);

// Holds all update handlers for debugger windows
static const WINDOW_UPDATE g_debuggerUpdaters[DEBUG_WINDOW_COUNT] = {
	updateCurrentOpcode,
	updateRegisters,
	updateMemory,
	updateCustomFlags,
    updateDisasm,
    updateVMdbg
};

typedef void (*WINDOW_INIT)(DebuggerWindow*);

void initCurrentOpcode(DebuggerWindow *window);
void initRegisters(DebuggerWindow *window);
void initMemory(DebuggerWindow *window);
void initCustomFlags(DebuggerWindow *window);
void initDisasm(DebuggerWindow *window);
void initVMdbg(DebuggerWindow *window);

// Holds all init handlers for debugger windows
static const WINDOW_INIT g_debuggerInits[DEBUG_WINDOW_COUNT] = {
	initCurrentOpcode,
	initRegisters,
	initMemory,
	initCustomFlags,
    initDisasm,
    initVMdbg
};

void generateWindowPos(DebuggerWindow *dwin);
void drawBlock(WINDOW *win, int y, int x, int w, int h);

// A struct defining all debugger window properties
typedef struct _DebuggerWindow {
	int lines;          // Number of lines it occupies on the screen (height)
	int columns;        // Number of columns it occupies on the screen (width)
	int xPos;           // Starting position of a window at x axis
	int yPos;           // Starting position of a window at y axis

	int textLines;      // Number of lines (height) the actual content occupies within the window

	const char *title;  // Window title
    char literal;       // A literal corresponding to current window in c8debug_layout.h

	WINDOW *win;        // ncurses window handler

    // Pointer to an update function
	WINDOW_UPDATE updateHandler;

    // Pointer to an init function
	WINDOW_INIT initHandler;
} DebuggerWindow;

#define FLAG_STEP_MODE      1 << 0
#define FLAG_STOP_ON_ERROR  1 << 1

// A struct (should be a singleton) that defines the whole debugger context
typedef struct _Debugger {
	DebuggerWindow *windows[DEBUG_WINDOW_COUNT];    // Pointer to all DebuggerWindow structs
	const C8core *core;                             // Pointer to a cheap8 core
	BYTE flags;                                     // 8 debugger flags

    DebuggerWindow *current;
    char lastInput;
} Debugger;

VM_RESULT updateDebugger(Debugger *dbg);
VM_RESULT initDebugger(Debugger **m_dbg, const C8core *_core);
VM_RESULT destroyDebugger(Debugger **m_dbg);

#endif /* _C8DEBUG_H_ */
