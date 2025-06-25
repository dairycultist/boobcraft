#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>

#define TRUE 1
#define FALSE 0

int main() {

	printf("starting\n");

	SDL_Init(SDL_INIT_VIDEO);

	SDL_Window *window = SDL_CreateWindow("bober", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 700, 500, 0);

	SDL_Event e;
	int quit = FALSE;
	while (!quit){
		while (SDL_PollEvent(&e)){
			if (e.type == SDL_QUIT){
				quit = TRUE;
			}
			if (e.type == SDL_KEYDOWN){
				quit = TRUE;
			}
			if (e.type == SDL_MOUSEBUTTONDOWN){
				quit = TRUE;
			}
		}
	}

	SDL_DestroyWindow(window);
	SDL_Quit();

    return 0;
}