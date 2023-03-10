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
extern const BYTE KEYPAD_MAP[KEYPAD_KEY_COUNT];

WORD getKeyBitmask(SDL_Scancode scancode);

#endif
