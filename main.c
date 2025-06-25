#include <SDL.h>

#define TRUE 1
#define FALSE 0

void crash(const char *msg) {
	
	if (strlen(SDL_GetError()) == 0) {
		fprintf(stderr, "\n%s: <No error given>\n\n", msg);
	} else {
		fprintf(stderr, "\n%s: %s\n\n", msg, SDL_GetError());
	}
	
	exit(1);
}

int main() {

	printf("starting\n");

	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		crash("Could not initialize SDL");
	}

	SDL_Window *window = SDL_CreateWindow("Boobcraft", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 700, 500, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

	if (!window) {
        crash("Could not create window");
    }

	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);

    if (!renderer) {
        crash("Could not create renderer");
    }

    SDL_SetRenderDrawColor(renderer, 0, 100, 0, 255);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);

	// process events until window is closed
	SDL_Event e;
	int quit = FALSE;

	while (!quit) {

		while (SDL_PollEvent(&e)) {

			if (e.type == SDL_QUIT) {
				quit = TRUE;
			}
		}
	}

	SDL_DestroyWindow(window);
	SDL_Quit();

    return 0;
}