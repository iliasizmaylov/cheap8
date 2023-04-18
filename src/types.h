/**
 * Cheap-8: a chip-8 emulator
 *
 * File: types.h
 * License: DWYW - "Do Whatever You Want"
 *
 * Including libraries related to all code in cheap8
 * Also some macros including debug and assert type macros
 * Some typedefs
 * That kind of stuff
 */

#ifndef _TYPES_H_
#define _TYPES_H_

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>

#include <time.h>
#include <math.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_audio.h>

#if defined(__STDC__)
#	define PREDEF_STANDARD_C_1989
#	if defined(__STDC_VERSION__)
#		define PREDEF_STANDARD_C_1990
#		if (__STDC_VERSION__ >= 201112L)
#			define PREDEF_STANDARD_C_2011
#		endif
#		if (__STDC_VERSION__ >= 199409L)
#			define PREDEF_STANDARD_C_1994
#		endif
#		if (__STDC_VERSION__ >= 199901L)
#			define PREDEF_STANDARD_C_1999
#		endif
#		if (__STDC_VERSION__ >= 201710L)
#			define PREDEF_STANDARD_C_2018
#		endif
#	endif
#endif

#ifdef PREDEF_STANDARD_C_1990
#	define __FUNC__ __func__
#else
#	define __FUNC__ __FUNCTION__
#endif

/* All these typedefs are totally unnecessary but I created them to
 * increase readablity for me due to my preference
 */
typedef uint8_t BYTE;
typedef uint16_t WORD;
typedef uint32_t DWORD;
typedef uint64_t QWORD;

// Various result codes for cheap8
typedef enum {
	VM_RESULT_ERROR,
	VM_RESULT_SUCCESS,
    VM_RESULT_DBG,
	VM_RESULT_EVENT_QUIT,
    VM_RESULT_EVENT_SIGINT,
	VM_RESULT_WARNING,

    VM_RESULT_COUNT
} _VM_RESULT;

typedef BYTE VM_RESULT;

/* Both these macros are used to print debug info on exceptions (well, they do asserts, evidently)
 * VM_ASSERT should be called from within a function that returns VM_RESULT (or BYTE) type
 *
 * XXX: I am honestly not sure if it's a good idea to handle asserts that way but currently I'm okay with that
 * VM_ASSERT is intended for hard faults that program won't be able to recover from
 * Whatever, I'm not that cool of a programmer anyway
 */
#define GET_EX_STR(x)	#x
#define VM_ASSERT(ex) \
	do {\
		if (ex) { \
			printf("[VM ASSERT] "); \
			printf("%s @ %s @ %d : ", __FILE__, __FUNC__, __LINE__); \
			printf(GET_EX_STR(ex)); \
			printf("\n"); \
			return VM_RESULT_ERROR; \
		} \
	} while(0)

#define DEFAULT_LOG_FILE "c8.log"
void logToFile(char *fmt, ...);

#endif  /* _TYPES_H_ */
