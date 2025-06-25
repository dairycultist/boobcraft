#define GLEW_STATIC
#include "GLEW/glew.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include "util.c"

Mesh *mesh;

void process() {
	
	draw_mesh(mesh);
}

void process_event(SDL_Event event) {

}

int main() {

	App *app = init_app("Boobcraft"); // must be first!

	glClearColor(0.2f, 0.2f, 0.23f, 1.0f);

	GLuint shader_program = load_shader_program("vertex.glsl", "fragment.glsl");
	mesh = load_obj_as_mesh("cat.obj", shader_program);

	run_app(app, process, process_event);

	free_app(app);
	free(mesh);

    return 0;
}