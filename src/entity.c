#define ITEM_PICKUP_DISTANCE 0.5
#define MAX_ENTITIES 5

typedef enum {

	UNUSED,
	ITEM_HEALTH

} EntityType;

typedef struct {

	EntityType type; // determines the singleton model, AABB collider size, and maybe collidability
	Mesh *mesh;
	Transform transform;

} Entity; // enemies, props, items, and the level end trigger will be entities

Entity entities[MAX_ENTITIES];

// adds an entity to the registry
void add_entity(int x, int z, EntityType type) {

	// find an empty slot
	Entity *entity = NULL;

	for (int i = 0; i < MAX_ENTITIES; i++) {

		if (entities[i].type == UNUSED) {

			entity = entities + i;
			break;
		}
	}

	if (!entity) {

		printf("Failed to add an entity.\n");
		return;
	}

	// TODO switch statement for EntityType
	entity->mesh = import_mesh("res/health.obj", "res/health.ppm");

	entity->type = type;

	entity->transform.x = (float) x;
	entity->transform.z = (float) z;

	entity->transform.y = 0.2;
	entity->transform.pitch = 0.2;
	entity->transform.yaw = 0.0;
}

// process all entities in the registry
void process_entities(Transform *camera) {

	for (int i = 0; i < MAX_ENTITIES; i++) {

		if (entities[i].type != UNUSED) {

			// TODO switch statement for EntityType
			// item spin + collision
			entities[i].transform.yaw += 0.1;

			// player collision detection (King's distance)
			if (entities[i].mesh && fmax(fabs(entities[i].transform.x - camera->x), fabs(entities[i].transform.z - camera->z)) < ITEM_PICKUP_DISTANCE) {

				// apply item's effect

				// free item
				free_mesh(entities[i].mesh);
				entities[i].type = UNUSED;
				entities[i].mesh = NULL;
			}
		}
	}
}

void draw_entities(Transform *camera) {

	for (int i = 0; i < MAX_ENTITIES; i++) {

		if (entities[i].type != UNUSED)
			draw_mesh(camera, &entities[i].transform, entities[i].mesh);
	}
}

void free_entities() {

	for (int i = 0; i < MAX_ENTITIES; i++) {

		if (entities[i].type != UNUSED)
			free_mesh(entities[i].mesh);
	}
}