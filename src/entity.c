#define ITEM_PICKUP_DISTANCE 0.5
#define MAX_ENTITIES 5

typedef enum {

	UNUSED,
	ITEM_HEALTH,
	ENEMY_TEST // TODO use miku model for testing

	// enemies, props, items, and the level end trigger will be entities

} EntityType;

typedef struct {

	EntityType type; // determines the singleton model, AABB collider size, and maybe collidability
	Transform transform;

} Entity;

Entity entities[MAX_ENTITIES];

static Mesh *item_health_mesh;

void init_entity_types() {

	item_health_mesh = import_mesh("res/health.obj", "res/health.ppm");
}

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

		switch (entities[i].type) {

			case UNUSED: break;

			case ITEM_HEALTH:

				// item spin
				entities[i].transform.yaw += 0.1;

				// player collision detection (King's distance)
				if (fmax(fabs(entities[i].transform.x - camera->x), fabs(entities[i].transform.z - camera->z)) < ITEM_PICKUP_DISTANCE) {

					// apply item's effect

					// free item
					entities[i].type = UNUSED;
				}

				break;
		}
	}
}

void draw_entities(Transform *camera) {

	for (int i = 0; i < MAX_ENTITIES; i++) {

		switch (entities[i].type) {

			case UNUSED: break;

			case ITEM_HEALTH:
				draw_mesh(camera, &entities[i].transform, item_health_mesh);
				break;
		}
	}
}

void free_entities() {

	free(item_health_mesh);
}