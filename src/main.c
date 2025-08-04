#define GLEW_STATIC
#include "../GLEW/glew.h"
#include <SDL2/SDL.h>

#include "engine.h"
#include "util.c"
#include "3D.c"

int main() {

	printf("Starting %s\n", get_title());

	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		log_error("Could not initialize SDL");
		return 1;
	}

	// init OpenGL
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

	// create the window
	SDL_Window *window = SDL_CreateWindow(get_title(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 400, 240, SDL_WINDOW_OPENGL);

	if (!window) {
        log_error("Could not create window");
		return 1;
    }

	SDL_GLContext context = SDL_GL_CreateContext(window);

	glewExperimental = GL_TRUE;
	glewInit();

	// enable depth buffer
	glEnable(GL_DEPTH_TEST);

	// enable backface culling
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CW);

	// other opengl stuff
	glClearColor(0.2f, 0.2f, 0.23f, 1.0f);
	SDL_SetRelativeMouseMode(SDL_TRUE);

	// initialize static values used by 3D.c
	initialize_3D_static_values();
	
	// let programmer initialize stuff
	on_start();

	// process events until window is closed
	SDL_Event event;
	int running = TRUE;

	int up        = FALSE;
	int down      = FALSE;
	int left      = FALSE;
	int right     = FALSE;
	int trigger_1 = FALSE;
	int trigger_2 = FALSE;
	int menu      = FALSE;

	while (running) {

		while (SDL_PollEvent(&event)) {

			if (event.type == SDL_QUIT) {

				running = FALSE;

			} else if (event.type == SDL_KEYDOWN && event.key.repeat == 0 && event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {

				SDL_SetRelativeMouseMode(!SDL_GetRelativeMouseMode());

			} else if (SDL_GetRelativeMouseMode()) {

				if (event.type == SDL_KEYDOWN && event.key.repeat == 0) {

					if (event.key.keysym.scancode == SDL_SCANCODE_LEFT) {
						left = TRUE;
					} else if (event.key.keysym.scancode == SDL_SCANCODE_RIGHT) {
						right = TRUE;
					} else if (event.key.keysym.scancode == SDL_SCANCODE_UP) {
						up = TRUE;
					} else if (event.key.keysym.scancode == SDL_SCANCODE_DOWN) {
						down = TRUE;
					} else if (event.key.keysym.scancode == SDL_SCANCODE_Z) {
						trigger_1 = TRUE;
					} else if (event.key.keysym.scancode == SDL_SCANCODE_X) {
						trigger_2 = TRUE;
					} else if (event.key.keysym.scancode == SDL_SCANCODE_LSHIFT) {
						menu = TRUE;
					}
				}

				else if (event.type == SDL_KEYUP) {

					if (event.key.keysym.scancode == SDL_SCANCODE_LEFT) {
						left = FALSE;
					} else if (event.key.keysym.scancode == SDL_SCANCODE_RIGHT) {
						right = FALSE;
					} else if (event.key.keysym.scancode == SDL_SCANCODE_UP) {
						up = FALSE;
					} else if (event.key.keysym.scancode == SDL_SCANCODE_DOWN) {
						down = FALSE;
					} else if (event.key.keysym.scancode == SDL_SCANCODE_Z) {
						trigger_1 = FALSE;
					} else if (event.key.keysym.scancode == SDL_SCANCODE_X) {
						trigger_2 = FALSE;
					}
				}
			}
		}

		process(up, down, left, right, trigger_1, trigger_2, menu);
		menu = FALSE; // automatically goes back after one frame

		SDL_GL_SwapWindow(window);
		SDL_Delay(1000 / 30);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	// free everything
	on_terminate();

	SDL_DestroyWindow(window);
	SDL_GL_DeleteContext(context);
	SDL_Quit();

	return 0;
}