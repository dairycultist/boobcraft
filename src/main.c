#define GLEW_STATIC
#include "../GLEW/glew.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include "app.c"
#include "3D.c"

Mesh *mesh;

int left  = FALSE;
int right = FALSE;
int up    = FALSE;
int down  = FALSE;

void on_start() {
	
	glClearColor(0.2f, 0.2f, 0.23f, 1.0f);

	GLuint shader_program = load_shader_program("res/shaded.vert", "res/shaded.frag");
	mesh = import_mesh("res/test.obj", "res/test.ppm", shader_program);

	mesh->transform.z = -2.0;
}

void on_terminate() {

	free(mesh);
}

void process_tick() {

	if (left) {
		mesh->transform.yaw += 0.1;
	} else if (right) {
		mesh->transform.yaw -= 0.1;
	}

	if (up) {
		mesh->transform.pitch += 0.1;
	} else if (down) {
		mesh->transform.pitch -= 0.1;
	}

	draw_mesh(mesh);
}

void process_event(SDL_Event event) {

	if (event.type == SDL_KEYDOWN && event.key.repeat == 0) {

		if (event.key.keysym.scancode == SDL_SCANCODE_A) {
			left = TRUE;
		} else if (event.key.keysym.scancode == SDL_SCANCODE_D) {
			right = TRUE;
		} else if (event.key.keysym.scancode == SDL_SCANCODE_W) {
			up = TRUE;
		} else if (event.key.keysym.scancode == SDL_SCANCODE_S) {
			down = TRUE;
		}
	}

	else if (event.type == SDL_KEYUP) {

		if (event.key.keysym.scancode == SDL_SCANCODE_A) {
			left = FALSE;
		} else if (event.key.keysym.scancode == SDL_SCANCODE_D) {
			right = FALSE;
		} else if (event.key.keysym.scancode == SDL_SCANCODE_W) {
			up = FALSE;
		} else if (event.key.keysym.scancode == SDL_SCANCODE_S) {
			down = FALSE;
		}
	}
}

int main() {

    return app("Boobcraft", 800, 400, on_start, on_terminate, process_tick, process_event);
}