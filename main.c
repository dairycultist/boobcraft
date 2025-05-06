#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include "draw.c"

int main() {

	CLEAR_SCREEN();

	int i = 20;

	while (i < 1000) {

		for (y = 0; y < HEIGHT; y++) {

			for (x = 0; x < WIDTH; x++) {

				int dist = sqrt(pow(x - 40, 2) + pow(y - 20, 2));

				if (dist < 20) {
					R[x][y] = abs((i - dist) % 10 - 5);
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