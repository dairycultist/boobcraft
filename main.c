#include <stdio.h>
#include <stdlib.h>

// we're using 216 color for now

// ANSI https://stackoverflow.com/questions/4842424/list-of-ansi-color-escape-sequences

// \033[38;2;<r>;<g>;<b>m     #Select RGB foreground color
// \033[48;2;<r>;<g>;<b>m     #Select RGB background color

#define RESET_COLOR() printf("\033[0m");
#define NEWLINE() printf("\033[0m\n");
#define CLEAR_SCREEN() printf("\033[2J");

void print_2pix(int topc, int botc) {

	static char short_short_2pix[] = "\033[38;5;XX;48;5;XXm▄";
	static char short_long_2pix[] = "\033[38;5;XX;48;5;111m▄";
	static char long_short_2pix[] = "\033[38;5;XXX;48;5;11m▄";
	static char long_long_2pix[] = "\033[38;5;XXX;48;5;111m▄";

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

			printf("%s", long_short_2pix);

		} else {

		}
	}
}

int main() {

	CLEAR_SCREEN();

	int r, g, b;

	for (r = 0; r < 6; r += 2) {
		for (b = 0; b < 6; b++) {

			int topc = 16 + b + r * 36;
			int botc = 16 + b + (r+1) * 36;

			print_2pix(topc, 16);
		}
		NEWLINE();
	}

	RESET_COLOR();

    return 0;
}