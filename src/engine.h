
#ifndef ENGINE_DEFINED

#define ENGINE_DEFINED

#define TRUE 1
#define FALSE 0

typedef struct {

	float x;
	float y;
	float z;
	float pitch;
	// no one needs roll
	float yaw;

} Transform;

#endif

// need to include all engine-side implemented stuff so the game-side can correctly call those upon linkage
Transform *get_mesh_transform(void *mesh);

// game-side implemented
char *get_title();

void on_start();

void on_terminate();

void process_tick();

void process_event(SDL_Event event);