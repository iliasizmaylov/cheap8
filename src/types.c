/**
 * Cheap-8: a chip-8 emulator
 * 
 * File: types.c
 * License: DWYW - "Do Whatever You Want"
 * 
 * Nothing here at this point. Created the file just in case I need it later.
 */

#include "types.h"

// Simple function for logging
// It's either going to be cut from the final version or will
// evolve into the full proper logging system
void logToFile(const char* fmt, ...) {
    va_list vaargs;
    va_start(vaargs, fmt);
    
    FILE *logFile = fopen(DEFAULT_LOG_FILE, "ab+");
    if (logFile) {
        fprintf(logFile, fmt, vaargs);
        fprintf(logFile, "\n");
    }

    fclose(logFile);
    va_end(vaargs);
}
