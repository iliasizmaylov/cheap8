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
void drawWindowBox(Debugger *dbg, DebuggerWindow *window) {
	if (dbg->current == window)
        wattron(window->win, COLOR_PAIR(WINDOW_BORDER_SELECTED_COLOR));
    box(window->win, 0, 0);
	if (dbg->current == window)
        wattroff(window->win, COLOR_PAIR(WINDOW_BORDER_SELECTED_COLOR));

    int tc = dbg->current == window ? 
            COLOR_PAIR(WINDOW_TITLE_SELECTED_COLOR) : 
            COLOR_PAIR(WINDOW_TITLE_COLOR);

    wattron(window->win, tc);
	mvwprintw(window->win, 0, WINDOW_TITLE_OFFSET, " %s ", window->title);
	wattroff(window->win, tc);
    wmove(window->win, WINDOW_CONTENT_Y_OFFSET, WINDOW_CONTENT_X_OFFSET);
}

void generateWindowPos(DebuggerWindow *dwin) {
    dwin->xPos = 0;
    dwin->yPos = 0;
    dwin->lines = 0;
    dwin->columns = 0;

    dwin->left = NULL;
    dwin->right = NULL;
    dwin->up = NULL;
    dwin->down = NULL;

    int xChunk = -1;
    int yChunk = -1;

    char cl;

    for (int i = 0; i < DEFAULT_LAYOUT_ROWS; i++) {
        for (int j = 0; j < DEFAULT_LAYOUT_COLS; j++) {
            cl = g_debuggerLayout[i * DEFAULT_LAYOUT_ROWS + j];
            if (cl == dwin->literal) {
                if (j > 0 && dwin->left == NULL) {
                    if (g_debuggerLayout[i * DEFAULT_LAYOUT_ROWS + (j - 1)] != cl) {
                        dwin->left = findByLiteral(g_debuggerLayout[i * DEFAULT_LAYOUT_ROWS + (j - 1)]);
                    }
                }

                if (j < (DEFAULT_LAYOUT_ROWS - 1) && dwin->right == NULL) {
                    if (g_debuggerLayout[i * DEFAULT_LAYOUT_ROWS + (j + 1)] != cl) {
                        dwin->right = findByLiteral(g_debuggerLayout[i * DEFAULT_LAYOUT_ROWS + (j + 1)]);
                    }
                }
                    
                if (i > 0 && dwin->up == NULL) {
                    if (g_debuggerLayout[(i - 1) * DEFAULT_LAYOUT_ROWS + j] != cl) {
                        dwin->up = findByLiteral(g_debuggerLayout[(i - 1) * DEFAULT_LAYOUT_ROWS + j]);
                    }
                }

                if (i < (DEFAULT_LAYOUT_ROWS - 1) && dwin->down == NULL) {
                    if (g_debuggerLayout[(i + 1) * DEFAULT_LAYOUT_ROWS + j] != cl) {
                        dwin->down = findByLiteral(g_debuggerLayout[(i + 1) * DEFAULT_LAYOUT_ROWS + j]);
                    }
                }

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

/* ======================= UPDATE HANDLERS ========================= */

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

    mvwprintw(window->win, WINDOW_CONTENT_Y_OFFSET, WINDOW_CONTENT_X_OFFSET, 
            "PC = 0x%03X (%u)   ", core->PC, core->PC);
    mvwprintw(window->win, WINDOW_CONTENT_Y_OFFSET + 1, WINDOW_CONTENT_X_OFFSET, 
            "I  = 0x%03X (%u)   ", core->I, core->I);
    mvwprintw(window->win, WINDOW_CONTENT_Y_OFFSET + 2, WINDOW_CONTENT_X_OFFSET, 
            "SP = 0x%03X (%u)   ", core->SP, core->SP);

	BYTE row = 4;
	for (BYTE i = 0; i < GENERAL_PURPOSE_REGISTERS; i++) {
		mvwprintw(window->win, WINDOW_CONTENT_Y_OFFSET + row, WINDOW_CONTENT_X_OFFSET, 
				"%s = 0x%03X (%u)   ", regnames[i], core->reg[i], core->reg[i]);
		row += 1;
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
};

void updatePreview(Debugger *dbg, DebuggerWindow *window, const C8core *core) {
    BYTE pxh = window->lines / 10;
    BYTE pxw = window->columns / 8;

    BYTE currentRow = 0; 

    for (BYTE i = 0; i < 10; i++) {
        currentRow = core->memory[core->I + i];
        for (BYTE j = 0; j < 8; j++) {
            if (GET_BIT_BE(currentRow, j)) {
                drawBlock(window->win, WINDOW_CONTENT_Y_OFFSET + i * pxh, WINDOW_CONTENT_X_OFFSET + j * pxw,
                            pxw, pxh);
            }
        }
    }
}

void updateStack(Debugger *dbg, DebuggerWindow *window, const C8core *core) {
    for (BYTE i = 0; i < STACK_SIZE; i++) {
        if (i < core->SP) {
            mvwprintw(window->win, WINDOW_CONTENT_Y_OFFSET + i, WINDOW_CONTENT_X_OFFSET + 6,
                        "[0x%04X]", core->stack[i]);
        } else {
            if (i == core->SP)
                mvwprintw(window->win, WINDOW_CONTENT_Y_OFFSET + i, WINDOW_CONTENT_X_OFFSET, "SP => ");

            mvwprintw(window->win, WINDOW_CONTENT_Y_OFFSET + i, WINDOW_CONTENT_X_OFFSET + 6, "[      ]");
        }
    }
}

/* ==================== DEBUGGER CONTEXT FUNCTIONS ================= */

static inline DebuggerWindow *getadj(DebuggerWindow *cur, BYTE side) {
    if (side >= WINDOW_SIDES || cur == NULL)
        return NULL;
     
    return cur->adj[side] == NULL ? cur : cur->adj[side];
}

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
            drawWindowBox(dbg, dbg->windows[i]);
            wrefresh(dbg->windows[i]->win);
        }
	}

    dbg->lastInput = getch();
    VM_RESULT ret = VM_RESULT_SUCCESS;

    switch(dbg->lastInput) {
        case KEY_UP:
            dbg->current = getadj(dbg->current, WINDOW_UP);
            break;
        case KEY_DOWN:
            dbg->current = getadj(dbg->current, WINDOW_DOWN);
            break;
        case KEY_LEFT:
            dbg->current = getadj(dbg->current, WINDOW_LEFT);
            break;
        case KEY_RIGHT:
            dbg->current = getadj(dbg->current, WINDOW_RIGHT);
            break;
    }

    if (dbg->flags & DEBUGGER_FLAG_STEP_MODE)
        ret = VM_RESULT_DBG;

    if (dbg->lastInput == ' ')
        ret = VM_RESULT_SUCCESS;

	doupdate();

	return ret;
}

void initWindow(Debugger *dbg, DebuggerWindow *window) {
    generateWindowPos(window);
    
    window->adj[WINDOW_LEFT] = window->left;
    window->adj[WINDOW_RIGHT] = window->right;
    window->adj[WINDOW_UP] = window->up;
    window->adj[WINDOW_DOWN] = window->down;

    if (window->lines == 0 && window->columns == 0) {
        window->win = NULL;
        return;
    }

    window->win = newwin(window->lines, window->columns, window->yPos, window->xPos);

    drawWindowBox(dbg, window);
}

static const DebuggerColorPair dbgColorPairs[WINDOW_COLOR_END_NR] = {
    /* Placeholder for 0th color pair that is reserved by curses lib*/ {
        .fc = 0, .bc = 0
    },
    /* WINDOW_TITLE_COLOR */ {
        .fc = COLOR_BLACK, .bc = COLOR_WHITE
    },
    /* WINDOW_BORDER_SELECTED_COLOR */ {
        .fc = COLOR_YELLOW, .bc = COLOR_BLACK
    },
    /* WINDOW_TITLE_SELECTED_COLOR */ {
        .fc = COLOR_BLACK, .bc = COLOR_YELLOW
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
	dbg->flags = DEBUGGER_FLAG_STEP_MODE;

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

    dbg->current = &dbgwnds[DEBUG_WINDOW_DISASM];

	for (BYTE i = 0; i < DEBUG_WINDOW_COUNT; i++) {
        dbg->windows[i] = &dbgwnds[i];
        initWindow(dbg, dbg->windows[i]);
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

	free(*m_dbg);

	return VM_RESULT_SUCCESS;
}
