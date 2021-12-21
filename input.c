#include "input.h"

WORD getKeyBitmask(BYTE scancode) {
	for (BYTE i = 0; i < KEYPAD_KEY_COUNT; i++) {
		if (KEYPAD_MAP[i] == scancode) {
			return 1 << i;
		}
	}

	return WRONG_INPUT;
}

#define POLL_INPUT_QUIT 		0
#define POLL_INPUT_KEEP_GOING	1

// TODO: Move this to vm.c
BYTE pollInput(WORD *keypadState) {
	SDL_Event ev;
	while (SDL_PollEvent(&ev)) {
		if (ev.type == SDL_QUIT) {
			return POLL_INPUT_QUIT;
		} 

		if (ev.type == SDL_KEYDOWN || ev.type == SDL_KEYUP) {
			if (ev.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
				return POLL_INPUT_QUIT;
			}

			WORD key = getKeyBitmask(ev.key.keysym.scancode);

			if (key != WRONG_INPUT) {
				if (ev.type == SDL_KEYDOWN) {
					*keypadState |= key;
				} else if (ev.type == SDL_KEYUP) {
					*keypadState &= ~key;
				}
			}
		}
	}

	return POLL_INPUT_KEEP_GOING;
}
