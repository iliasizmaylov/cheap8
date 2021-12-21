#include "vm.h"

// ============================= Video Interface Definition =============================

VM_RESULT initVideoInterface(VideoInterface **m_interface) {
	*m_interface = (VideoInterface*) malloc(sizeof(VideoInterface));
	VideoInterface *interface = *m_interface;
	VM_ASSERT(interface == NULL);
	
	interface->window = NULL;
	interface->renderer = NULL;

	interface->resolutionWidth = VIDEO_DEFAULT_RESOLUTION_WIDTH;
	interface->resolutionHeight = VIDEO_DEFAULT_RESOLUTION_HEIGHT;
	interface->isFullScreen = 0;
	interface->windowFlags = VIDEO_DEFAULT_FLAGS;
	interface->pixelWidth = interface->resolutionWidth / SCREEN_RESOLUTION_WIDTH;
	interface->pixelHeight = interface->resolutionHeight / SCREEN_RESOLUTION_HEIGHT;
	interface->windowTitle = VIDEO_DEFAULT_WINDOW_TITLE;

	interface->window = SDL_CreateWindow(
			interface->windowTitle,
			SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
			interface->resolutionWidth, interface->resolutionHeight,
			interface->windowFlags);

	VM_ASSERT(interface->window == NULL);

	interface->renderer = SDL_CreateRenderer(
			interface->window,
			VIDEO_DEFAULT_RENDER_INDEX,
			VIDEO_DEFAULT_RENDER_FLAGS);
	
	VM_ASSERT(interface->renderer == NULL);

	return VM_RESULT_SUCCESS;
}

VM_RESULT clearScreen(VideoInterface *interface) {
	SDL_SetRenderDrawColor(interface->renderer, VIDEO_BLACK_PIXEL_RGBO);
	SDL_RenderClear(interface->renderer);
	SDL_RenderPresent(interface->renderer);

	return VM_RESULT_SUCCESS;
}

VM_RESULT redrawScreenRow(VideoInterface *interface, BYTE rowOffset, QWORD rowContent) {
	currentPixel = 0;
	rowOffset *= interface->pixelHeight;
	SDL_Rect pixel;

	for (BYTE i = SCREEN_RESOLUTION_WIDTH - 1; i >= 0; i--, currentPixel += 1) {
		GET_NEXT_PIXEL(pixel,
				currentPixel * interface->pixelWidth,
				rowOffset,
				interface->pixelWidth;
				interface->pixelHeight);
		
		if (((rowContent >> i) & 1)) {
			SDL_SetRenderDrawColor(interface->renderer, VIDEO_PIXEL_WHITIE_RGBO);
		} else {
			SDL_SetRenderDrawColor(interface->renderer, VIDEO_PIXEL_BLACK_RGBO);
		}

		SDL_RenderFillRect(interface->renderer, &rect);
	}

	return VM_RESULT_SUCCESS;
}

VM_RESULT redrawScreen(VideoInterface *interface, QWORD *screen) {
	VM_ASSERT(interface == NULL);

	BYTE isScreenNotEmpty = 0;
	for (BYTE i = 0; i < SCREEN_RESOLUTION_HEIGHT; i++) {
		isScreenNotEmpty |= screen[i];
	}

	if (!isScreenNotEmpty) {
		clearScreen(interface);
		return VM_RESULT_SUCCESS;
	}

	for (BYTE i = 0; i < SCREEN_RESOLUTION_HEIGHT; i++) {
		redrawScreenRow(interface, i, screen[i]);
	}

	SDL_RenderPresent(interface->renderer);
	return VM_RESULT_SUCCESS;
}

VM_RESULT destroyVideoInterface(VideoInterface *interface) {
	VM_ASSERT(!SDL_WasInit(SDL_INIT_VIDEO));
	if (interface->window != NULL) {
		SDL_DestroyWindow(interface->window);
	}
	return VM_RESULT_SUCCESS;
}

// ============================= Audio Interface Definition =============================

VM_RESULT initAudioInterface(AudioInterface **m_interface) {
	*m_interface = (AudioInterface*) malloc(sizeof(AudioInterface));
	AudioInterface *interface = *m_interface;
}

// =================================== VM Definition ===================================

