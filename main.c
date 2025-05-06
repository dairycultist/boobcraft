#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include "draw.c"

int main() {

	CLEAR_SCREEN();

	int r, g, b, i = 0;

	while (i < 30) {

		for (r = 0; r < 6 - abs(i % 10 - 5); r++) {

			for (g = 0; g < 6; g++) {

				for (b = 0; b < 6; b++) {

					x = 30 + b * 2 - g * 2;
					y = 20 + g + b - r * 2;

					R[x][y] = r;
					R[x+1][y] = r;
					R[x+1][y+1] = r;
					R[x][y+1] = r;

					G[x][y] = g;
					G[x+1][y] = g;
					G[x+1][y+1] = g;
					G[x][y+1] = g;

					B[x][y] = b;
					B[x+1][y] = b;
					B[x+1][y+1] = b;
					B[x][y+1] = b;
				}
			}
		}

		draw_buffer();
		clear_buffer();
		usleep(100000); // this is in MICROSECONDS (1/1000 of a millisecond)

		i++;
	}

	RESET_COLOR();

    return 0;
}