/**
 * Cheap-8: a chip-8 emulator
 * 
 * File: vm.c
 * License: DWYW - "Do Whatever You Want"
 * 
 * Definition of function that handle most of the logic in the VM
 */

#include "vm.h"

// ============================= Video Interface Functions =============================

/** initVideoInterface
 *
 * @param m_interface
 *  A reference to a pointer to struct VideoInterface to be initialized
 * @description
 *  Initializes the VideoInterface structure and prepares it for use with 
 *  selected video mode
 *  TODO:   Right now the only supported interface is SDL2 
 *          In the future the should be a (probably) curses interface
 *          CLI video output option
 */
VM_RESULT initVideoInterface(VideoInterface **m_interface) {
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		return VM_RESULT_ERROR;
	}

	*m_interface = (VideoInterface*) malloc(sizeof(VideoInterface));
	VideoInterface *interface = *m_interface;

	VM_ASSERT(interface == NULL);

	interface->resolutionWidth = VIDEO_DEFAULT_RESOLUTION_WIDTH;
	interface->resolutionHeight = VIDEO_DEFAULT_RESOLUTION_HEIGHT;
	interface->isFullScreen = 0;
	interface->windowFlags = VIDEO_DEFAULT_FLAGS;
	interface->pixelWidth = interface->resolutionWidth / SCREEN_RESOLUTION_WIDTH;
	interface->pixelHeight = interface->resolutionHeight / SCREEN_RESOLUTION_HEIGHT;
	interface->windowTitle = VIDEO_DEFAULT_WINDOW_TITLE;

	interface->window = NULL;
	interface->renderer = NULL;

	interface->window = SDL_CreateWindow(
			interface->windowTitle,
			SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_CENTERED,
			interface->resolutionWidth,
			interface->resolutionHeight,
			VIDEO_DEFAULT_FLAGS);

	VM_ASSERT(interface->window == NULL);

	interface->renderer = SDL_CreateRenderer(
			interface->window,
			VIDEO_DEFAULT_RENDER_INDEX,
			VIDEO_DEFAULT_RENDER_FLAGS);

    SDL_SetRenderDrawBlendMode(interface->renderer, SDL_BLENDMODE_BLEND);
    clearScreen(interface);

	VM_ASSERT(interface->renderer == NULL);

	return VM_RESULT_SUCCESS;
}

// Clears the screen of a given VideoInterface
// TODO: add support for curses VideoInterface later
VM_RESULT clearScreen(VideoInterface *interface) {
	VM_ASSERT(interface == NULL);
    
    SDL_SetRenderDrawColor(interface->renderer, VIDEO_BLACK_PIXEL_RGBO);
    SDL_RenderClear(interface->renderer);
    SDL_RenderPresent(interface->renderer);

	return VM_RESULT_SUCCESS;
}

/** redrawPixel
 * 
 * @param interface
 *  A pointer to VideoInterface struct to be used for output
 * @param screen
 *  Array of 64-bit long values representing screen row (size of 32 or 64 if hires)
 * @param row
 *  Row of a pixel to redraw
 * @param col
 *  Col of a pixel to redraw
 * @description:
 *  Redraws a given pixel on the screen
 */
VM_RESULT redrawPixel(VideoInterface *interface, QWORD *screen, WORD row, WORD col) {
    SDL_Rect nextPixel;
    BYTE pxl = GET_BIT_BE(screen[row], col);

    row *= interface->pixelHeight;
    col *= interface->pixelWidth;

    GET_NEXT_PIXEL(nextPixel,
                col, row,
                interface->pixelWidth,
                interface->pixelHeight);

    if (pxl)
        SDL_SetRenderDrawColor(interface->renderer, VIDEO_WHITE_PIXEL_RGBO);
    else
        SDL_SetRenderDrawColor(interface->renderer, VIDEO_BLACK_PIXEL_RGBO);

    SDL_RenderFillRect(interface->renderer, &nextPixel);

    return VM_RESULT_SUCCESS;
}

/** redrawScreen
 *
 * @param interface
 *  A pointer to a VideoInterface struct to be used
 * @param screen
 *  A 32x64 array of bits representing white and black pixels (corresponding to
 *  set and unset bits in said array)
 * @description:
 *  Redraws the whole screen using a given screen content
 */
VM_RESULT redrawScreen(VideoInterface *interface, QWORD *screen) {
	VM_ASSERT(interface == NULL);

	for (BYTE i = 0; i < SCREEN_RESOLUTION_HEIGHT; i++)
		for (BYTE j = 0; j < SCREEN_RESOLUTION_WIDTH; j++)
            redrawPixel(interface, screen, i, j);

	SDL_RenderPresent(interface->renderer);

	return VM_RESULT_SUCCESS;
}

