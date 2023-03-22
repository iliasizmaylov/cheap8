/**
 * Cheap-8: a chip-8 emulator
 * 
 * File: c8debug.c
 * License: DWYW - "Do Whatever You Want"
 * 
 * Functions and data structures for a cheap8 debugger
 */

#include "c8debug.h"
#include "c8debug_layout.h"
#include "c8comp.h"

/* ======================= GENERIC FUNCTIONS ======================= */

/** drawWindowBox
 *
 * @param window
 *  pointer to DebuggerWindow struct for which the window border
 *  is to be drawn
 * @description:
 *  Generic function that can be used with any debugger window
 *  that draws a border around the window and window title
 */
void drawWindowBox(DebuggerWindow *window) {
	box(window->win, 0, 0);
	wattron(window->win, COLOR_PAIR(WINDOW_TITLE_COLOR));
	mvwprintw(window->win, 0, WINDOW_TITLE_OFFSET, window->title);
	wattroff(window->win, COLOR_PAIR(WINDOW_TITLE_COLOR));
    wmove(window->win, WINDOW_CONTENT_Y_OFFSET, WINDOW_CONTENT_X_OFFSET);
}

void generateWindowPos(DebuggerWindow *dwin) {
    dwin->xPos = 0;
    dwin->yPos = 0;
    dwin->lines = 0;
    dwin->columns = 0;

    int xChunk = -1;
    int yChunk = -1;

    for (int i = 0; i < DEFAULT_LAYOUT_ROWS; i++) {
        for (int j = 0; j < DEFAULT_LAYOUT_COLS; j++) {
            if (g_debuggerLayout[i * DEFAULT_LAYOUT_ROWS + j] == dwin->literal) {
                if (xChunk < 0) {
                    dwin->xPos = j * SCREEN_CHUNK_COLS;
                    xChunk = j;
                    dwin->columns = SIZE_CHUNKS_COLS(xChunk, (j + 1));
                } else {
                    if (dwin->columns < SIZE_CHUNKS_COLS(xChunk, (j + 1)))
                        dwin->columns = SIZE_CHUNKS_COLS(xChunk, (j + 1));
                }

                if (yChunk < 0) {
                    dwin->yPos = i * SCREEN_CHUNK_LINES;
                    yChunk = i;
                    dwin->lines = SIZE_CHUNKS_LINES(yChunk, (i + 1));
                } else {
                    if (dwin->lines < SIZE_CHUNKS_LINES(yChunk, (i + 1)))
                        dwin->lines = SIZE_CHUNKS_LINES(yChunk, (i + 1));
                }
            }
        }
    }

    dwin->xPos += SCREEN_CONTENT_X_OFFSET;
    dwin->yPos += SCREEN_CONTENT_Y_OFFSET;

    dwin->textLines = dwin->lines - 2;
}

void drawBlock(WINDOW *win, int y, int x, int w, int h) {
    for (int i = 0; i < h; i++) {
        wmove(win, y + i, x);
        for (int j = 0; j < w; j++) {
            wprintw(win, "%s", "▓");
        }
    }
}

/* ======================= INIT HANDLERS =========================== */

/** initCurrentOpcode
 * 
 * @param window
 *  Pointer to DebuggerWindow struct representing a current opcode window
 * @description:
 *  An init handler for a current opcode window
 */
void initCurrentOpcode(DebuggerWindow *window) {
	window->title = " Current Opcode ";
    window->literal = 'o';

    generateWindowPos(window);

    if (window->lines == 0 && window->columns == 0) {
        window->win = NULL;
        return;
    }

	window->win = newwin(window->lines, window->columns, window->yPos, window->xPos);

	drawWindowBox(window);
}

/** initRegisters
 *
 * @param window
 *  Pointer to DebuggerWindow struct representing a register state window
 * @description:
 *  An init handler for a register state window
 */
void initRegisters(DebuggerWindow *window) {
	window->title = " Registers ";
    window->literal = 'r';

    generateWindowPos(window);

    if (window->lines == 0 && window->columns == 0) {
        window->win = NULL;
        return;
    }

	window->win = newwin(window->lines, window->columns, window->yPos, window->xPos);

	drawWindowBox(window);
}

/** initMemory
 *
 * @param window
 *  Pointer to a DebuggerWindow struct representing a memory explorer window
 * @description:
 *  An init handler for a memory explorer window
 */
