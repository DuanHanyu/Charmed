#include "lib/boardInterface.h"
#include <stdlib.h>

int main() {
	BoardAnalyse   boardAnalyser;
	BoardInterface boardInterface(boardAnalyser);
	printf("Use numbers to place a piece, h for help, 0 to exit.\n");
	printf("The game is on ->\n");

	// boardInterface.normalMode();

	// there was something normal going on here, but not anymore
	boardInterface.controlMode();

	cout << "Over.\n";
	system("pause");
	return 0;
}