#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "draw.c"

int main() {

	CLEAR_SCREEN();

	int i = 0;

	while (i < 1000) {

		for (y = 0; y < HEIGHT; y += 2) {

			for (x = 0; x < WIDTH; x++) {

				if (x < 20) {
					R[x][y] = (i / 6) % 6;
					R[x][y+1] = (i / 6) % 6;
					G[x][y] = i % 6;
					G[x][y+1] = i % 6;
					B[x][y] = (i / 36) % 6;
					B[x][y+1] = (i / 36) % 6;
				}
			}
		}

		draw_buffer();

		i++;

		usleep(50000); // this is in MICROSECONDS (1/1000 of a millisecond)
	}

	RESET_COLOR();

    return 0;
}