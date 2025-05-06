#include <stdio.h>
#include <stdlib.h>

// we're using 216 color for now

// ANSI https://stackoverflow.com/questions/4842424/list-of-ansi-color-escape-sequences

// \033[38;2;<r>;<g>;<b>m     #Select RGB foreground color
// \033[48;2;<r>;<g>;<b>m     #Select RGB background color

#define PRINT_2PIX(topc, botc) printf("\033[38;5;" #botc ";48;5;" #topc "m▄");

#define RESET_COLOR() printf("\033[0m");

int main() {

	PRINT_2PIX(149, 58);
	RESET_COLOR();
    return 0;
}