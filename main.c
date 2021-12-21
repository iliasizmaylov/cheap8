#include "vm.h"

int main (int argc, char **argv) {
	VM *Chip8VirtualMachine;
	char *ROMFile = NULL;

	if (argc > 1) {
		ROMFile = argv[1];
	}

	if (ROMFile == NULL) {
		printf("Specify a .ch8 ROM file\n");
		return 0;
	}

	if (initVM(&Chip8VirtualMachine, ROMFile) == VM_RESULT_SUCCESS) {
		runVM(Chip8VirtualMachine);
	}

	destroyVM(Chip8VirtualMachine);

	return 0;
}
