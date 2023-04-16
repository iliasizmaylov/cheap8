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

    int bc = 0;
    int tc = 0;

    if (dbg->current == window) {
        if (dbg->flags & DEBUGGER_FLAG_EDITING) {
            bc = COLOR_PAIR(WINDOW_TITLE_EDITED_COLOR);
            tc = COLOR_PAIR(WINDOW_TITLE_EDITED_COLOR);
        } else {
            bc = COLOR_PAIR(WINDOW_TITLE_SELECTED_COLOR);
            tc = COLOR_PAIR(WINDOW_TITLE_SELECTED_COLOR);
        }
    } else {
        bc = COLOR_PAIR(WINDOW_TITLE_COLOR);
        tc = COLOR_PAIR(WINDOW_TITLE_COLOR);
    }

    wattron(window->win, bc);
    box(window->win, 0, 0);
    wattroff(window->win, bc);

    wattron(window->win, tc);
	mvwprintw(window->win, 0, WINDOW_TITLE_OFFSET, " %s ", window->title);
	wattroff(window->win, tc);
    wmove(window->win, WINDOW_CONTENT_Y_OFFSET, WINDOW_CONTENT_X_OFFSET);
}

void drawPopupBox(Debugger *dbg) {
    if (!dbg->popup)
        return;

    wattron(dbg->popup_win, COLOR_PAIR(POPUP_BORDER_COLOR));
    box(dbg->popup_win, 0, 0);
    wattroff(dbg->popup_win, COLOR_PAIR(POPUP_BORDER_COLOR));

    wattron(dbg->popup_win, COLOR_PAIR(POPUP_TITLE_COLOR));
	mvwprintw(dbg->popup_win, 0, WINDOW_TITLE_OFFSET, " %s ", dbg->popup->title);
	wattroff(dbg->popup_win, COLOR_PAIR(POPUP_TITLE_COLOR));
}

