#include "input.h"

WORD getKeyBitmask(SDL_Scancode scancode) {
	for (BYTE i = 0; i < KEYPAD_KEY_COUNT; i++) {
		if (KEYPAD_MAP[i] == scancode) {
			return 1 << i;
		}
	}

	return WRONG_INPUT;
}
