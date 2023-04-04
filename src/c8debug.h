/**
 * Cheap-8: a chip-8 emulator
 * 
 * File: c8debug.h
 * License: DWYW - "Do Whatever You Want"
 * 
 * Functions and data structures for a cheap8 debugger.
 */

#ifndef _C8DEBUG_H_
#define _C8DEBUG_H_

#include "c8core.h"
#include "opcodes.h"

#include <ncurses.h>

#include <panel.h>
#include <form.h>

#include <locale.h>

#define SCREEN_CONTENT_X_OFFSET         1
#define SCREEN_CONTENT_Y_OFFSET         1

#define DEBUGGER_PAUSE_TICKS            10

#define S_LINES \
    (LINES - SCREEN_CONTENT_Y_OFFSET * 2)

#define S_COLS \
    (COLS - SCREEN_CONTENT_X_OFFSET * 2)

// Colors for ncurses interface
enum debuggerColorPairs {
    WINDOW_COLOR_START_NR = 1,

    WINDOW_TITLE_COLOR = 1,
    WINDOW_BORDER_SELECTED_COLOR,
    WINDOW_BORDER_EDITED_COLOR,
    WINDOW_TITLE_SELECTED_COLOR,
    WINDOW_TITLE_EDITED_COLOR,
    WINDOW_MEMORY_END_COLOR,
    WINDOW_MEMORY_OPCODE_COLOR,
    WINDOW_FLAGS_ERROR_COLOR,
    WINDOW_FLAGS_NORMAL_COLOR,
    MENU_BAR_COLOR,
    POPUP_BORDER_COLOR,
    POPUP_TITLE_COLOR,

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
	DEBUG_WINDOW_REGISTERS = 0,         // Window showing state of all registers
	DEBUG_WINDOW_MEMORY,            // Window that acts as a memory explorer
	DEBUG_WINDOW_CUSTOM_FLAGS,      // Window that shows the state of custom flags
    DEBUG_WINDOW_DISASM,            // Disassembler window
    DEBUG_WINDOW_VM,
    DEBUG_WINDOW_PREVIEW,
    DEBUG_WINDOW_STACK,

	DEBUG_WINDOW_COUNT              // A placeholder that contains a count of all aforementioned windows
} DebuggerWindows;

typedef struct _DebuggerWindow DebuggerWindow;
typedef struct _DebuggerPopup DebuggerPopup;
typedef struct _Debugger Debugger;

typedef void (*WINDOW_UPDATE)(Debugger*, DebuggerWindow*, const C8core*);
typedef void (*POPUP_FORM_DRAW)(Debugger*, DebuggerPopup*);
typedef void (*POPUP_FORM_SAVE)(Debugger*, DebuggerPopup*);

void drawWindowBox(Debugger *dbg, DebuggerWindow *window);
void drawPopupBox(Debugger *dbg);

void updateRegisters(Debugger *dbg, DebuggerWindow *window, const C8core *core);
void updateMemory(Debugger *dbg, DebuggerWindow *window, const C8core *core);
void updateCustomFlags(Debugger *dbg, DebuggerWindow *window, const C8core *core);
void updateDisasm(Debugger *dbg, DebuggerWindow *window, const C8core *core);
void updateVMdbg(Debugger *dbg, DebuggerWindow *window, const C8core *core);
void updatePreview(Debugger *dbg, DebuggerWindow *window, const C8core *core);
void updateStack(Debugger *dbg, DebuggerWindow *window, const C8core *core);

// Holds all update handlers for debugger windows
static const WINDOW_UPDATE g_debuggerUpdaters[DEBUG_WINDOW_COUNT] = {
	updateRegisters,
	updateMemory,
	updateCustomFlags,
    updateDisasm,
    updateVMdbg,
    updatePreview,
    updateStack
};

void generateWindowPos(DebuggerWindow *dwin);

typedef struct _DebuggerMenuOption {
    const char *name;
    const int key;
    const char *keystr;
    void (*handler)(Debugger *dbg);
} DebuggerMenuOption;

void gNavHandler(Debugger *dbg);
void gHandler_quit(Debugger *dbg);
void gHandler_back(Debugger *dbg);
void gHandler_select(Debugger *dbg);
void gHandler_stepin(Debugger *dbg);
void gHandler_pauseresume(Debugger *dbg);
void gHandler_loadrom(Debugger *dbg);

void pHandler_cancel(Debugger *dbg);
void pHandler_save(Debugger *dbg);

void wNavHandler_memory(Debugger *dbg);
void wNavHandler_disasm(Debugger *dbg);

void wHandler_mem_goto(Debugger *dbg);

