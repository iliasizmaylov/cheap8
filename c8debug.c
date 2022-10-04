#include "c8debug.h"

void drawWindowBox(DebuggerWindow *window) {
	box(window->win, 0, 0);
	wattron(window->win, COLOR_PAIR(WINDOW_TITLE_COLOR));
	mvwprintw(window->win, 0, WINDOW_TITLE_OFFSET, window->title);
	wattroff(window->win, COLOR_PAIR(WINDOW_TITLE_COLOR));
}

void initCurrentOpcode(DebuggerWindow *window) {
	window->lines = 4;
	window->columns = COLS;
	window->yPos = 0;
	window->xPos = 0;

	window->win = newwin(window->lines, window->columns, window->yPos, window->xPos);
	window->pan = new_panel(window->win);

	window->title = " Current Opcode ";
	drawWindowBox(window);
}

void initRegisters(DebuggerWindow *window) {
	window->lines = LINES / 5.5;
	window->columns = COLS / 2 - 1;
	window->yPos = LINES / 1.3 + 4;
	window->xPos = 0;

	window->win = newwin(window->lines, window->columns, window->yPos, window->xPos);
	window->pan = new_panel(window->win);

	window->title = " Registers ";
	drawWindowBox(window);
}

void initMemory(DebuggerWindow *window) {
	window->lines = LINES / 1.3;
	window->columns = COLS;
	window->yPos = 4;
	window->xPos = 0;

	window->win = newwin(window->lines, window->columns, window->yPos, window->xPos);
	window->pan = new_panel(window->win);

	window->title = " Memory ";
	drawWindowBox(window);
}

void initCustomFlags(DebuggerWindow *window) {
	window->lines = LINES / 5.5;
	window->columns = COLS / 2;
	window->yPos = LINES / 1.3 + 4;
	window->xPos = COLS / 2;

	window->win = newwin(window->lines, window->columns, window->yPos, window->xPos);
	window->pan = new_panel(window->win);

	window->title = " Custom Flags ";
	drawWindowBox(window);
}

void updateCurrentOpcode(DebuggerWindow *window, const C8core* core) {
	window->textLines = 1;
	mvwprintw(window->win, WINDOW_CONTENT_Y_OFFSET, WINDOW_CONTENT_X_OFFSET, "OPCODE: ");
	wattron(window->win, COLOR_PAIR(WINDOW_MEMORY_OPCODE_COLOR));
	wprintw(window->win, "%04X", core->opcode);
	wattroff(window->win, COLOR_PAIR(WINDOW_MEMORY_OPCODE_COLOR));
	wclrtoeol(window->win);
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

    wprintw(window->win, " Description: %s", OPCODES[getOpcodeIndex(core->opcode)].description);
}

