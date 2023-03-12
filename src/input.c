/**
 * Cheap-8: a chip-8 emulator
 * 
 * File: input.c
 * License: DWYW - "Do Whatever You Want"
 * 
 * Various consts and function declaration for input handling
 */

#include "input.h"

const BYTE KEYPAD_MAP[KEYPAD_KEY_COUNT] = {
	SDL_SCANCODE_1,
	SDL_SCANCODE_2,
	SDL_SCANCODE_3,
	SDL_SCANCODE_Q,
	SDL_SCANCODE_W,
	SDL_SCANCODE_E,
	SDL_SCANCODE_A,
	SDL_SCANCODE_S,
	SDL_SCANCODE_D,
	SDL_SCANCODE_Z,
	SDL_SCANCODE_X,
	SDL_SCANCODE_C,
	SDL_SCANCODE_4,
	SDL_SCANCODE_R,
	SDL_SCANCODE_F,
	SDL_SCANCODE_V
};

// Convert raw input handled by SDL2 library to a bit that is set corresponding
// to a key on a chip-8 keypad which is:
// 1 2 3 4
// Q W E R
// A S D F
// Z X C V
//
// TODO: Add support for other media libraries (probably only curses)
WORD getKeyBitmask(SDL_Scancode scancode) {
	for (BYTE i = 0; i < KEYPAD_KEY_COUNT; i++) {
		if (KEYPAD_MAP[i] == scancode) {
			return 1 << i;
		}
	}

	return WRONG_INPUT;
}
