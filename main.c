#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "draw.c"

#define WIDTH 80
#define HEIGHT 40

char R[WIDTH][HEIGHT], G[WIDTH][HEIGHT], B[WIDTH][HEIGHT]; // [0,5]

int main() {

	CLEAR_SCREEN();

	int x, y;

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

		RESET_CURSOR();

		for (y = 0; y < HEIGHT; y += 2) {

			for (x = 0; x < WIDTH; x++) {

				print_2pix(R[x][y], G[x][y], B[x][y], R[x][y + 1], G[x][y + 1], B[x][y + 1]);
			}

			NEWLINE();
		}

		i++;

		usleep(100000);
	}

	RESET_COLOR();

    return 0;
}