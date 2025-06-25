#define GLEW_STATIC
#include "GLEW/glew.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include "util.c"

Mesh *mesh;

void on_start() {
	
	glClearColor(0.2f, 0.2f, 0.23f, 1.0f);

	GLuint shader_program = load_shader_program("vertex.glsl", "fragment.glsl");
	mesh = load_obj_as_mesh("cat.obj", shader_program);
}

void on_terminate() {

	free(mesh);
}

void process() {
	
	draw_mesh(mesh);
}

void process_event(SDL_Event event) {

}

int main() {

	app("Boobcraft", on_start, on_terminate, process, process_event);

    return 0;
}