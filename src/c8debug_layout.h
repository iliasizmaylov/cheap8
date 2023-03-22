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
#define DEFAULT_LAYOUT_COLS 10

#define SCREEN_CHUNK_COLS   (S_COLS / DEFAULT_LAYOUT_COLS)
#define SCREEN_CHUNK_LINES  (S_LINES / DEFAULT_LAYOUT_ROWS)

#define SIZE_CHUNKS_COLS(from, to) \
    (to * SCREEN_CHUNK_COLS - from * SCREEN_CHUNK_COLS)

#define SIZE_CHUNKS_LINES(from, to) \
    (to * SCREEN_CHUNK_LINES - from * SCREEN_CHUNK_LINES)

const char *g_debuggerLayout =
    ".........."
    "vvvvvvoooo"
    "vvvvvvdddr"
    "vvvvvvdddr"
    "vvvvvvdddr"
    "vvvvvvdddf"
    "vvvvvvdddf"
    "vvvvvvmmmm"
    "vvvvvvmmmm"
    "..........";

#endif  /* _C8DEBUG_LAYOUT_H_ */
