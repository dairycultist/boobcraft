#define GLEW_STATIC
#include "GLEW/glew.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include "util.c"

#define PROGRAM_NAME "Boobcraft"

Mesh *mesh;

void process_event(SDL_Event event) {

}

void process() {
	
	draw_mesh(mesh);
}

int main() {

	printf("Starting %s\n", PROGRAM_NAME);

	App *app = init_app(PROGRAM_NAME); // must be first!

	glClearColor(0.0f, 0.5f, 0.0f, 1.0f);

	GLuint shader_program = load_shader_program("vertex.glsl", "fragment.glsl");
	mesh = load_obj_as_mesh(shader_program);

	run_app(app, process, process_event);

	free_app(app);
	free(mesh);

    return 0;
}