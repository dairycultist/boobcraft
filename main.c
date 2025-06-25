#include "include/SDL.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>

int main() {

	printf("starting %d\n", SDL_WINDOWPOS_CENTERED);

	usleep(50000); // this is in MICROSECONDS (1/1000 of a millisecond)

    return 0;
}