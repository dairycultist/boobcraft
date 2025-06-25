#define GLEW_STATIC
#include "GLEW/glew.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include "util.c"

Mesh *mesh;

void on_start() {
	
	glClearColor(0.2f, 0.2f, 0.23f, 1.0f);

	GLuint shader_program = load_shader_program("vertex.glsl", "fragment.glsl");
	mesh = load_obj_as_mesh("test.obj", shader_program);
}

void on_terminate() {

	free(mesh);
}

void process_tick() {
	
	draw_mesh(mesh);
}

void process_event(SDL_Event event) {

	if (event.type == SDL_KEYDOWN && event.key.repeat == 0) {
		printf("pressed %d\n", event.key.keysym.scancode);
	}

	else if (event.type == SDL_KEYUP) {
		printf("lifted %d\n", event.key.keysym.scancode);
	}
}

int main() {

    return app("Boobcraft", 800, 400, on_start, on_terminate, process_tick, process_event);
}