void initMemory(DebuggerWindow *window) {
	window->title = " Memory ";
    window->literal = 'm';

    generateWindowPos(window);

    if (window->lines == 0 && window->columns == 0) {
        window->win = NULL;
        return;
    }

	window->win = newwin(window->lines, window->columns, window->yPos, window->xPos);

	drawWindowBox(window);
}

/** initCustomFlags
 *
 * @param window
 *  Pointer to a DebuggerWindow struct representing a custom flag state window
 * @description:
 *  An init handler for a custom flag state window
 */
void initCustomFlags(DebuggerWindow *window) {
	window->title = " Custom Flags ";
    window->literal = 'f';

    generateWindowPos(window);

    if (window->lines == 0 && window->columns == 0) {
        window->win = NULL;
        return;
    }

	window->win = newwin(window->lines, window->columns, window->yPos, window->xPos);

	drawWindowBox(window);
}

/** initDisasm
 *
 * @param window
 *  Pointer to a DebuggerWindow struct representing a disassembler window
 * @description:
 *  An init handler for a disassembler window
 */
void initDisasm(DebuggerWindow *window) {
    window->title = " Disassembly ";
    window->literal = 'd';

    generateWindowPos(window);

    if (window->lines == 0 && window->columns == 0) {
        window->win = NULL;
        return;
    }

    window->win = newwin(window->lines, window->columns, window->yPos, window->xPos);

    drawWindowBox(window);
}

void initVMdbg(DebuggerWindow *window) {
    window->title = " Cheap-8 Display ";
    window->literal = 'v';

    generateWindowPos(window);

    if (window->lines == 0 && window->columns == 0) {
        window->win = NULL;
        return;
    }

    window->win = newwin(window->lines, window->columns, window->yPos, window->xPos);

    drawWindowBox(window);
}

/* ======================= UPDATE HANDLERS ========================= */

/** updateCurrentOpcode
 *
 * @param dbg
 *  Pointer to a Debugger struct representing the debugger context
 * @param window
 *  Pointer to a DebuggerWindow struct representing a current opcode window
 * @param core
 *  Pointer to a C8core struct representing a chip-8 system core state
 * @description:
 *  Update handler for a current opcode window
 *  Fetches current opcore from core->PC amd displays brief info about it
 */
void updateCurrentOpcode(Debugger *dbg, DebuggerWindow *window, const C8core* core) {
	window->textLines = 1;
	mvwprintw(window->win, WINDOW_CONTENT_Y_OFFSET, WINDOW_CONTENT_X_OFFSET, "OPCODE: ");
	wattron(window->win, COLOR_PAIR(WINDOW_MEMORY_OPCODE_COLOR));
	wprintw(window->win, "%04X", core->opcode);
	wattroff(window->win, COLOR_PAIR(WINDOW_MEMORY_OPCODE_COLOR));
	wprintw(window->win, " | PC: %04X | ", core->PC);
	
	wprintw(window->win, "X Param: ");
	if (core->xParam == 0xFFFF) {
		wprintw(window->win, "NULL | ");
	} else {
		wprintw(window->win, "0x%02X | ", core->xParam);
	}

	wprintw(window->win, "Y Param: ");
	if (core->yParam == 0xFFFF) {
		wprintw(window->win, "NULL | ");
	} else {
		wprintw(window->win, "0x%02X | ", core->yParam);
	}

	wprintw(window->win, "N Param: ");
	if (core->nParam == 0xFFFF) {
		wprintw(window->win, "NULL | ");
	} else {
		wprintw(window->win, "0x%02X | ", core->nParam);
	}

    mvwprintw(window->win, WINDOW_CONTENT_Y_OFFSET + 1, WINDOW_CONTENT_X_OFFSET, 
                "Description: %s", OPCODES[getOpcodeIndex(core->opcode)].description);
}

/** updateRegisters
 *
 * @param dbg
 *  Pointer to Debugger struct representing a debugger context
 * @param window
 *  Pointer to a DebuggerWindow struct representing a register state window
 * @param core
 *  Pointer to a C8core struct representing chip-8 system core
 * @description:
 *  Update handler for a register state window
 *  Fetches data from chip-8 registers and displays them
 */