static inline DebuggerWindow *findByLiteral(char l) {
    for (int i = 0; i < DEBUG_WINDOW_COUNT; i++)
        if (dbgwnds[i].literal == l)
            return &dbgwnds[i];

    return NULL;
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
    const char *layout = g_debuggerLayoutNoDisplay;

    char cl;

    for (int i = 0; i < DEFAULT_LAYOUT_ROWS; i++) {
        for (int j = 0; j < DEFAULT_LAYOUT_COLS; j++) {
            cl = layout[i * DEFAULT_LAYOUT_ROWS + j];
            if (cl == dwin->literal) {
                if (j > 0 && dwin->left == NULL) {
                    if (layout[i * DEFAULT_LAYOUT_ROWS + (j - 1)] != cl) {
                        dwin->left = findByLiteral(layout[i * DEFAULT_LAYOUT_ROWS + (j - 1)]);
                    }
                }

                if (j < (DEFAULT_LAYOUT_ROWS - 1) && dwin->right == NULL) {
                    if (layout[i * DEFAULT_LAYOUT_ROWS + (j + 1)] != cl) {
                        dwin->right = findByLiteral(layout[i * DEFAULT_LAYOUT_ROWS + (j + 1)]);
                    }
                }
                    
                if (i > 0 && dwin->up == NULL) {
                    if (layout[(i - 1) * DEFAULT_LAYOUT_ROWS + j] != cl) {
                        dwin->up = findByLiteral(layout[(i - 1) * DEFAULT_LAYOUT_ROWS + j]);
                    }
                }

                if (i < (DEFAULT_LAYOUT_ROWS - 1) && dwin->down == NULL) {
                    if (layout[(i + 1) * DEFAULT_LAYOUT_ROWS + j] != cl) {
                        dwin->down = findByLiteral(layout[(i + 1) * DEFAULT_LAYOUT_ROWS + j]);
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

static inline void drawBlock(WINDOW *win, int y, int x, int w, int h) {
    for (int i = 0; i < h; i++) {
        wmove(win, y + i, x);
        for (int j = 0; j < w; j++) {
            wprintw(win, "%s", "▓");
        }
    }
}

static inline DebuggerWindow *getadj(DebuggerWindow *cur, BYTE side) {
    if (side >= WINDOW_SIDES || cur == NULL)
        return NULL;
     
    return cur->adj[side] == NULL ? cur : cur->adj[side];
}


static const char *rot_chars = "//-\\\\|";
static const char *cur_rot_char = NULL;

static void printRotatingThingy(WINDOW *win) {
    if (!cur_rot_char)
        cur_rot_char = rot_chars;
    wprintw(win, "%c", (char)*cur_rot_char);
    cur_rot_char++;
    if (!(char)*cur_rot_char)
        cur_rot_char = rot_chars;
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
    __INIT_AUX_DATA(MemoryWindowData) {
        AUX_DATA->addr_start    = core->PC;
        AUX_DATA->addr_end      = 0xFFFF;
        AUX_DATA->max_cols      = 0;
    }

    if (!(window->flags & WINDOW_FLAG_TOUCHED))
        AUX_DATA->addr_start = core->PC;

    AUX_DATA->max_cols = (window->columns - 2) / (WINDOW_MEMORY_COLUMN_OFFSET);
	BYTE memEnd = 0;

	for (WORD i = 0; i < window->textLines; i++) {
		for (WORD j = i * AUX_DATA->max_cols, k = 0; j < (i + 1) * AUX_DATA->max_cols; j++, k++) {
			QWORD nextMem = AUX_DATA->addr_start + j - 1;
			if (j == i * AUX_DATA->max_cols) {
				wattron(window->win, COLOR_PAIR(WINDOW_MEMORY_END_COLOR));
				mvwprintw(window->win, WINDOW_CONTENT_Y_OFFSET + i, 
                            WINDOW_CONTENT_X_OFFSET + (k * WINDOW_MEMORY_COLUMN_OFFSET), 
						    "%03X:", nextMem);
				wattroff(window->win, COLOR_PAIR(WINDOW_MEMORY_END_COLOR));
			} else {
				if (nextMem <= MEMORY_RANGE_PROGRAM_MAX) {
					if (nextMem == core->PC || nextMem == core->PC + 1) {
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
 * @p
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
    __INIT_AUX_DATA(DisasmWindowData) {
        AUX_DATA->addr_start = core->PC;
        AUX_DATA->addr_end = 0;
    }

    if (!(window->flags & WINDOW_FLAG_TOUCHED))
        AUX_DATA->addr_start = core->PC;

    const char *fmt_current = "=>0x%03X [0x%04X]  %-20s# %s";
    const char *fmt_not_current = "  0x%03X [0x%04X]  %-20s# %s";
    const char *fmt_selected = fmt_current;

    const Instruction *beingrun = getInstructionAt(core->PC);

    for (int i = 0; i < window->textLines; i++) {
        const Instruction *current = getInstructionAt(AUX_DATA->addr_start + i * 2);

        if (current->op != NULL) {
            if (current->addr == beingrun->addr) {
                fmt_selected = fmt_current;
                wattron(window->win, COLOR_PAIR(WINDOW_MEMORY_OPCODE_COLOR));
            } else {
                fmt_selected = fmt_not_current;
            }
            mvwprintw(window->win, WINDOW_CONTENT_Y_OFFSET + i, WINDOW_CONTENT_X_OFFSET, 
                            fmt_selected, current->addr, current->raw, current->asmstr, current->readable); 
        } else {
            wattron(window->win, COLOR_PAIR(WINDOW_MEMORY_END_COLOR));
            mvwprintw(window->win, WINDOW_CONTENT_Y_OFFSET + i, WINDOW_CONTENT_X_OFFSET, "N/A");
        }
        wattroff(window->win, COLOR_PAIR(WINDOW_MEMORY_OPCODE_COLOR));
        wattroff(window->win, COLOR_PAIR(WINDOW_MEMORY_END_COLOR));
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

void updateCurrentState(Debugger *dbg, DebuggerWindow *window, const C8core *core) {
    mvwprintw(window->win, WINDOW_CONTENT_Y_OFFSET, WINDOW_CONTENT_X_OFFSET, "Status: ");
    if (dbg->flags & DEBUGGER_FLAG_STEP_MODE) {
        wattron(window->win, COLOR_PAIR(WINDOW_STATUS_PAUSED_COLOR));
        wprintw(window->win, "PAUSED");
        wattroff(window->win, COLOR_PAIR(WINDOW_STATUS_PAUSED_COLOR));
    } else {
        wattron(window->win, COLOR_PAIR(WINDOW_STATUS_RUNNING_COLOR));
        wprintw(window->win, "Running... ");
        printRotatingThingy(window->win);
        wattroff(window->win, COLOR_PAIR(WINDOW_STATUS_RUNNING_COLOR));
    }

    mvwprintw(window->win, WINDOW_CONTENT_Y_OFFSET + 2, WINDOW_CONTENT_X_OFFSET,
                "Delay timer state: %-2u", core->tDelay);
    mvwprintw(window->win, WINDOW_CONTENT_Y_OFFSET + 3, WINDOW_CONTENT_X_OFFSET,
                "Sound timer state: %-2u", core->tSound);

    mvwprintw(window->win, WINDOW_CONTENT_Y_OFFSET + 5, WINDOW_CONTENT_X_OFFSET,
                "Keypad state: %s", g_keypadReadable);
    mvwprintw(window->win, WINDOW_CONTENT_Y_OFFSET + 6, WINDOW_CONTENT_X_OFFSET,
                "              ");

    for (BYTE i = 0; i < 16; i++)
        wprintw(window->win, "%u", GET_BIT(core->keypadState, i));
}

/* ====================== DEBUGGER MENU HANDLERS =================== */

/* Global actions handlers */

void gNavHandler(Debugger *dbg) {
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
}

void gHandler_back(Debugger *dbg) {
    dbg->flags &= ~DEBUGGER_FLAG_EDITING;
}

void gHandler_quit(Debugger *dbg) {
    dbg->flags |= DEBUGGER_FLAG_EXIT;
}

void gHandler_select(Debugger *dbg) {
    if (!(dbg->current->flags & (WINDOW_FLAG_READONLY | WINDOW_FLAG_PERMANENT)))
        dbg->flags |= DEBUGGER_FLAG_EDITING;
}

void gHandler_stepin(Debugger *dbg) {
    dbg->flags |= DEBUGGER_FLAG_NEXT_STEP;
}

void gHandler_pauseresume(Debugger *dbg) {
    dbg->flags ^= DEBUGGER_FLAG_STEP_MODE;
}

/* Popup-wise actions handlers */

void pHandler_cancel(Debugger *dbg) {
    dbg->flags &= ~DEBUGGER_FLAG_POPUP;
    gPopupExit_ALL(dbg, dbg->popup);
    dbg->popup = NULL;
}

void pHandler_save(Debugger *dbg) {
    dbg->flags &= ~DEBUGGER_FLAG_POPUP;
    dbg->popup->hsave(dbg, dbg->popup);
    gPopupExit_ALL(dbg, dbg->popup);
    dbg->popup = NULL;
}

/* Window-wise navigation handlers */

void wNavHandler_memory(Debugger *dbg) {
    MemoryWindowData *wdata = (MemoryWindowData*)dbg->current->auxdata;
    WORD ystep = wdata->max_cols;
    WORD xstep = 1;
    BYTE isTouched = 0;

    switch(dbg->lastInput) {
    case KEY_UP:
        wdata->addr_start -= ystep;
        isTouched = 1; break;
    case KEY_DOWN:
        wdata->addr_start += ystep;
        isTouched = 1; break;
    case KEY_LEFT:
        wdata->addr_start -= xstep;
        isTouched = 1; break;
    case KEY_RIGHT:
        wdata->addr_start += xstep;
        isTouched = 1; break;
    }

    if (isTouched)
        dbg->current->flags |= WINDOW_FLAG_TOUCHED;
}

void wNavHandler_disasm(Debugger *dbg) {
    DisasmWindowData *wdata = (DisasmWindowData*)dbg->current->auxdata;
    BYTE isTouched = 0;

    switch(dbg->lastInput) {
    case KEY_UP:
        wdata->addr_start -= OPCODE_SIZE;
        isTouched = 1; break;
    case KEY_DOWN:
        wdata->addr_start += OPCODE_SIZE;
        isTouched = 1; break;
    }

    if (isTouched)
        dbg->current->flags |= WINDOW_FLAG_TOUCHED;
}

/* Window-wise actions handlers */

void wHandler_mem_goto(Debugger *dbg) {
    dbg->flags |= DEBUGGER_FLAG_POPUP;     
    dbg->popup = &g_popups[DEBUGGER_POPUP_MEM_GOTO];
}

void wHandler_dis_goto(Debugger *dbg) {
    dbg->flags |= DEBUGGER_FLAG_POPUP;
    dbg->popup = &g_popups[DEBUGGER_POPUP_DIS_GOTO];
}

/* ================== DEBUGGER POPUP DRAW SAVE EXIT ================ */

static inline void destroyForm(DebuggerPopup *popup) {
    unpost_form(popup->form);
    free_form(popup->form);
    for (BYTE i = 0; i < popup->field_count; i++)
        free_field(popup->fields[i]);

    popup->form = NULL;
    popup->isDrawn = 0;
}

void gPopupExit_ALL(Debugger *dbg, DebuggerPopup *popup) {
    if (popup->form)
        destroyForm(popup);
}

void gPopupKeys_ALL(Debugger *dbg, DebuggerPopup *popup) {
    if (dbg->lastInput != ERR) {
        switch(dbg->lastInput) {
        case KEY_STAB:
        case KEY_DOWN:
            form_driver(popup->form, REQ_NEXT_FIELD);
            form_driver(popup->form, REQ_END_LINE);
            break;
        case KEY_UP:
            form_driver(popup->form, REQ_PREV_FIELD);
            form_driver(popup->form, REQ_END_LINE);
            break;
        case KEY_BACKSPACE:
            form_driver(popup->form, REQ_PREV_CHAR);
            form_driver(popup->form, REQ_DEL_CHAR);
            break;
        case KEY_DC:
            form_driver(popup->form, REQ_DEL_CHAR);
            break;
        case KEY_LEFT:
            form_driver(popup->form, REQ_PREV_CHAR);
            break;
        case KEY_RIGHT:
            form_driver(popup->form, REQ_NEXT_CHAR);
            break;
        default:
            form_driver(popup->form, dbg->lastInput);
            break;
        }
    }
}

void wPopupDraw_findmem(Debugger *dbg, DebuggerPopup *popup) {
    for (int i = 0; i < DEBUGGER_POPUP_MAX_FIELDS; i++)
        popup->fields[i] = NULL;

    popup->fields[0] = new_field(1, POPUP_SUB_X_LENGTH, POPUP_SUB_Y_OFFSET, POPUP_SUB_X_OFFSET, 0, 0);    
    set_field_fore(popup->fields[0], COLOR_PAIR(MENU_BAR_COLOR));
    set_field_back(popup->fields[0], COLOR_PAIR(MENU_BAR_COLOR));
    field_opts_off(popup->fields[0], O_AUTOSKIP);

    popup->fields[popup->field_count] = NULL;

    set_form_win(popup->form, dbg->popup_win);
    set_form_sub(popup->form, dbg->popup_sub);
    popup->form = new_form(popup->fields);
    set_current_field(popup->form, popup->fields[0]);
    post_form(popup->form);

    mvwprintw(dbg->popup_win, 2, 2, "Memory Address: ");
}

void wPopupSave_findmem(Debugger *dbg, DebuggerPopup *popup) {
    MemoryWindowData *data;
    if (popup->form) {
        form_driver(popup->form, REQ_VALIDATION);
        data = dbg->windows[DEBUG_WINDOW_MEMORY]->auxdata;
        data->addr_start = strtoll(field_buffer(popup->fields[0], 0), NULL, 16);
        dbg->windows[DEBUG_WINDOW_MEMORY]->flags |= WINDOW_FLAG_TOUCHED;
        destroyForm(popup);
    }
}

void wPopupDraw_findop(Debugger *dbg, DebuggerPopup *popup) {
    for (int i = 0; i < DEBUGGER_POPUP_MAX_FIELDS; i++)
        popup->fields[i] = NULL;

    popup->fields[0] = new_field(1, POPUP_SUB_X_LENGTH, POPUP_SUB_Y_OFFSET, POPUP_SUB_X_OFFSET, 0, 0);    
    set_field_fore(popup->fields[0], COLOR_PAIR(MENU_BAR_COLOR));
    set_field_back(popup->fields[0], COLOR_PAIR(MENU_BAR_COLOR));
    field_opts_off(popup->fields[0], O_AUTOSKIP);

    popup->fields[popup->field_count] = NULL;

    set_form_win(popup->form, dbg->popup_win);
    set_form_sub(popup->form, dbg->popup_sub);
    popup->form = new_form(popup->fields);
    set_current_field(popup->form, popup->fields[0]);
    post_form(popup->form);

    mvwprintw(dbg->popup_win, 2, 2, "Memory Address: ");
}

void wPopupSave_findop(Debugger *dbg, DebuggerPopup *popup) {
    DisasmWindowData *data;
    if (popup->form) {
        form_driver(popup->form, REQ_VALIDATION);
        data = dbg->windows[DEBUG_WINDOW_DISASM]->auxdata;
        data->addr_start = strtoll(field_buffer(popup->fields[0], 0), NULL, 16);
        dbg->windows[DEBUG_WINDOW_DISASM]->flags |= WINDOW_FLAG_TOUCHED;
        destroyForm(popup);
    }
}

/* ==================== DEBUGGER CONTEXT FUNCTIONS ================= */

void initMenu(Debugger *dbg) {
    dbg->menuwin = newwin(DEBUGGER_MENU_COLS, DEBUGGER_MENU_ROWS,
                    DEBUGGER_MENU_Y_POS, DEBUGGER_MENU_X_POS);
    wbkgd(dbg->menuwin, COLOR_PAIR(MENU_BAR_COLOR));
    wrefresh(dbg->menuwin);
}

static inline void printOpts(WINDOW *win, const DebuggerMenuOption *opts, BYTE c) {
    for (BYTE i = 0; i < c; i++)
        wprintw(win, " [%s] %s ", opts[i].keystr, opts[i].name);
}

void updateMenu(Debugger *dbg, DebuggerWindow *dwin) {
    werase(dbg->menuwin);
    wbkgd(dbg->menuwin, COLOR_PAIR(MENU_BAR_COLOR));

    mvwprintw(dbg->menuwin, 0, 1, "| %-15s | ", dwin->menu->name);

    if (dbg->flags & DEBUGGER_FLAG_POPUP) {
        printOpts(dbg->menuwin, g_popup_opts, POPUP_OPTION_COUNT);
    } else {
        if (dwin->menu->global_opts != NULL) {
            printOpts(dbg->menuwin, dwin->menu->global_opts, GLOBAL_OPTION_COUNT);
        }

        if (dwin->menu->opts != NULL && dbg->flags & DEBUGGER_FLAG_EDITING) {
            printOpts(dbg->menuwin, dwin->menu->opts, dwin->menu->optcount);
        }
    }

    wrefresh(dbg->menuwin);
}

static void callHandler(Debugger *dbg) {
    if (dbg->popup) {
        for (BYTE i = 0; i < POPUP_OPTION_COUNT; i++) {
            if (dbg->lastInput == g_popup_opts[i].key) {
                g_popup_opts[i].handler(dbg);
                return;
            }
        }
        gPopupKeys_ALL(dbg, dbg->popup);
    } else {
        for (BYTE i = 0; i < GLOBAL_OPTION_COUNT; i++) {
            if (dbg->lastInput == dbg->current->menu->global_opts[i].key) {
                dbg->current->menu->global_opts[i].handler(dbg);
                return;
            }
        }

        if (dbg->flags & DEBUGGER_FLAG_EDITING) {
            for (BYTE i = 0; i < dbg->current->menu->optcount; i++) {
                if (dbg->lastInput == dbg->current->menu->opts[i].key) {
                    dbg->current->menu->opts[i].handler(dbg);
                    return;
                }
            }
        }
    }
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
    if (!(dbg->flags & DEBUGGER_FLAG_POPUP)) {
        for (BYTE i = 0; i < DEBUG_WINDOW_COUNT; i++) {
            if (dbg->windows[i]->win != NULL) {
                werase(dbg->windows[i]->win);
                dbg->windows[i]->updateHandler(dbg, dbg->windows[i], dbg->core);
                drawWindowBox(dbg, dbg->windows[i]);
                wrefresh(dbg->windows[i]->win);
            }
        }
    }

    updateMenu(dbg, dbg->current);

    dbg->lastInput = getch();
    VM_RESULT ret = VM_RESULT_SUCCESS;
   
    if (dbg->lastInput != ERR) {
        if (!(dbg->flags & DEBUGGER_FLAG_EDITING))
            gNavHandler(dbg);
        else if (dbg->current->menu->navhandler != NULL) 
            dbg->current->menu->navhandler(dbg);

        callHandler(dbg);
    }

    if (dbg->flags & DEBUGGER_FLAG_EXIT)
        return VM_RESULT_EVENT_QUIT;

    if (dbg->flags & DEBUGGER_FLAG_STEP_MODE)
        ret = VM_RESULT_DBG;

    if (ret == VM_RESULT_DBG && dbg->flags & DEBUGGER_FLAG_NEXT_STEP) {
        ret = VM_RESULT_SUCCESS;
        dbg->flags ^= DEBUGGER_FLAG_NEXT_STEP;
    }

    if (dbg->flags & DEBUGGER_FLAG_POPUP) {
        curs_set(1);
        if (!dbg->popup->isDrawn) {
            top_panel(dbg->popup_pan);
            show_panel(dbg->popup_pan);
            top_panel(dbg->popup_sub_pan);
            show_panel(dbg->popup_sub_pan);
            werase(dbg->popup_sub);
            werase(dbg->popup_win);
            dbg->popup->hdraw(dbg, dbg->popup);
            dbg->popup->isDrawn = 1;
            drawPopupBox(dbg);
        }
        wrefresh(dbg->popup_win);
        wrefresh(dbg->popup_sub);
    } else {
        curs_set(0);
        werase(dbg->popup_sub);
        werase(dbg->popup_win);
        hide_panel(dbg->popup_sub_pan);
        hide_panel(dbg->popup_pan);
    }

    update_panels();
	doupdate();

	return ret;
}

void initWindow(Debugger *dbg, DebuggerWindow *window) {
    generateWindowPos(window);
    
    window->auxdata = NULL;

    window->adj[WINDOW_LEFT] = window->left;
    window->adj[WINDOW_RIGHT] = window->right;
    window->adj[WINDOW_UP] = window->up;
    window->adj[WINDOW_DOWN] = window->down;

    if (window->lines == 0 && window->columns == 0) {
        window->win = NULL;
        return;
    }

    window->win = newwin(window->lines, window->columns, window->yPos, window->xPos);
    window->pan = new_panel(window->win);
    bottom_panel(window->pan);

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
    /* WINDOW_BORDER_EDITED_COLOR */ {
        .fc = COLOR_BLUE, .bc = COLOR_BLACK
    },
    /* WINDOW_TITLE_SELECTED_COLOR */ {
        .fc = COLOR_BLACK, .bc = COLOR_YELLOW
    },
    /* WINDOW_TITLE_EDITED_COLOR */ {
        .fc = COLOR_WHITE, .bc = COLOR_BLUE
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
    },
    /* WINDOW_STATUS_PAUSED_COLOR */ {
        .fc = COLOR_YELLOW, .bc = COLOR_BLACK
    },
    /* WINDOW_STATUS_RUNNING_COLOR */ {
        .fc = COLOR_GREEN, .bc = COLOR_BLACK
    },
    /* MENU_BAR_COLOR */ {
        .fc = COLOR_WHITE, .bc = COLOR_BLUE
    },
    /* POPUP_BORDER_COLOR */ {
        .fc = COLOR_BLACK, .bc = COLOR_BLUE
    },
    /* POPUP_TITLE_COLOR */ {
        .fc = COLOR_BLACK, .bc = COLOR_WHITE
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

    for (BYTE i = WINDOW_COLOR_START_NR; i < WINDOW_COLOR_END_NR; i++)
        init_pair(i, dbgColorPairs[i].fc, dbgColorPairs[i].bc);

    dbg->current = &dbgwnds[DEBUG_WINDOW_DISASM];

    initMenu(dbg);
	for (BYTE i = 0; i < DEBUG_WINDOW_COUNT; i++) {
        dbg->windows[i] = &dbgwnds[i];
        initWindow(dbg, dbg->windows[i]);
		dbg->windows[i]->updateHandler = g_debuggerUpdaters[i];
        dbg->windows[i]->menu = &g_menus[i];
	}

    dbg->popup_win = newwin(DEBUGGER_POPUP_LINES, DEBUGGER_POPUP_COLS, 
                DEBUGGER_POPUP_Y_POS, DEBUGGER_POPUP_X_POS);
    dbg->popup_sub = derwin(dbg->popup_win, DEBUGGER_POPUP_SUB_L, DEBUGGER_POPUP_SUB_C,
                DEBUGGER_POPUP_SUB_Y, DEBUGGER_POPUP_SUB_X);
    dbg->popup_pan = new_panel(dbg->popup_win);
    dbg->popup_sub_pan = new_panel(dbg->popup_sub);
    top_panel(dbg->popup_sub_pan);
    top_panel(dbg->popup_pan);
    dbg->popup = NULL;

    for (BYTE i = 0; i < NR_DEBUGGER_POPUPS; i++)
        g_popups[i].isDrawn = 0;

    update_panels();
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

    del_panel(dbg->popup_pan);
    del_panel(dbg->popup_sub_pan);
    delwin(dbg->popup_win);
    delwin(dbg->popup_sub);
    delwin(dbg->menuwin);
	for (BYTE i = 0; i < DEBUG_WINDOW_COUNT; i++) {
		if (dbg->windows[i]->win != NULL) {
            del_panel(dbg->windows[i]->pan);
			delwin(dbg->windows[i]->win);
            if (dbg->windows[i]->auxdata != NULL)
                free(dbg->windows[i]->auxdata);
		}
	}

	endwin();

	free(*m_dbg);

	return VM_RESULT_SUCCESS;
}
