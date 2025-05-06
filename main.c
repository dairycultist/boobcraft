#include <stdio.h>
#include <stdlib.h>
#include "draw.c"

int main() {

	CLEAR_SCREEN();

	int r, g, b;

	for (r = 0; r < 6; r += 2) {
		for (b = 0; b < 6; b++) {

			print_2pix(r, 0, b, r + 1, 0, b);
		}
		NEWLINE();
	}

	RESET_COLOR();

    return 0;
}