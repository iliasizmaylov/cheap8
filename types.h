#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

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

typedef uint8_t BYTE;
typedef uint16_t WORD;
typedef uint32_t DWORD;
typedef uint64_t QWORD;

typedef enum {
	VM_RESULT_ERROR,
	VM_RESULT_SUCCESS,
	VM_RESULT_WARNING
} _VM_RESULT;

typedef BYTE VM_RESULT;

// TODO: printing exception like that is temporary
//	but something tells me that it will stay this way
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
