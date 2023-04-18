/**
 * Cheap-8: a chip-8 emulator
 *
 * File: vm.h
 * License: DWYW - "Do Whatever You Want"
 *
 * Declaration of function that handle most of the logic in the VM
 */

#ifndef _VM_H_
#define _VM_H_

#include "opcodes.h"
#include "c8debug.h"

// ============================= Video Interface Definition =============================

#define VIDEO_DEFAULT_RESOLUTION_WIDTH		(SCREEN_RESOLUTION_WIDTH * 16)
#define VIDEO_DEFAULT_RESOLUTION_HEIGHT		(SCREEN_RESOLUTION_HEIGHT * 16)
#define VIDEO_DEFAULT_FLAGS					0
#define VIDEO_DEFAULT_WINDOW_TITLE			"Cheap-8"
#define VIDEO_DEFAULT_RENDER_INDEX			-1
#define VIDEO_DEFAULT_RENDER_FLAGS			0

#define VIDEO_BLACK_PIXEL_RGBO				0x00, 0x00, 0x00, 0xFF
#define VIDEO_WHITE_PIXEL_RGBO				0xFF, 0xFF, 0xFF, 0xFF

#define GET_NEXT_PIXEL(pxl, xc, yc, width, height) \
	pxl.x = xc; \
	pxl.y = yc; \
	pxl.w = width; \
	pxl.h = height;

typedef struct _VideoInterface {
	SDL_Window *window;
	SDL_Renderer *renderer;
	WORD resolutionWidth;
	WORD resolutionHeight;
	BYTE isFullScreen;
	DWORD windowFlags;
	BYTE pixelWidth;
	BYTE pixelHeight;
	const char *windowTitle;
} VideoInterface;

VM_RESULT initVideoInterface(VideoInterface **m_interface);
VM_RESULT clearScreen(VideoInterface *interface);
VM_RESULT redrawPixel(VideoInterface *interface, QWORD *screen, WORD row, WORD col);
VM_RESULT redrawScreen(VideoInterface *interface, QWORD *screen);
VM_RESULT destroyVideoInterface(VideoInterface **m_interface);

// ============================= Audio Interface Definition =============================

#define AUDIO_SAMPLES_PER_SECOND		44100
#define AUDIO_FORMAT_DEFAULT			AUDIO_S16SYS
#define AUDIO_CHANNELS					1
#define AUDIO_BUFFER_SIZE				1024
#define AUDIO_BYTES_PER_SAMPLE			sizeof(Uint16) * 2

#define DEFAULT_SQWAVE_TONE				1101.0f
#define DEFAULT_SQWAVE_VOLUME			3000
#define DEFAULT_SQWAVE_PERMUTATIONS		300

#define AUDIO_STATE_PLAYING				0
#define AUDIO_STATE_PAUSED				1

typedef Sint16 (*WAVE_FUNCTION)(float, Uint16, Uint16, double);

typedef struct _AudioCallbackData {
	int currentSample;
	WAVE_FUNCTION getWaveSample;
} AudioCallbackData;

typedef struct _AudioInterface {
	SDL_AudioSpec specWant;
	SDL_AudioSpec specHave;
	AudioCallbackData cbData;
	BYTE state;
    SDL_AudioDeviceID deviceId;
} AudioInterface;

VM_RESULT initAudioInterface(AudioInterface **m_interface);
void processAudioCallback(void *userData, Uint8 *bytestream, int bytes);
Sint16 getSquareWave(float tone, Uint16 volume, Uint16 permutations, double time);
VM_RESULT destroyAudioInterface(AudioInterface **m_interface);
void startBeep(AudioInterface *interface);
void stopBeep(AudioInterface *interface);

// =================================== VM Definition ===================================

// XXX: This is the only flag I need currently
//      Maybe in the future I will need more flags (which is honestly unlikely)
#define VM_FLAG_DEBUGGER    1 << 0

typedef struct _VM {
	AudioInterface *audio;
	VideoInterface *video;
	C8core *core;
	Debugger *dbg;

    BYTE flags;
} VM;

VM_RESULT initVM(VM **m_vm, char *ROMFileName, BYTE flags);
VM_RESULT pollEvents(VM *vm, VM_RESULT dbgState);
VM_RESULT runVM(VM *vm);
VM_RESULT destroyVM(VM **m_vm);

#endif  /* _VM_H_ */