void updateRegisters(Debugger *dbg, DebuggerWindow *window, const C8core* core) {
	window->textLines = window->lines - 3;
	static const char *regnames[GENERAL_PURPOSE_REGISTERS] = {
		"V0", "V1", "V2", "V3", "V4",
		"V5", "V6", "V7", "V8", "V9",
		"VA", "VB", "VC", "VD", "VE",
		"VF"
	};
	BYTE col = 0;
	BYTE row = 0;
	for (BYTE i = 0; i < GENERAL_PURPOSE_REGISTERS; i++) {
		mvwprintw(window->win, WINDOW_CONTENT_Y_OFFSET + row, WINDOW_CONTENT_X_OFFSET + col * 16, 
				"%s = 0x%02X (%d)   ", regnames[i], core->reg[i], core->reg[i]);
		row += 1;
		if (row > window->textLines) {
			row = 0;
			col += 1;
		}
	}
}

/** updateMemory
 *
 * @param dbg
 *  Pointer to Debugger struct representing a debugger context
 * @param window
 *  Pointer to DebuggerWindow struct representing a memory explorer window
 * @param core
 *  Pointer to C8core struct representing a chip-8 system core
 * @description:
 *  Update handler for memory explorer window
 *  Fetches memory from a current position given by core->PC register and displays it
 */
void updateMemory(Debugger *dbg, DebuggerWindow *window, const C8core* core) {
	WORD maxColumns = (window->columns - 2) / (WINDOW_MEMORY_COLUMN_OFFSET);
	BYTE memEnd = 0;

	for (WORD i = 0; i < window->textLines; i++) {
		for (WORD j = i * maxColumns, k = 0; j < (i + 1) * maxColumns; j++, k++) {
			QWORD nextMem = core->PC + j - 1;
			if (j == i * maxColumns) {
				wattron(window->win, COLOR_PAIR(WINDOW_MEMORY_END_COLOR));
				mvwprintw(window->win, WINDOW_CONTENT_Y_OFFSET + i, 
                            WINDOW_CONTENT_X_OFFSET + (k * WINDOW_MEMORY_COLUMN_OFFSET), 
						    "%03X:", nextMem);
				wattroff(window->win, COLOR_PAIR(WINDOW_MEMORY_END_COLOR));
			} else {
				if (nextMem <= MEMORY_RANGE_PROGRAM_MAX) {
					if (j >= (i * maxColumns) + 1 && j <= (i * maxColumns) + 2 && i == 0) {
						wattron(window->win, COLOR_PAIR(WINDOW_MEMORY_OPCODE_COLOR));
					}
					mvwprintw(window->win, WINDOW_CONTENT_Y_OFFSET + i, 
                                WINDOW_CONTENT_X_OFFSET + (k * WINDOW_MEMORY_COLUMN_OFFSET) + 1, 
							    "%02X ", core->memory[nextMem]);
					wattroff(window->win, COLOR_PAIR(WINDOW_MEMORY_OPCODE_COLOR));
					
				} else {
					if (!memEnd) {
						wattron(window->win, COLOR_PAIR(WINDOW_MEMORY_END_COLOR));
						mvwprintw(window->win, WINDOW_CONTENT_Y_OFFSET + i, 
                                    WINDOW_CONTENT_X_OFFSET + (k * WINDOW_MEMORY_COLUMN_OFFSET) + 1, 
								    WINDOW_MEMORY_END_CHAR);
						wattroff(window->win, COLOR_PAIR(WINDOW_MEMORY_END_COLOR));
					} else {
						mvwprintw(window->win, WINDOW_CONTENT_Y_OFFSET + i, 
                                    WINDOW_CONTENT_X_OFFSET + (k * WINDOW_MEMORY_COLUMN_OFFSET) + 1, 
								    WINDOW_MEMORY_NO_MEM_CHAR);
					}
					memEnd = 1;
				}
			}
		}
	}
}

/** updateCustomFlags
 *
 * @param dbg
 *  Pointer to Debugger struct representing a debugger context
 * @param window
 *  Pointer to DebuggerWindow struct representing a custom flags state window
 * @param core
 *  Pointer to C8core struct representing a chip-8 system core state
 * @description:
 *  Update handler for a custom flag state window
 *  Fetches custom flags from core and displays them
 */
