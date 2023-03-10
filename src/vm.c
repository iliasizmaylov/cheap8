#include "vm.h"

// ============================= Video Interface Functions =============================

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

	VM_ASSERT(interface->renderer == NULL);

	return VM_RESULT_SUCCESS;
}

VM_RESULT clearScreen(VideoInterface *interface) {
	VM_ASSERT(interface == NULL);

	SDL_RenderClear(interface->renderer);

	return VM_RESULT_SUCCESS;
}

VM_RESULT redrawScreenRow(VideoInterface *interface, WORD rowOffset, QWORD rowContent) {
	SDL_Rect nextPixel;
	WORD currentCol = 0;
	rowOffset *= interface->pixelHeight;

	for (BYTE i = SCREEN_RESOLUTION_WIDTH ; i > 0; i--) {
		GET_NEXT_PIXEL(nextPixel,
				currentCol, rowOffset,
				interface->pixelWidth,
				interface->pixelHeight);
		
		if ((rowContent >> (i - 1)) & 1) {
			SDL_SetRenderDrawColor(interface->renderer, VIDEO_WHITE_PIXEL_RGBO);
		} else {
			SDL_SetRenderDrawColor(interface->renderer, VIDEO_BLACK_PIXEL_RGBO);
		}

		SDL_RenderFillRect(interface->renderer, &nextPixel);
		currentCol += interface->pixelWidth;
	}

	return VM_RESULT_SUCCESS;
}

VM_RESULT redrawScreen(VideoInterface *interface, QWORD *screen) {
	VM_ASSERT(interface == NULL);

	for (BYTE i = 0; i < SCREEN_RESOLUTION_HEIGHT; i++) {
		redrawScreenRow(interface, i, screen[i]);
	}

	SDL_RenderPresent(interface->renderer);

	return VM_RESULT_SUCCESS;
}

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

Sint16 getSquareWave(float tone, Uint16 volume, Uint16 permutations, double time) {
	Sint16 sample = 0;
	for (int i = 0; i < permutations; i++) {
		sample += volume * sin((2 * i + 1) * tone * time) / (2 * i + 1);
	}
	return sample;
}

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

VM_RESULT initAudioInterface(AudioInterface **m_interface) {
	if (SDL_Init(SDL_INIT_AUDIO) != 0) {
		return VM_RESULT_ERROR;
	}	

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

	if (SDL_OpenAudio(&(interface->specWant), &(interface->specHave)) != 0) {
		free(interface);
		return VM_RESULT_ERROR;
	}

	if (interface->specWant.format != interface->specHave.format) {
		free(interface);
		return VM_RESULT_ERROR;
	}

	return VM_RESULT_SUCCESS;
}

VM_RESULT destroyAudioInterface(AudioInterface **m_interface) {
	AudioInterface *interface = *m_interface;

	if (interface != NULL) {
		free(interface);
	}
	
	SDL_CloseAudio();
	return VM_RESULT_SUCCESS;
}

void startBeep(AudioInterface *interface) {
	if (interface->state == AUDIO_STATE_PLAYING) {
		return;
	}

	interface->state = AUDIO_STATE_PLAYING;
	SDL_PauseAudio(interface->state);
}

void stopBeep(AudioInterface *interface) {
	if (interface->state == AUDIO_STATE_PAUSED) {
		return;
	}

	interface->state = AUDIO_STATE_PAUSED;
	SDL_PauseAudio(interface->state);
}

// =================================== VM Functions =================================== 

VM_RESULT initVM(VM **m_vm, char *ROMFileName, BYTE flags) {
	*m_vm = (VM*) malloc(sizeof(VM));
	VM *vm = *m_vm;

	VM_ASSERT(vm == NULL);

	vm->audio = NULL;
	vm->video = NULL;
	vm->core = NULL;
	vm->dbg = NULL;
    vm->flags = flags;

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

VM_RESULT pollEvents(VM *vm) {
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

	return VM_RESULT_SUCCESS;
}

VM_RESULT runVM(VM *vm) {
	VM_ASSERT(vm == NULL);

	Uint64 currentTicks;
	Uint64 nextTicks;
	Uint64 nextTimerTicks;
	
	BYTE isRunning = 1;

	vm->core->opcode = GET_WORD(vm->core->memory[vm->core->PC], vm->core->memory[vm->core->PC + 1]);
	if (vm->dbg != NULL && (vm->flags & VM_FLAG_DEBUGGER)) {
		updateDebugger(vm->dbg);
	}

	while (isRunning == VM_RESULT_SUCCESS) {
		currentTicks = SDL_GetTicks64();
		nextTicks = vm->core->prevCycleTicks + CORE_TICKS_PER_CYCLE;
		nextTimerTicks = vm->core->prevTimerTicks + CORE_TICKS_PER_TIMER;

		if (currentTicks < nextTicks) {
			SDL_Delay(nextTicks - currentTicks);
		}

		vm->core->prevCycleTicks = currentTicks;

		if (currentTicks >= nextTimerTicks) {
			vm->core->tDelay -= vm->core->tDelay > 0 ? 1 : 0;
			vm->core->tSound -= vm->core->tSound > 0 ? 1 : 0;
			vm->core->prevTimerTicks = currentTicks;
		}

		processOpcode(vm->core);
		vm->core->opcode = GET_WORD(vm->core->memory[vm->core->PC], vm->core->memory[vm->core->PC + 1]);
		if (vm->dbg != NULL && (vm->flags & VM_FLAG_DEBUGGER)) {
			updateDebugger(vm->dbg);
		}

		if (CHECK_CUSTOM_FLAG(vm->core, CUSTOM_FLAG_REDRAW_PENDING)) {
			redrawScreen(vm->video, vm->core->gfx);
			UNSET_CUSTOM_FLAG(vm->core, CUSTOM_FLAG_REDRAW_PENDING);
		}

		if (vm->core->tSound > 0) {
			startBeep(vm->audio);
		} else {
			stopBeep(vm->audio);
		}

		isRunning = pollEvents(vm);
	}

	return VM_RESULT_SUCCESS;
}

VM_RESULT destroyVM(VM **m_vm) {
	VM *vm = *m_vm;

	VM_ASSERT(vm == NULL);

	destroyAudioInterface(&vm->audio);
	destroyVideoInterface(&vm->video);
	destroyCore(&vm->core);

	if (vm->dbg != NULL) {
		destroyDebugger(&vm->dbg);
	}

	free(vm);
	SDL_Quit();

	return VM_RESULT_SUCCESS;
}
