#include <GL/glew.h>
#include <SDL2/SDL.h>

typedef char bool;
#define TRUE 1
#define FALSE 0

typedef unsigned char tile;
#define TILE_EMPTY 0
#define TILE_WALL 1
// lava, door, etc

typedef struct {

	float x;
	float y;
	float z;
	float pitch;
	float yaw;

} Transform;

typedef enum {

  MESH_SHADED,
  MESH_SKY,
  MESH_UI // 2D

} MeshType;

// generic struct for all types of mesh (2D, 3D, sky, etc)
typedef struct {

	MeshType type;

	GLuint vertex_array; // "VAO"
	uint vertex_count;
	GLuint texture;

} Mesh;

// static uint rng_state = 1; // uint32_t? time(NULL)?

// // stole this from nash so I don't have to use rand(). it's deterministic!
// uint random_uint(uint bound) {
	
//     rng_state ^= rng_state << 13;
//     rng_state ^= rng_state >> 17;
//     rng_state ^= rng_state << 5;
//     return rng_state % bound;
// }

typedef struct {

	char data[65536 * 64];
	int byte_count;

} EZArray;

void append_ezarray(EZArray *array, void *data, int data_length) {

	memcpy(&array->data[array->byte_count], data, data_length);
	array->byte_count += data_length;
}

#include "render.c"
#include "game_logic.c"

int main() {

	printf("Starting game\n");

	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		fprintf(stderr, "Could not initialize SDL: %s\n", SDL_GetError());
		return 1;
	}

	// init OpenGL
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

	// create the window
	SDL_Window *window = SDL_CreateWindow("Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 400, 240, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

	if (!window) {
		fprintf(stderr, "Could not create window: %s\n", SDL_GetError());
		return 1;
    }

	SDL_GLContext context = SDL_GL_CreateContext(window);

	glewExperimental = GL_TRUE;
	glewInit();

	SDL_SetRelativeMouseMode(SDL_TRUE); // lock mouse

	initialize_shaders();
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	
	on_start();

	// process events until window is closed
	SDL_Event event;
	bool running = TRUE;

	bool up       = FALSE;
	bool down     = FALSE;
	bool left     = FALSE;
	bool right    = FALSE;
	bool action_1 = FALSE;
	bool action_2 = FALSE;

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
						action_1 = TRUE;
					} else if (event.key.keysym.scancode == SDL_SCANCODE_X) {
						action_2 = TRUE;
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
						action_1 = FALSE;
					} else if (event.key.keysym.scancode == SDL_SCANCODE_X) {
						action_2 = FALSE;
					}
				}
			}
		}

		process(up, down, left, right, action_1, action_2, !SDL_GetRelativeMouseMode());

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