
#ifndef ENGINE_DEFINED

#define ENGINE_DEFINED

typedef char bool;
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

/*
 * engine-side implemented
 * game-side will reference these headers upon linkage
 */
void set_skybox_color(float r, float g, float b);

void *import_mesh(const char *obj_path, const char *ppm_path); // eventually will be replaced by make_generic_mesh or something
void *make_sky_mesh(const char *ppm_path);
void *make_sprite_mesh(const char *ppm_path);

void draw_mesh(const Transform *camera, const void *void_mesh);
Transform *get_mesh_transform(void *mesh);

void free_mesh(void *void_mesh);

/*
 * game-side implemented
 */
char *get_title();
void on_start();
void on_terminate();
void process(bool up, bool down, bool left, bool right, bool action_1, bool action_2, bool menu);