#define GLOBAL_OPTION_COUNT     6
static const DebuggerMenuOption g_opts[GLOBAL_OPTION_COUNT] = {
    {.name = "Quit", .key = 'q', .keystr = "Q", .handler = gHandler_quit},
    {.name = "Back", .key = 27, .keystr = "ESC", .handler = gHandler_back},
    {.name = "Select", .key = 's', .keystr = "ENTER", .handler = gHandler_select},
    {.name = "Step-In", .key = ' ', .keystr = "SPACE", .handler = gHandler_stepin},
    {.name = "Pause/Resume", .key = 'p', .keystr = "P", .handler = gHandler_pauseresume},
    {.name = "Load ROM", .key = 'l', .keystr = "L", .handler = gHandler_loadrom},
};

#define POPUP_OPTION_COUNT      2
static const DebuggerMenuOption g_popup_opts[POPUP_OPTION_COUNT] = {
    {.name = "Cancel", .key = 27, .keystr = "ESC", .handler = pHandler_cancel},
    {.name = "OK", .key = 's', .keystr = "ENTER", .handler = pHandler_save}
};

#define WINDOW_MEMORY_OPTION_COUNT  1
static const DebuggerMenuOption w_mem_opts[WINDOW_MEMORY_OPTION_COUNT] = {
    {.name = "Goto", .key = 'g', .keystr = "G", .handler = wHandler_mem_goto}
};

typedef struct _DebuggerMenu {
    const char  *name;
    const int optcount;
    const DebuggerMenuOption *global_opts;
    const DebuggerMenuOption *opts;
    void (*navhandler)(Debugger *dbg);
} DebuggerMenu;

static const DebuggerMenu g_menus[DEBUG_WINDOW_COUNT] = {
    /* DEBUG_WINDOW_REGISTERS */ {
        .name = "Registers", .global_opts = g_opts, .opts = NULL, .optcount = 0,
        .navhandler = NULL
    },
    /* DEBUG_WINDOW_MEMORY */ {
        .name = "Memory", .global_opts = g_opts, 
        .opts = w_mem_opts, .optcount = WINDOW_MEMORY_OPTION_COUNT,
        .navhandler = wNavHandler_memory
    },
    /* DEBUG_WINDOW_CUSTOM_FLAGS */ {
        .name = "Custom Flags", .global_opts = g_opts, .opts = NULL, .optcount = 0,
        .navhandler = NULL 
    },
    /* DEBUG_WINDOW_DISASM */ {
        .name = "Disassembly", .global_opts = g_opts, .opts = NULL, .optcount = 0,
        .navhandler = wNavHandler_disasm
    },
    /* DEBUG_WINDOW_VM */ {
        .name = "Cheap-8 Display", .global_opts = g_opts, .opts = NULL, .optcount = 0,
        .navhandler = NULL
    },
    /* DEBUG_WINDOW_PREVIEW */ {
        .name = "Preview", .global_opts = g_opts, .opts = NULL, .optcount = 0,
        .navhandler = NULL
    },
    /* DEBUG_WINDOW_STACK */ {
        .name = "Stack", .global_opts = g_opts, .opts = NULL, .optcount = 0,
        .navhandler = NULL
    }
};

void updateMenu(Debugger *dbg, DebuggerWindow *dwin);

#define WINDOW_FLAG_READONLY    (1 << 0)
#define WINDOW_FLAG_PERMANENT   (1 << 1)
#define WINDOW_FLAG_TOUCHED     (1 << 2)

typedef enum {
    WINDOW_LEFT = 0,
    WINDOW_RIGHT,
    WINDOW_UP,
    WINDOW_DOWN,
    WINDOW_SIDES
} WindowSides;

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
    PANEL *pan;

    DebuggerWindow *left;
    DebuggerWindow *right;
    DebuggerWindow *up;
    DebuggerWindow *down;
    DebuggerWindow *adj[WINDOW_SIDES];

    BYTE flags;

    const DebuggerMenu *menu;

    void *auxdata;

    // Pointer to an update function
	WINDOW_UPDATE updateHandler;
} DebuggerWindow;

static DebuggerWindow dbgwnds[DEBUG_WINDOW_COUNT] = {
    {.title = "Registers", .literal = 'r', .flags = WINDOW_FLAG_READONLY},
    {.title = "Memory", .literal = 'm', .flags = 0},
    {.title = "Custom Flags", .literal = 'f', .flags = WINDOW_FLAG_READONLY},
    {.title = "Disassembly", .literal = 'd', .flags = 0},
    {.title = "Cheap-8 Display", .literal = 'v', .flags = 0},
    {.title = "Sprite Preview", .literal = 'p', .flags = WINDOW_FLAG_READONLY},
    {.title = "Stack", .literal = 's', .flags = WINDOW_FLAG_READONLY}
};

