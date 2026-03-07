#define ITEM_PICKUP_DISTANCE 0.5
#define MAX_ENTITIES 10

typedef enum {

	UNUSED,
	ITEM_HEALTH,
	PROJ_FIREBALL

	// enemies, props, items, and the level end trigger will be entities

} EntityType;

typedef struct {

	EntityType type; // determines the singleton model, AABB collider size, and maybe collidability
	Transform transform;
	// maybe add lifetime int for animation

} Entity;

Entity entities[MAX_ENTITIES];

static Mesh *item_health_mesh;
static Mesh *proj_fireball_mesh;

void init_entity_types() {

	item_health_mesh = import_mesh("res/health.obj", "res/health.ppm");
	proj_fireball_mesh = import_mesh("res/fireball.obj", "res/fireball.ppm");
}

// adds an entity to the registry and returns it in case you need to modify it
Entity *add_entity(float x, float z, EntityType type) {

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
		return NULL;
	}

	entity->type = type;
	entity->transform.x = x;
	entity->transform.z = z;

	switch (type) {

		case UNUSED: break;

		case ITEM_HEALTH:
			entity->transform.y = 0.2;
			entity->transform.pitch = 0.2;
			entity->transform.yaw = 0.0;
			break;
		
		case PROJ_FIREBALL:
			entity->transform.y = 0.5;
			entity->transform.pitch = 0.0;
			entity->transform.yaw = 0.0;
			break;
	}

	return entity;
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
			
			case PROJ_FIREBALL:
				
				// move forward
				entities[i].transform.x += sin(entities[i].transform.yaw) * 0.15;
				entities[i].transform.z -= cos(entities[i].transform.yaw) * 0.15;

				entities[i].transform.y -= sin(entities[i].transform.pitch) * 0.06;
				entities[i].transform.pitch += 0.07;

				// free upon collide
				if (aabb_collides_with_map(&entities[i].transform, 0.1) || entities[i].transform.y < 0.0)
					entities[i].type = UNUSED;

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

			case PROJ_FIREBALL:
				draw_mesh(camera, &entities[i].transform, proj_fireball_mesh);
				break;
		}
	}
}

void free_entities() {

	free(item_health_mesh);
}