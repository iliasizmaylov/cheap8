#include "vm.h"

#define DEMO_ROM_FILE	"maze_test.ch8"
#define DEMO_ROM_NAME	"Maze"

int main (int argc, char **argv) {
	VM *Chip8VirtualMachine;
	char *ROMFile = NULL;

	if (argc > 1) {
		ROMFile = argv[1];
	}

	if (ROMFile == NULL) {
		ROMFile = DEMO_ROM_FILE;
		printf("Using demo ROM \"%s\" (%s)\n", DEMO_ROM_NAME, DEMO_ROM_FILE);
	}

	if (initVM(&Chip8VirtualMachine, ROMFile) == VM_RESULT_SUCCESS) {
		runVM(Chip8VirtualMachine);
	}

	destroyVM(&Chip8VirtualMachine);

	return 0;
}
