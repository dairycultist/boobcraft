
typedef struct {

	Mesh *mesh;
	Transform transform;
	// TODO add type enum

} Item;

Item items;

// adds an item to the registry
void add_item(int x, int z) { // TODO add type enum

	items.mesh = import_mesh("res/health.obj", "res/health.ppm");

	items.transform.x = (float) x;
	items.transform.z = (float) z;

	items.transform.y = 0.2;
	items.transform.pitch = 0.2;
}

// process all items in the registry
void process_items() {

	items.transform.yaw += 0.1;

	// TODO player collision detection, in which case apply items effect
}

void draw_items(Transform *camera) {

	draw_mesh(camera, &items.transform, items.mesh);
}

void free_items() {

	free_mesh(items.mesh);
}