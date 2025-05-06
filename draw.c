#include <stdio.h>
#include <stdlib.h>

// we're using 216 color for now

// ANSI https://stackoverflow.com/questions/4842424/list-of-ansi-color-escape-sequences

// \033[38;2;<r>;<g>;<b>m     #Select RGB foreground color
// \033[48;2;<r>;<g>;<b>m     #Select RGB background color

#define WIDTH 80
#define HEIGHT 40

char R[WIDTH][HEIGHT], G[WIDTH][HEIGHT], B[WIDTH][HEIGHT]; // [0,5]
char x, y;

#define RESET_COLOR() printf("\033[0m");
#define NEWLINE() printf("\033[0m\n");
#define CLEAR_SCREEN() printf("\033[2J");
#define RESET_CURSOR() printf("\033[0;0H");

void print_2pix(int top_r, int top_g, int top_b, int bot_r, int bot_g, int bot_b) {

	static char short_short_2pix[] = "\033[38;5;XX;48;5;XXm▄";
	static char short_long_2pix[] = "\033[38;5;XX;48;5;111m▄";
	static char long_short_2pix[] = "\033[38;5;XXX;48;5;11m▄";
	static char long_long_2pix[] = "\033[38;5;XXX;48;5;111m▄";

	int topc = 16 + top_b + top_g * 6 + top_r * 36;
	int botc = 16 + bot_b + bot_g * 6 + bot_r * 36;

	// 48 is 0
	if (botc < 100) {

		if (topc < 100) {

			short_short_2pix[7] = (char) (botc / 10 + 48);
			short_short_2pix[8] = (char) (botc % 10 + 48);
			short_short_2pix[15] = (char) (topc / 10 + 48);
			short_short_2pix[16] = (char) (topc % 10 + 48);
			printf("%s", short_short_2pix);

		} else {

			short_long_2pix[7] = (char) (botc / 10 + 48);
			short_long_2pix[8] = (char) (botc % 10 + 48);
			short_long_2pix[15] = (char) (topc / 100 + 48);
			short_long_2pix[16] = (char) ((topc / 10) % 10 + 48);
			short_long_2pix[17] = (char) (topc % 10 + 48);
			printf("%s", short_long_2pix);
		}

	} else {

		if (topc < 100) {

			long_short_2pix[7] = (char) (botc / 100 + 48);
			long_short_2pix[8] = (char) ((botc / 10) % 10 + 48);
			long_short_2pix[9] = (char) (botc % 10 + 48);
			long_short_2pix[16] = (char) (topc / 10 + 48);
			long_short_2pix[17] = (char) (topc % 10 + 48);
			printf("%s", long_short_2pix);

		} else {

			long_long_2pix[7] = (char) (botc / 100 + 48);
			long_long_2pix[8] = (char) ((botc / 10) % 10 + 48);
			long_long_2pix[9] = (char) (botc % 10 + 48);
			long_long_2pix[16] = (char) (topc / 100 + 48);
			long_long_2pix[17] = (char) ((topc / 10) % 10 + 48);
			long_long_2pix[18] = (char) (topc % 10 + 48);
			printf("%s", long_long_2pix);
		}
	}
}

void draw_buffer() {

	RESET_CURSOR();

	for (y = 0; y < HEIGHT; y += 2) {

		for (x = 0; x < WIDTH; x++) {

			print_2pix(R[x][y], G[x][y], B[x][y], R[x][y + 1], G[x][y + 1], B[x][y + 1]);
		}

		NEWLINE();
	}
}

void clear_buffer() {

	for (y = 0; y < HEIGHT; y++) {

		for (x = 0; x < WIDTH; x++) {

			R[x][y] = 0;
			G[x][y] = 0;
			B[x][y] = 0;
		}
	}
}