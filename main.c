#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>

int main() {

	printf("starting\n");

	SDL_CreateWindow("bober", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 200, 200, 0);

	usleep(50000); // this is in MICROSECONDS (1/1000 of a millisecond)

    return 0;
}