/** destroyVideoInterface
 *
 * @param m_interface
 *  A reference to a pointer to struct VideoInterface to be uninitialized
 * @description:
 *  Destroys a connection to a video framework being used and frees memory 
 *  allocated to handle it (i.e. struct VideoInterface)
 *  TODO:   Add support for curses CLI video output
 */
VM_RESULT destroyVideoInterface(VideoInterface **m_interface) {
	VideoInterface *interface = *m_interface;

	if (interface != NULL) {
        SDL_DestroyWindow(interface->window);
		SDL_DestroyRenderer(interface->renderer);
		free(interface);
	}

	return VM_RESULT_SUCCESS;
}

// ============================= Audio Interface Functions =============================

/** gitSquareWave
 *
 * @param tone
 *  A float number representing a frequency of a sound
 * @param volume
 *  An integer representing sound's amplitude (or volume)
 * @param permutations
 *  Basically a sample rate of a sound
 * @param time
 *  A double float number representing a point at time at which current sample is 
 *  to be created
 * @description:
 *  Used in processAudioCallback function to build a sample of a square wave at
 *  a given point in time with given tone and sound
 */
Sint16 getSquareWave(float tone, Uint16 volume, Uint16 permutations, double time) {
	Sint16 sample = 0;
	for (int i = 0; i < permutations; i++) {
		sample += volume * sin((2 * i + 1) * tone * time) / (2 * i + 1);
	}
	return sample;
}

/** processAudioCallback
 *
 * @param userData
 *  Pointer to data that is to be used to create the next sample of sound
 *  For SDL2 implementation the struct that is used is AudioCallbackData
 * @param bytestream
 *  Array of bytes that will hold a result of a callback which is a new audio sample
 * @param bytes
 *  The length of a sample in bytes
 * @description:
 *  A function that is called all the time when the audio state is on and
 *  generates a next piece (or sample) of audio to be played
 */
void processAudioCallback(void *userData, Uint8 *bytestream, int bytes) {
	Sint16 *stream = (Sint16*) bytestream;
	int length = bytes / 2;
	AudioCallbackData *cbData = (AudioCallbackData*) userData;
	
	int *currentSample = &cbData->currentSample;

	for (int i = 0; i < length; i++, (*currentSample)++) {
		double time = (double)(*currentSample) / (double)AUDIO_SAMPLES_PER_SECOND;
		stream[i] = cbData->getWaveSample(DEFAULT_SQWAVE_TONE, 
				DEFAULT_SQWAVE_VOLUME, 
				DEFAULT_SQWAVE_PERMUTATIONS,
				time);
	}
}

/** initAudioInterface
 *
 * @param m_interface
 *  A reference to a pointer to AudioInterface struct representing an
 *  audio interface to be initialized
 * @description:
 *  Allocates memory for an audio interface and then populates it with
 *  appropriate data and also preparing the actual media library to be 
 *  used with a given interface
 *  TODO: add support for other media libraries compatible with CLI only mode
 */
VM_RESULT initAudioInterface(AudioInterface **m_interface) {
    VM_ASSERT(SDL_Init(SDL_INIT_AUDIO) != 0);

	*m_interface = (AudioInterface*) malloc(sizeof(AudioInterface));
	AudioInterface *interface = *m_interface;

	VM_ASSERT(interface == NULL);

	SDL_zero(interface->specWant);
	SDL_zero(interface->specHave);

	interface->specWant.freq = AUDIO_SAMPLES_PER_SECOND;
	interface->specWant.format = AUDIO_FORMAT_DEFAULT;
	interface->specWant.channels = AUDIO_CHANNELS;
	interface->specWant.samples = AUDIO_BUFFER_SIZE;

	interface->specWant.callback = processAudioCallback;
	interface->specWant.userdata = &(interface->cbData);
	interface->cbData.currentSample = 0;
	interface->cbData.getWaveSample = getSquareWave;

	interface->state = AUDIO_STATE_PAUSED;
    
    interface->deviceId = SDL_OpenAudioDevice(NULL, 0,
                &interface->specWant, &interface->specHave,
                SDL_AUDIO_ALLOW_ANY_CHANGE);

	if (!interface->deviceId) {
		free(interface);
		return VM_RESULT_ERROR;
	}

	if (interface->specWant.format != interface->specHave.format) {
		free(interface);
		return VM_RESULT_ERROR;
	}

	return VM_RESULT_SUCCESS;
}

