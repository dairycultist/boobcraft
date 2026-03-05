#define ITEM_PICKUP_DISTANCE 0.5
#define MAX_ITEMS 5

typedef enum {

	ITEM_UNUSED,
	ITEM_HEALTH

} ItemType;

typedef struct {

	ItemType type;
	Mesh *mesh;
	Transform transform;

} Item; // TODO turn into Entities, which have a type (which determines the singleton model, AABB collider size, and maybe collidability)
// enemies, props, items, and the level end trigger will be entities

Item items[MAX_ITEMS];

// adds an item to the registry
void add_item(int x, int z, ItemType type) {

	// find an empty slot
	Item *item = NULL;

	for (int i = 0; i < MAX_ITEMS; i++) {

		if (items[i].type == ITEM_UNUSED) {

			item = items + i;
			break;
		}
	}

	if (!item) {

		printf("Failed to add an item.\n");
		return;
	}

	// TODO switch statement for ItemType
	item->mesh = import_mesh("res/health.obj", "res/health.ppm");

	item->type = type;

	item->transform.x = (float) x;
	item->transform.z = (float) z;

	item->transform.y = 0.2;
	item->transform.pitch = 0.2;
	item->transform.yaw = 0.0;
}

// process all items in the registry
void process_items(Transform *camera) {

	for (int i = 0; i < MAX_ITEMS; i++) {

		if (items[i].type != ITEM_UNUSED) {

			items[i].transform.yaw += 0.1;

			// player collision detection (King's distance)
			if (items[i].mesh && fmax(fabs(items[i].transform.x - camera->x), fabs(items[i].transform.z - camera->z)) < ITEM_PICKUP_DISTANCE) {

				// apply item's effect

				// free item
				free_mesh(items[i].mesh);
				items[i].type = ITEM_UNUSED;
				items[i].mesh = NULL;
			}
		}
	}
}

void draw_items(Transform *camera) {

	for (int i = 0; i < MAX_ITEMS; i++) {

		if (items[i].type != ITEM_UNUSED)
			draw_mesh(camera, &items[i].transform, items[i].mesh);
	}
}

void free_items() {

	for (int i = 0; i < MAX_ITEMS; i++) {

		if (items[i].type != ITEM_UNUSED)
			free_mesh(items[i].mesh);
	}
}