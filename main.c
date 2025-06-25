#define GLEW_STATIC
#include "GLEW/glew.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include "util.c"

#define TRUE 1
#define FALSE 0
#define PROGRAM_NAME "Boobcraft"

int main() {

	printf("Starting %s\n", PROGRAM_NAME);

	App *app = init_app(PROGRAM_NAME); // must be first!

	GLuint shader_program = load_shader_program("vertex.glsl", "fragment.glsl");
	Mesh *mesh = load_obj_as_mesh(shader_program);

	// process events until window is closed
	SDL_Event event;
	int running = TRUE;

	glClearColor(0.0f, 0.5f, 0.0f, 1.0f);

	while (running) {

		while (SDL_PollEvent(&event)) {

			if (event.type == SDL_QUIT) {
				running = FALSE;
			}
		}

		draw_mesh(mesh);

		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // idk if I need this
		SDL_GL_SwapWindow(app->window);
	}

	free_app(app);
	free(mesh);

    return 0;
}