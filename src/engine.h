
#ifndef ENGINE_DEFINED

#define ENGINE_DEFINED

#define TRUE 1
#define FALSE 0

typedef enum {

  MESH_SHADED,
  MESH_SKY

} MeshShader;

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
void *import_mesh(const char *obj_path, const char *ppm_path, const MeshShader shader);
void draw_mesh(const Transform *camera, const void *mesh);

Transform *get_mesh_transform(void *mesh);

// game-side implemented
char *get_title();

void on_start();

void on_terminate();

void process(int up, int down, int left, int right, int trigger_1, int trigger_2);