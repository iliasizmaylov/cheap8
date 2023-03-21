/**
 * Cheap-8: a chip-8 emulator
 * 
 * File: c8debug.h
 * License: DWYW - "Do Whatever You Want"
 *
 * Window layout description for c8 debugger
 * Header file only, has no corresponding .c file
 */

#ifndef _C8DEBUG_LAYOUT_H_
#define _C8DEBUG_LAYOUT_H_

#include "c8debug.h"

#define DEFAULT_LAYOUT_ROWS 10

#define SCREEN_CHUNK_COLS   (S_COLS / 8)
#define SCREEN_CHUNK_LINES  (S_LINES / DEFAULT_LAYOUT_ROWS)

#define SIZE_CHUNKS_COLS(from, to) \
    (to * SCREEN_CHUNK_COLS - from * SCREEN_CHUNK_COLS)

#define SIZE_CHUNKS_LINES(from, to) \
    (to * SCREEN_CHUNK_LINES - from * SCREEN_CHUNK_LINES)

static const BYTE windowLayouts[DEBUG_WINDOW_COUNT][DEFAULT_LAYOUT_ROWS] = {
    /* DEBUG_WINDOW_CURRENT_OPCODE */ {
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b11110000,
        0b00000000
    },
    /* DEBUG_WINDOW_REGISTERS */ {
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00001100,
        0b00001100
    },
    /* DEBUG_WINDOW_CUSTOM_FLAGS */ {
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000011,
        0b00000011
    },
    /* DEBUG_WINDOW_MEMORY */ {
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b11110000,
        0b11110000,
        0b11110000,
        0b11110000,
        0b00000000,
        0b00000000
    },
    /* DEBUG_WINDOW_DISASM */ {
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00001111,
        0b00001111,
        0b00001111,
        0b00001111,
        0b00000000,
        0b00000000
    }
};

#endif  /* _C8DEBUG_LAYOUT_H_ */
