
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
void *import_mesh(const char *obj_path, const char *ppm_path);
void draw_mesh(const Transform *camera, const void *mesh);
Transform *get_mesh_transform(void *mesh);
void *make_sky_mesh(const char *ppm_path);
void set_skybox_color(float r, float g, float b);

/*
 * game-side implemented
 */
char *get_title();
void on_start();
void on_terminate();
void process(bool up, bool down, bool left, bool right, bool action_1, bool action_2, bool menu);