void updateRegisters(DebuggerWindow *window, const C8core* core) {
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

void updateMemory(DebuggerWindow *window, const C8core* core) {
	window->textLines = window->lines - 3;
	WORD maxColumns = (window->columns - 2) / (WINDOW_MEMORY_COLUMN_OFFSET);
	BYTE memEnd = 0;
	for (WORD i = 0; i < window->textLines; i++) {
		for (WORD j = i * maxColumns, k = 0; j < (i + 1) * maxColumns; j++, k++) {
			QWORD nextMem = core->PC + j - 1;
			if (j == i * maxColumns) {
				wattron(window->win, COLOR_PAIR(WINDOW_MEMORY_END_COLOR));
				mvwprintw(window->win, WINDOW_CONTENT_Y_OFFSET + i, WINDOW_CONTENT_X_OFFSET + (k * WINDOW_MEMORY_COLUMN_OFFSET), 
						"%03X:", nextMem);
				wattroff(window->win, COLOR_PAIR(WINDOW_MEMORY_END_COLOR));
			} else {
				if (nextMem <= MEMORY_RANGE_PROGRAM_MAX) {
					if (j >= (i * maxColumns) + 1 && j <= (i * maxColumns) + 2 && i == 0) {
						wattron(window->win, COLOR_PAIR(WINDOW_MEMORY_OPCODE_COLOR));
					}
					mvwprintw(window->win, WINDOW_CONTENT_Y_OFFSET + i, WINDOW_CONTENT_X_OFFSET + (k * WINDOW_MEMORY_COLUMN_OFFSET), 
							"%02X ", core->memory[nextMem]);
					wattroff(window->win, COLOR_PAIR(WINDOW_MEMORY_OPCODE_COLOR));
					
				} else {
					if (!memEnd) {
						wattron(window->win, COLOR_PAIR(WINDOW_MEMORY_END_COLOR));
						mvwprintw(window->win, WINDOW_CONTENT_Y_OFFSET + i, WINDOW_CONTENT_X_OFFSET + (k * WINDOW_MEMORY_COLUMN_OFFSET), 
								WINDOW_MEMORY_END_CHAR);
						wattroff(window->win, COLOR_PAIR(WINDOW_MEMORY_END_COLOR));
					} else {
						mvwprintw(window->win, WINDOW_CONTENT_Y_OFFSET + i, WINDOW_CONTENT_X_OFFSET + (k * WINDOW_MEMORY_COLUMN_OFFSET), 
								WINDOW_MEMORY_NO_MEM_CHAR);
					}
					memEnd = 1;
				}
			}
		}
	}
}

void updateCustomFlags(DebuggerWindow *window, const C8core *core) {
	window->textLines = window->lines - 3;

	static const char *flagnames[8] = {
		"CUSTOM_FLAG_REDRAW_PENDING",
		"CUSTOM_FLAG_BAD_INPUT",
		"CUSTOM_FLAG_BAD_STACK",
		"CUSTOM_FLAG_BAD_OPCODE",
		"CUSTOM_FLAG_BAD_MEMORY",
		"CUSTOM_FLAG_BAD_SP",
		"CUSTOM_FLAG_MCR_OPCODE",
		"CUSTOM_FLAG_CRITICAL_ERROR"
	};

	for (BYTE i = 0; i < 8; i++) {
		if ((core->customFlags >> i) & 1) {
			wattron(window->win, A_BOLD);
			if (i == 0) {
				wattron(window->win, COLOR_PAIR(WINDOW_FLAGS_NORMAL_COLOR));
			} else {
				wattron(window->win, COLOR_PAIR(WINDOW_FLAGS_ERROR_COLOR));
			}
			mvwprintw(window->win, WINDOW_CONTENT_Y_OFFSET + i, WINDOW_CONTENT_X_OFFSET, "%s", flagnames[i]);
			mvwprintw(window->win, WINDOW_CONTENT_Y_OFFSET + i, WINDOW_CONTENT_X_OFFSET + 32, "( * )", flagnames[i]);
		} else {
			mvwprintw(window->win, WINDOW_CONTENT_Y_OFFSET + i, WINDOW_CONTENT_X_OFFSET, "%s", flagnames[i]);
			mvwprintw(window->win, WINDOW_CONTENT_Y_OFFSET + i, WINDOW_CONTENT_X_OFFSET + 32, "( )  ", flagnames[i]);
		}
		wattroff(window->win, A_BOLD);
		wattroff(window->win, COLOR_PAIR(WINDOW_FLAGS_NORMAL_COLOR));
		wattroff(window->win, COLOR_PAIR(WINDOW_FLAGS_ERROR_COLOR));
	}
}

VM_RESULT updateDebugger(Debugger *dbg) {
	for (BYTE i = 0; i < DEBUG_WINDOW_COUNT; i++) {
		dbg->windows[i]->updateHandler(dbg->windows[i], dbg->core);
		drawWindowBox(dbg->windows[i]);
		wrefresh(dbg->windows[i]->win);
	}

	update_panels();
	doupdate();

	return VM_RESULT_SUCCESS;
}

VM_RESULT initDebugger(Debugger **m_dbg, const C8core *_core) {
	VM_ASSERT(_core == NULL);

	*m_dbg = NULL;
	*m_dbg = (Debugger*) malloc(sizeof(Debugger));

	VM_ASSERT(*m_dbg == NULL);

	Debugger *dbg = *m_dbg;
	dbg->core = _core;
	dbg->isStepMode = 0;

	initscr();
	start_color();
	cbreak();
	noecho();
	curs_set(0);

	init_pair(WINDOW_TITLE_COLOR, COLOR_BLACK, COLOR_WHITE);
	init_pair(WINDOW_MEMORY_END_COLOR, COLOR_BLACK, COLOR_WHITE);
	init_pair(WINDOW_MEMORY_OPCODE_COLOR, COLOR_GREEN, COLOR_BLACK);
	init_pair(WINDOW_FLAGS_ERROR_COLOR, COLOR_RED, COLOR_BLACK);
	init_pair(WINDOW_FLAGS_NORMAL_COLOR, COLOR_GREEN, COLOR_BLACK);

	for (BYTE i = 0; i < DEBUG_WINDOW_COUNT; i++) {
		dbg->windows[i] = NULL;
		dbg->windows[i] = (DebuggerWindow*) malloc(sizeof(DebuggerWindow));

		VM_ASSERT(dbg->windows[i] == NULL);

		dbg->windows[i]->initHandler = g_debuggerInits[i];
		dbg->windows[i]->initHandler(dbg->windows[i]);
		dbg->windows[i]->updateHandler = g_debuggerUpdaters[i];
	}

	update_panels();
	doupdate();

	return VM_RESULT_SUCCESS;
}

VM_RESULT destroyDebugger(Debugger **m_dbg) {
	VM_ASSERT(*m_dbg == NULL);
	Debugger *dbg = *m_dbg;

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
