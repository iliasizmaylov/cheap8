#include "vm.h"

// To parse command line arguments with getopt
#include <unistd.h>

#define DEMO_ROM_FILE	"../chip8-roms/demos/Maze [David Winter, 199x].ch8"
#define DEMO_ROM_NAME	"Maze"

/* ====================== CONSOLE ARGUMENTS ======================= */

struct program_param {
    const char letter;
    const char description[1 << 9];
    const BYTE is_bool;
};

const struct program_param param_debug_on = {
    .letter = 'd',
    .description = "Usage: -d; When specified it launches emulator along with a debugger",
    .is_bool = 1,
};

const struct program_param param_rom_path = {
    .letter = 'r',
    .description = "Usage: -r [PATH_TO_ROM]; Specify which ROM image to run, runs a Maze demo if left empty",
    .is_bool = 0,
};

const struct program_param param_help = {
    .letter = 'h',
    .description = "Show this help message",
    .is_bool = 1,
};

#define PROGRAM_PARAM_COUNT 3

const struct program_param* const params[PROGRAM_PARAM_COUNT] = {&param_debug_on, &param_rom_path, &param_help};
const char *getopt_param_string = "dr:h";

/* ====================== PROGRAM DESCRIPTION ===================== */

struct program_info {
    const char name[1 << 7];
    const char version[1 << 5];
    const char description[1 << 10];
    BYTE vmflags;
};

const struct program_info cheap8_info = {
    .name = "cheap8",
    .version = "1.0 ALPHA",
    .description = "A chip-8 emulator made using SDL2, it has a debugger with a curses based TUI",
    .vmflags = 0
};

const struct program_info* const current_version = &cheap8_info;


/* ====================== UTILITY FUNCTIONS ======================= */

// Just prints the help message
void print_help() {
    printf("Program: %s (ver. %s)\nDescription: %s\nOptions:\n", 
            current_version->name, 
            current_version->version, 
            current_version->description); 

    for (BYTE i = 0; i < PROGRAM_PARAM_COUNT; i++) {
        printf("\t-%c, %s\n", 
                params[i]->letter,
                params[i]->description);
    }

    printf("\n");
}

/* ====================== PROGRAM MAIN ENTRY ====================== */

int main (int argc, char **argv) {
	VM *Chip8VirtualMachine;
    char ROMFile[1 << 9] = "";

    BYTE vmFlags = 0;

    int opt;
    while ((opt = getopt(argc, argv, getopt_param_string)) != -1) {
        switch(opt) {
            case 'd':
                vmFlags |= VM_FLAG_DEBUGGER;
                printf("Opening debugger...\n");
                break;
            case 'r':
                if (optarg)
                    strcpy(ROMFile, optarg);
                break;
            case 'h':
                print_help();
                return 0;
                break;
            default:
                printf("Error: unknown option -%c\n\n", opt);
                print_help();
                return 0;
        }
    }

	if (strcmp(ROMFile, "") == 0) {
		strcpy(ROMFile, DEMO_ROM_FILE);
	} else if (access(ROMFile, F_OK) != 0) {
        printf("Error: Provided ROM file \"%s\" doesn't exist\n\n", ROMFile);
        print_help();
        return 0;
    }

    printf("Using ROM \"%s\"...\n", ROMFile);

	if (initVM(&Chip8VirtualMachine, ROMFile, vmFlags) == VM_RESULT_SUCCESS) {
		runVM(Chip8VirtualMachine);
	}

	destroyVM(&Chip8VirtualMachine);

	return 0;
}
