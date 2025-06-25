#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#define TRUE 1
#define FALSE 0
#define PROGRAM_NAME "Boobcraft"

// maybe move to a util.c file
void crash(const char *msg) {
	
	if (strlen(SDL_GetError()) == 0) {
		fprintf(stderr, "\n%s: <No error given>\n\n", msg);
	} else {
		fprintf(stderr, "\n%s: %s\n\n", msg, SDL_GetError());
	}
	
	exit(1);
}

int main() {

	printf("Starting %s\n", PROGRAM_NAME);

	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		crash("Could not initialize SDL");
	}

	// init OpenGL
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

	// window
	SDL_Window *window = SDL_CreateWindow(PROGRAM_NAME, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 700, 500, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

	if (!window) {
        crash("Could not create window");
    }

	SDL_GLContext context = SDL_GL_CreateContext(window);

	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);

    if (!renderer) {
        crash("Could not create renderer");
    }

    SDL_SetRenderDrawColor(renderer, 0, 100, 0, 255);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);

	// process events until window is closed
	SDL_Event event;
	int quit = FALSE;

	while (!quit) {

		while (SDL_PollEvent(&event)) {

			if (event.type == SDL_QUIT) {
				quit = TRUE;
			}
		}

		SDL_GL_SwapWindow(window);
	}

	SDL_DestroyWindow(window);
	SDL_GL_DeleteContext(context);
	SDL_Quit();

    return 0;
}