/** destroyAudioInterface
 *
 * @param m_interface
 *  A reference to a pointer to AudioInterface struct that is to be destroyed
 * @description:
 *  Frees memory fro a given audio interface an destroyes a connection to
 *  a media library that's been used
 *  TODO: Add support for something compatible with CLI only
 */
VM_RESULT destroyAudioInterface(AudioInterface **m_interface) {
	AudioInterface *interface = *m_interface;

	if (interface != NULL) {
		free(interface);
	}

	SDL_CloseAudioDevice(interface->deviceId);
	return VM_RESULT_SUCCESS;
}

/** startBeep
 *
 * @param interface
 *  Pointer to AudioInterface struct
 * @description:
 *  Sets the audio state to AUDIO_STATE_PLAYING and
 *  signalling SDL2 library to start playing it
 *  TODO: Only for SDL2 based interface, need to add support for other libs
 */
void startBeep(AudioInterface *interface) {
	if (interface->state == AUDIO_STATE_PLAYING)
		return;

    logToFile("Beep start\n");

	interface->state = AUDIO_STATE_PLAYING;
	SDL_PauseAudioDevice(interface->deviceId, interface->state);
}

/** stopBeep
 *
 * @param interface
 *  Pointer to AudioInterface struct
 * @description:
 *  Sets the audio state to AUDIO_STATE_PAUSED and
 *  signalling SDL2 library to stop playing it
 *  TODO: Only for SDL2 based interface, need to add support for other libs
 */
void stopBeep(AudioInterface *interface) {
	if (interface->state == AUDIO_STATE_PAUSED)
		return;

    logToFile("Beep stop\n");
	interface->state = AUDIO_STATE_PAUSED;
	SDL_PauseAudioDevice(interface->deviceId, interface->state);
}

// =================================== VM Functions =================================== 

/** initVM
 *
 * @param m_vm
 *  Reference to a pointer to VM struct that is to be initialized
 * @param ROMFileName
 *  Path to a ROM file to be executed by VM
 * @param flags
 *  A byte representing 8 flags for VM
 * @description:
 *  Allocates memory and initializes VM with populating it's handler
 *  struct with appropriate data and calling all initializer functions 
 *  for each interface included in a VM (and according to the flags being set)
 */
VM_RESULT initVM(VM **m_vm, char *ROMFileName, BYTE flags) {
	*m_vm = (VM*) malloc(sizeof(VM));
	VM *vm = *m_vm;

	VM_ASSERT(vm == NULL);

	vm->audio = NULL;
	vm->video = NULL;
	vm->core = NULL;
	vm->dbg = NULL;
    vm->flags = flags;

    SDL_Init(SDL_INIT_EVENTS);
	VM_ASSERT(initVideoInterface(&vm->video) != VM_RESULT_SUCCESS);
	VM_ASSERT(initAudioInterface(&vm->audio) != VM_RESULT_SUCCESS);

	FILE *ROMHandler = fopen(ROMFileName, "r");

	VM_ASSERT(ROMHandler == NULL);

	VM_RESULT coreInitResult = initCore(&vm->core, ROMHandler);
	fclose(ROMHandler);

	VM_ASSERT(coreInitResult != VM_RESULT_SUCCESS);

    if (vm->flags & VM_FLAG_DEBUGGER) {
        if (initDebugger(&vm->dbg, vm->core) != VM_RESULT_SUCCESS) {
            printf("Failed to initialize debugger!\n");
            vm->dbg = NULL;
        }
    }

	return VM_RESULT_SUCCESS;
}

/** pollEvents
 *
 * @param vm
 *  Pointer to a VM struct
 * @description:
 *  Used to poll for keyboard events
 *  TODO: Only supports SDL2, need to add support for other libs
 */
VM_RESULT pollEvents(VM *vm, VM_RESULT dbgState) {
	VM_ASSERT(vm == NULL);
	SDL_Event ev;
	while (SDL_PollEvent(&ev)) {
		if (ev.type == SDL_QUIT) {
			return VM_RESULT_EVENT_QUIT;
		} 

		if (ev.type == SDL_KEYUP || ev.type == SDL_KEYDOWN) {
			if (ev.type == SDL_KEYUP) {
				switch (ev.key.keysym.scancode) {
					case INPUT_QUIT:
						return VM_RESULT_EVENT_QUIT;
                    default:
                        break;
				}
			}

            if (dbgState == VM_RESULT_SUCCESS) {
                WORD key = getKeyBitmask(ev.key.keysym.scancode);

                if (key != WRONG_INPUT) {
                    if (ev.type == SDL_KEYDOWN) {
                        vm->core->keypadState |= key;
                    } else if (ev.type == SDL_KEYUP) {
                        vm->core->keypadState &= ~key;
                    }
                }
            }
		}
	}

	return VM_RESULT_SUCCESS;
}

