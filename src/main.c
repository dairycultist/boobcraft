#include <GL/glew.h>
#include <SDL2/SDL.h>

typedef char bool;
#define TRUE 1
#define FALSE 0

typedef unsigned char tile;
#define TILE_EMPTY 0
#define TILE_FLOOR 1
#define TILE_LAVA 2
#define TILE_OUTSIDE 3

#define SCREEN_W 400
#define SCREEN_H 240

typedef struct {

	float x;
	float y;
	float z;
	float pitch;
	float yaw;

} Transform;

typedef enum {

  MESH_SHADED,
  MESH_UNSHADED,
  MESH_SKY, // does not take a transform matrix
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

	char *data;
	int alloc_size;
	int byte_count;

} EZArray;

void append_ezarray(EZArray *array, void *data, int data_length) {

	// initialize EZArray if necessary
	if (!array->data) {
		
		array->data = malloc(65536);
		array->alloc_size = 65536;
	}

	// grow size of EZArray allocation if necessary
	while (array->alloc_size < array->byte_count + data_length) {

		array->data = realloc(array->data, array->alloc_size *= 2);
	}

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
	SDL_Window *window = SDL_CreateWindow("Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_W, SCREEN_H, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

	if (!window) {
		fprintf(stderr, "Could not create window: %s\n", SDL_GetError());
		return 1;
    }

	SDL_GLContext context = SDL_GL_CreateContext(window);

	glewExperimental = GL_TRUE;
	glewInit();

	SDL_SetRelativeMouseMode(SDL_TRUE); // lock mouse

	initialize_shaders();
	glFrontFace(GL_CCW);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	
	on_start();

	// create frame buffer for original-resolution mode (as opposed to native-resolution mode)
	// involves first creating two texures, one for color and one for depth, and then attaching to that frame buffer
	GLuint fbo_color;
	glGenTextures(1, &fbo_color);
	glBindTexture(GL_TEXTURE_2D, fbo_color);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCREEN_W, SCREEN_H, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	GLuint fbo_depth;
	glGenTextures(1, &fbo_depth);
	glBindTexture(GL_TEXTURE_2D, fbo_depth);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SCREEN_W, SCREEN_H, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glBindTexture(GL_TEXTURE_2D, 0);

	GLuint fbo;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbo_color, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, fbo_depth, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// ...and a mesh for that frame buffer
	const float data[] = {
		0, 0, 1, 0, 0,
		0, 2, 1, 0, 1,
		2, 2, 1, 1, 1,
		0, 0, 1, 0, 0,
		2, 2, 1, 1, 1,
		2, 0, 1, 1, 0,
	};
	
	Mesh *fbo_mesh = mesh_builder((const float *) data, sizeof(float) * 5 * 6, 6, "res/sky.ppm", MESH_UI); // TODO refactor mesh builder to take in a texture GLuint, not a PPM path
	fbo_mesh->texture = fbo_color;

	// process events until window is closed
	SDL_Event event;
	bool running = TRUE;
	bool use_original_res = TRUE;

	bool up       = FALSE;
	bool down     = FALSE;
	bool left     = FALSE;
	bool right    = FALSE;
	bool action_1 = FALSE;
	bool action_2 = FALSE;

	int window_w = SCREEN_W, window_h = SCREEN_H, window_x = 0, window_y = 0;

	while (running) {

		while (SDL_PollEvent(&event)) {

			if (event.type == SDL_QUIT) {

				running = FALSE;

			} else if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED) {

				if (SCREEN_W / (float) SCREEN_H > event.window.data1 / (float) event.window.data2) {

					window_w = event.window.data1;
					window_h = SCREEN_H * event.window.data1 / SCREEN_W;

					window_x = 0;
					window_y = (event.window.data2 - window_h) / 2;

				} else {

					window_w = SCREEN_W * event.window.data2 / SCREEN_H;
					window_h = event.window.data2;

					window_x = (event.window.data1 - window_w) / 2;
					window_y = 0;
				}

				glViewport(window_x, window_y, window_w, window_h);
			
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
					} else if (event.key.keysym.scancode == SDL_SCANCODE_O) {
						use_original_res = !use_original_res;
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

		if (use_original_res) {

			// switch to rendering to framebuffer
			glBindFramebuffer(GL_FRAMEBUFFER, fbo);
			glViewport(0, 0, SCREEN_W, SCREEN_H);

			// clear that mf
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}

		process(up, down, left, right, action_1, action_2, !SDL_GetRelativeMouseMode());

		if (use_original_res) {

			// switch to rendering to screen
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glViewport(window_x, window_y, window_w, window_h);

			// render framebuffer to screen
			draw_mesh(NULL, &transform_zero, fbo_mesh);
		}

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