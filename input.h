#ifndef _INPUT_H_
#define _INPUT_H_

#include "types.h"

#define KEYPAD_KEY_COUNT		16
#define WRONG_INPUT				0xFFFF

#define INPUT_DEBUG_STEP_MODE	SDL_SCANCODE_M				// A key to enter debug step-by-step mode
#define INPUT_DEBUG_NEXT_STEP	SDL_SCANCODE_RIGHTBRACKET	// A key to step into in step-by-step mode
#define INPUT_QUIT				SDL_SCANCODE_ESCAPE			// Quit emulator

/*
 * Chip-8 keypad is mapped to these standard keyboard keys:
 * 1 2 3 4
 * Q W E R
 * A S D F
 * Z X C V
*/
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

WORD getKeyBitmask(BYTE scancode);

#endif
