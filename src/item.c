
typedef struct {

	Mesh *mesh;
	Transform transform;
	// TODO add type enum

} Item;

void init_item(Item *item, int x, int z) { // TODO add type enum

	item->mesh = import_mesh("res/health.obj", "res/health.ppm");

	item->transform.x = (float) x;
	item->transform.z = (float) z;

	item->transform.y = 0.2;
	item->transform.pitch = 0.2;
}

void process_item(Item *item) {

	item->transform.yaw += 0.1;

	// TODO player collision detection, in which case apply item effect
}