typedef struct _MemoryWindowData {
    WORD addr_start;
    WORD addr_end;
    WORD max_cols;
} MemoryWindowData;

typedef struct _DisasmWindowData {
    WORD addr_start;
    WORD addr_end;
} DisasmWindowData;

#define DEBUGGER_POPUP_MAX_FIELDS   3
typedef struct _DebuggerPopup {
    const char *title;

    FORM *form;
    FIELD *fields[DEBUGGER_POPUP_MAX_FIELDS];
    const BYTE field_count;
    BYTE isDrawn;

    POPUP_FORM_DRAW hdraw;
    POPUP_FORM_SAVE hsave;
} DebuggerPopup;

void gPopupExit_ALL(Debugger *dbg, DebuggerPopup *popup);
void gPopupKeys_ALL(Debugger *dbg, DebuggerPopup *popup);

void gPopupDraw_loadrom(Debugger *dbg, DebuggerPopup *popup);
void gPopupSave_loadrom(Debugger *dbg, DebuggerPopup *popup);

void wPopupDraw_findmem(Debugger *dbg, DebuggerPopup *popup);
void wPopupSave_findmem(Debugger *dbg, DebuggerPopup *popup);

void wPopupDraw_findop(Debugger *dbg, DebuggerPopup *popup);
void wPopupSave_findop(Debugger *dbg, DebuggerPopup *popup);

typedef enum {
    DEBUGGER_POPUP_MEM_GOTO = 0,
    
    NR_DEBUGGER_POPUPS
} DebuggerPopupType;

#define DEBUGGER_POPUP_X_POS    (S_COLS / 6)
#define DEBUGGER_POPUP_Y_POS    (S_LINES / 6)
#define DEBUGGER_POPUP_LINES    (S_LINES / 6) * 2
#define DEBUGGER_POPUP_COLS     (S_COLS / 6) * 2

#define DEBUGGER_POPUP_SUB_X    (20)
#define DEBUGGER_POPUP_SUB_Y    (1)
#define DEBUGGER_POPUP_SUB_L    DEBUGGER_POPUP_LINES
#define DEBUGGER_POPUP_SUB_C    (DEBUGGER_POPUP_COLS - DEBUGGER_POPUP_SUB_X)

static DebuggerPopup g_popups[NR_DEBUGGER_POPUPS] = {
    /* DEBUGGER_POPUP_MEM_GOTO */ {
        .title = "Go to Memory Address", .hdraw = wPopupDraw_findmem, 
        .hsave = wPopupSave_findmem, .field_count = 1
    }
};

#define __INIT_AUX_DATA(type) \
    BYTE IS_AUX_NEW = 0; \
    if (window->auxdata == NULL) { \
        window->auxdata = malloc(sizeof(type)); \
        IS_AUX_NEW = 1; \
    }\
    type *AUX_DATA = (type*)window->auxdata; \
    if (IS_AUX_NEW)

#define DEBUGGER_FLAG_STEP_MODE         (1 << 0)
#define DEBUGGER_FLAG_EDITING           (1 << 1)
#define DEBUGGER_FLAG_POPUP             (1 << 5)
#define DEBUGGER_FLAG_NEXT_STEP         (1 << 6)
#define DEBUGGER_FLAG_EXIT              (1 << 7)

#define DEBUGGER_MENU_X_POS             SCREEN_CONTENT_X_OFFSET
#define DEBUGGER_MENU_Y_POS             S_LINES
#define DEBUGGER_MENU_ROWS              S_COLS
#define DEBUGGER_MENU_COLS              1

// A struct (should be a singleton) that defines the whole debugger context
typedef struct _Debugger {
	DebuggerWindow *windows[DEBUG_WINDOW_COUNT];    // Pointer to all DebuggerWindow structs
	const C8core *core;                             // Pointer to a cheap8 core
	BYTE flags;                                     // 8 debugger flags

    DebuggerWindow *current;
    int lastInput;

    WINDOW *menuwin;

    DebuggerPopup *popup;
    WINDOW *popup_win;
    WINDOW *popup_sub;
    PANEL *popup_pan;
} Debugger;

void initMenu(Debugger *dbg);
void updateMenu(Debugger *dbg, DebuggerWindow *dwin);
VM_RESULT updateDebugger(Debugger *dbg);
void initWindow(Debugger *dbg, DebuggerWindow *window);
VM_RESULT initDebugger(Debugger **m_dbg, const C8core *_core);
VM_RESULT destroyDebugger(Debugger **m_dbg);

#endif /* _C8DEBUG_H_ */
