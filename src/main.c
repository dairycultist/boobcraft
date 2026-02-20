#include <GL/glew.h>
#include <SDL2/SDL.h>

typedef char bool;
#define TRUE 1
#define FALSE 0

typedef struct {

	float x;
	float y;
	float z;
	float pitch;
	float yaw;

} Transform;

/*
 * engine-side implemented
 * game-side will reference these headers upon linkage
 */
void set_skybox_color(float r, float g, float b);

void *import_mesh(const char *obj_path, const char *ppm_path); // eventually will be replaced by make_generic_mesh or something, taking raw data instead of a filepath
void *make_sky_mesh(const char *ppm_path);
void *make_sprite_mesh(const char *ppm_path);
void *make_text_sprite_mesh(const char *text, const char *ppm_path, const int glyph_width, const int glyph_height);

void draw_mesh(const Transform *camera, const Transform *mesh_transform, const void *void_mesh);

void free_mesh(void *void_mesh);

/*
 * game-side implemented
 */
void on_start();
void on_terminate();
void process(bool up, bool down, bool left, bool right, bool action_1, bool action_2, bool menu);

#include "util.c"
#include "game_logic.c"
#include "render.c"

int main() {

	printf("Starting game");

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
	SDL_Window *window = SDL_CreateWindow("Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 400, 240, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

	if (!window) {
        log_error("Could not create window");
		return 1;
    }

	SDL_GLContext context = SDL_GL_CreateContext(window);

	glewExperimental = GL_TRUE;
	glewInit();

	// lock mouse
	SDL_SetRelativeMouseMode(SDL_TRUE);

	// initialize shaders
	initialize_shaders();
	
	// let programmer initialize stuff
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
	bool menu     = FALSE;

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
						action_1 = FALSE;
					} else if (event.key.keysym.scancode == SDL_SCANCODE_X) {
						action_2 = FALSE;
					}
				}
			}
		}

		process(up, down, left, right, action_1, action_2, menu);
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