/** runVM
 *
 * @param vm
 *  Pointer to VM struct representing a VM context that is to be started
 * @description:
 *  An entry pointer for a cheap8 VM run
 *  Also holds an infinite execution loop within it and
 *  invokes debugger and such
 *  Basically handles everything
 */
VM_RESULT runVM(VM *vm) {
	VM_ASSERT(vm == NULL);

	Uint64 currentTicks = 0;
	Uint64 nextTicks = 0;
	Uint64 nextTimerTicks = 0;

	VM_RESULT runningState = VM_RESULT_SUCCESS;
    VM_RESULT dbgHeld = VM_RESULT_SUCCESS;

	vm->core->opcode = GET_WORD(vm->core->memory[vm->core->PC], vm->core->memory[vm->core->PC + 1]);
	if (vm->dbg != NULL && (vm->flags & VM_FLAG_DEBUGGER)) {
		dbgHeld = updateDebugger(vm->dbg);
    }

	while (runningState == VM_RESULT_SUCCESS) {
		currentTicks = SDL_GetTicks64();
        
        if (dbgHeld == VM_RESULT_SUCCESS) {
            nextTicks = vm->core->prevCycleTicks + CORE_TICKS_PER_CYCLE;
            nextTimerTicks = vm->core->prevTimerTicks + CORE_TICKS_PER_CYCLE;
        } else {
            nextTicks = vm->core->prevCycleTicks + CORE_TICKS_PER_CYCLE_DBG;
            nextTimerTicks = currentTicks + CORE_TICKS_PER_CYCLE;
        }

		if (currentTicks < nextTicks) {
			SDL_Delay(nextTicks - currentTicks);
		}

		vm->core->prevCycleTicks = currentTicks;

		if (currentTicks >= nextTimerTicks) {
			vm->core->tDelay -= vm->core->tDelay > 0 ? 1 : 0;
			vm->core->tSound -= vm->core->tSound > 0 ? 1 : 0;
			vm->core->prevTimerTicks = currentTicks;
		}

        if (dbgHeld == VM_RESULT_SUCCESS) {
            processOpcode(vm->core);
            vm->core->opcode = GET_WORD(vm->core->memory[vm->core->PC], vm->core->memory[vm->core->PC + 1]);
        }

		if (vm->dbg != NULL && (vm->flags & VM_FLAG_DEBUGGER)) {
			dbgHeld = updateDebugger(vm->dbg);
            if (dbgHeld == VM_RESULT_EVENT_QUIT)
                return VM_RESULT_EVENT_QUIT;
        }

        if (dbgHeld == VM_RESULT_SUCCESS) {
            if (CHECK_CUSTOM_FLAG(vm->core, CUSTOM_FLAG_REDRAW_PENDING)) {
                redrawScreen(vm->video, vm->core->gfx);
                UNSET_CUSTOM_FLAG(vm->core, CUSTOM_FLAG_REDRAW_PENDING);
            } else if (CHECK_CUSTOM_FLAG(vm->core, CUSTOM_FLAG_CLEAR_SCREEN)) {
                clearScreen(vm->video);
                UNSET_CUSTOM_FLAG(vm->core, CUSTOM_FLAG_CLEAR_SCREEN);
            }

            if (vm->core->tSound > 0)
                startBeep(vm->audio);
            else
                stopBeep(vm->audio);
        }
        
        runningState = pollEvents(vm, dbgHeld);
	}

	return runningState;
}

/** destroyVM
 *
 * @param m_vm
 *  A reference to a pointer to a VM struct that is to be destroyed
 * @description
 *  A destructor for a VM itself that calls all other destructors for 
 *  all components and then destroyes the VM itself
 */
VM_RESULT destroyVM(VM **m_vm) {
	VM *vm = *m_vm;

	VM_ASSERT(vm == NULL);

	if (vm->dbg != NULL) {
		destroyDebugger(&vm->dbg);
	}

	destroyAudioInterface(&vm->audio);
	destroyVideoInterface(&vm->video);
	destroyCore(&vm->core);

	free(vm);
	SDL_Quit();

	return VM_RESULT_SUCCESS;
}