void updateCustomFlags(Debugger *dbg, DebuggerWindow *window, const C8core *core) {
	window->textLines = window->lines - 3;

	static const char *flagnames[8] = {
		"REDRAW_PENDING",
		"BAD_INPUT",
		"BAD_STACK",
		"BAD_OPCODE",
		"BAD_MEMORY",
		"BAD_SP",
		"MCR_OPCODE",
		"CRITICAL_ERROR"
	};

	for (BYTE i = 0; i < 8; i++) {
		if ((core->customFlags >> i) & 1) {
			wattron(window->win, A_BOLD);
			if (i == 0) {
				wattron(window->win, COLOR_PAIR(WINDOW_FLAGS_NORMAL_COLOR));
			} else {
				wattron(window->win, COLOR_PAIR(WINDOW_FLAGS_ERROR_COLOR));
			}
			mvwprintw(window->win, WINDOW_CONTENT_Y_OFFSET + i, WINDOW_CONTENT_X_OFFSET, 
                        "%s", flagnames[i]);
			mvwprintw(window->win, WINDOW_CONTENT_Y_OFFSET + i, WINDOW_CONTENT_X_OFFSET + 16, 
                        "(*)", flagnames[i]);
		} else {
			mvwprintw(window->win, WINDOW_CONTENT_Y_OFFSET + i, WINDOW_CONTENT_X_OFFSET, 
                        "%s", flagnames[i]);
			mvwprintw(window->win, WINDOW_CONTENT_Y_OFFSET + i, WINDOW_CONTENT_X_OFFSET + 16, 
                        "( )  ", flagnames[i]);
		}
		wattroff(window->win, A_BOLD);
		wattroff(window->win, COLOR_PAIR(WINDOW_FLAGS_NORMAL_COLOR));
		wattroff(window->win, COLOR_PAIR(WINDOW_FLAGS_ERROR_COLOR));
	}
}

/** updateDisasm
 *
 * @param dbg
 *  Pointer to Debugger struct representing a debugger context
 * @param window
 *  Pointer to DebuggerWindow struct representing a disassembler window
 * @param core
 *  Pointer to C8core struct representing a chip-8 system core state
 * @description:
 *  Update handler for a disassembler window
 */
void updateDisasm(Debugger *dbg, DebuggerWindow *window, const C8core *core) { 
    const char *fmt_current = "=>0x%03X  %-20s# %s";
    const char *fmt_not_current = "  0x%03X  %-20s# %s";
    const char *fmt_selected = fmt_current;

    for (int i = 0; i < window->textLines; i++) {
        const Instruction *current = getInstructionAt(core->PC + i * 2);

        if (current->op != NULL) {
            fmt_selected = i == 0 ? fmt_current : fmt_not_current;
            mvwprintw(window->win, WINDOW_CONTENT_Y_OFFSET + i, WINDOW_CONTENT_X_OFFSET, 
                            fmt_selected, current->addr, current->asmstr, current->readable); 
        } else {
            mvwprintw(window->win, WINDOW_CONTENT_Y_OFFSET + i, WINDOW_CONTENT_X_OFFSET, "N/A");
        }
    }
}

void updateVMdbg(Debugger *dbg, DebuggerWindow *window, const C8core *core) {
    QWORD currentRow = 0;

    BYTE pxh = window->lines / SCREEN_RESOLUTION_HEIGHT;
    BYTE pxw = window->columns / SCREEN_RESOLUTION_WIDTH;

    for (BYTE i = 0; i < SCREEN_RESOLUTION_HEIGHT; i++) {
        currentRow = core->gfx[i];
        for (BYTE j = 0; j < SCREEN_RESOLUTION_WIDTH; j++) {
            if (GET_BIT_BE(currentRow, j)) {
                drawBlock(window->win, WINDOW_CONTENT_Y_OFFSET + i * pxh, WINDOW_CONTENT_X_OFFSET + j * pxw,
                            pxw, pxh);
            }
        }
    }

    return;
};

/* ==================== DEBUGGER CONTEXT FUNCTIONS ================= */

/** updateDebugger
 *
 * @param dbg
 *  Pointer to a Debugger struct representing a debugger context
 * @description:
 *  Calls all window update handlers from a given debugger context
 *  therefore updating the whole debugger context
 */
VM_RESULT updateDebugger(Debugger *dbg) {
	for (BYTE i = 0; i < DEBUG_WINDOW_COUNT; i++) {
        if (dbg->windows[i]->win != NULL) {
            werase(dbg->windows[i]->win);
            dbg->windows[i]->updateHandler(dbg, dbg->windows[i], dbg->core);
            drawWindowBox(dbg->windows[i]);
            wrefresh(dbg->windows[i]->win);
        }
	}

    dbg->lastInput = (char)getch();
    VM_RESULT ret = VM_RESULT_SUCCESS;

    if (dbg->flags & FLAG_STEP_MODE)
        ret = VM_RESULT_DBG;

    if (dbg->lastInput != ERR)
        ret = VM_RESULT_SUCCESS;

	doupdate();

	return ret;
}

static const DebuggerColorPair dbgColorPairs[WINDOW_COLOR_END_NR] = {
    /* Placeholder for 0th color pair that is reserved by curses lib*/ {
        .fc = 0, .bc = 0
    },
    /* WINDOW_TITLE_COLOR */ {
        .fc = COLOR_BLACK, .bc = COLOR_WHITE
    },
    /* WINDOW_MEMORY_END_COLOR */ {
        .fc = COLOR_BLACK, .bc = COLOR_WHITE
    },
    /* WINDOW_MEMORY_OPCODE_COLOR */ {
        .fc = COLOR_GREEN, .bc = COLOR_BLACK
    },
    /* WINDOW_FLAGS_ERROR_COLOR */ {
        .fc = COLOR_RED, .bc = COLOR_BLACK
    },
    /* WINDOW_FLAGS_NORMAL_COLOR */ {
        .fc = COLOR_GREEN, .bc = COLOR_BLACK
    }
};

/** initDebugger
 *
 * @param m_dbg
 *  Reference to a pointer to Debugger struct representing a 
 *  debugger context that is to be initialized
 * @param _core
 *  Pointer to a C8core struct representing a chip-8 system core state
 * @description:
 *  Populates Debugger struct corresponding to a given debugger context
 *  with initial data and parameters and calls all init handlers for 
 *  all debugger windows in a given debugger context
 */
VM_RESULT initDebugger(Debugger **m_dbg, const C8core *_core) {
	VM_ASSERT(_core == NULL);

	*m_dbg = NULL;
	*m_dbg = (Debugger*) malloc(sizeof(Debugger));

	VM_ASSERT(*m_dbg == NULL);

    disassemble(_core);

	Debugger *dbg = *m_dbg;
	dbg->core = _core;
	dbg->flags = FLAG_STEP_MODE | FLAG_STOP_ON_ERROR;

    dbg->current = NULL;
    dbg->lastInput = ERR;

    setlocale(LC_ALL, "");

	initscr();
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);
	start_color();
	cbreak();
	noecho();
	curs_set(0);

    for (BYTE i = WINDOW_COLOR_START_NR; i < WINDOW_COLOR_END_NR; i++) {
        init_pair(i, dbgColorPairs[i].fc, dbgColorPairs[i].bc);
    }

	for (BYTE i = 0; i < DEBUG_WINDOW_COUNT; i++) {
		dbg->windows[i] = NULL;
		dbg->windows[i] = (DebuggerWindow*) malloc(sizeof(DebuggerWindow));

		VM_ASSERT(dbg->windows[i] == NULL);

		dbg->windows[i]->initHandler = g_debuggerInits[i];
		dbg->windows[i]->initHandler(dbg->windows[i]);
		dbg->windows[i]->updateHandler = g_debuggerUpdaters[i];
	}

	doupdate();

	return VM_RESULT_SUCCESS;
}

/** destroyDebugger
 *
 * @param m_dbg
 *  A reference to a pointer to Debugger struct representing
 *  a debugger context that is to be destroyed
 * @description:
 *  Frees memory and destroys curses interface
 */
VM_RESULT destroyDebugger(Debugger **m_dbg) {
	VM_ASSERT(*m_dbg == NULL);
	Debugger *dbg = *m_dbg;

    destroyDisassembler();

	for (BYTE i = 0; i < DEBUG_WINDOW_COUNT; i++) {
		if (dbg->windows[i]->win != NULL) {
			delwin(dbg->windows[i]->win);
		}
	}

	endwin();
	
	for (BYTE i = 0; i < DEBUG_WINDOW_COUNT; i++) {
		if ((*m_dbg)->windows[i] != NULL) {
			free((*m_dbg)->windows[i]);
		}
	}

	free(*m_dbg);

	return VM_RESULT_SUCCESS;
}
