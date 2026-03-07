#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// TODO add better error reporting for functions like import_mesh

#define TILE_AT(x, z) (map[(int) floor((z) + 0.5)][(int) floor((x) + 0.5)])

#define PLAYER_RADIUS 0.2
#define PLAYER_MOVE_SPEED 0.1

#define MAP_W 4
#define MAP_H 8

static const tile map[MAP_H][MAP_W] = {
	{ TILE_FLOOR,	TILE_LAVA,		TILE_LAVA,		TILE_FLOOR },
	{ TILE_FLOOR,	TILE_EMPTY,		TILE_LAVA,		TILE_FLOOR },
	{ TILE_FLOOR,	TILE_FLOOR,		TILE_FLOOR,		TILE_FLOOR },
	{ TILE_EMPTY,	TILE_FLOOR,		TILE_FLOOR,		TILE_EMPTY },
	{ TILE_EMPTY,	TILE_OUTSIDE,	TILE_OUTSIDE,	TILE_EMPTY },
	{ TILE_OUTSIDE,	TILE_EMPTY,		TILE_OUTSIDE,	TILE_OUTSIDE },
	{ TILE_OUTSIDE,	TILE_EMPTY,		TILE_EMPTY,		TILE_OUTSIDE },
	{ TILE_OUTSIDE,	TILE_OUTSIDE,	TILE_OUTSIDE,	TILE_OUTSIDE },
};

int aabb_collides_with_map(Transform *transform, float radius) {

	// boundary collision
	if (transform->x < radius - 0.5)
		return 1;

	if (transform->x > MAP_W - 0.5 - radius)
		return 1;

	if (transform->z < radius - 0.5)
		return 1;

	if (transform->z > MAP_H - 0.5 - radius)
		return 1;

	// map collision (radius is made smaller to prevent weird float behaviour when converting to map-space (int))
	if (
		TILE_AT(transform->x + radius * 0.95, transform->z + radius * 0.95) == TILE_EMPTY ||
		TILE_AT(transform->x - radius * 0.95, transform->z + radius * 0.95) == TILE_EMPTY ||
		TILE_AT(transform->x + radius * 0.95, transform->z - radius * 0.95) == TILE_EMPTY ||
		TILE_AT(transform->x - radius * 0.95, transform->z - radius * 0.95) == TILE_EMPTY) {
		
		return 1;
	}

	return 0;
}

#include "entity.c"

Transform transform_zero;
Transform camera;

int fire_cooldown;

Mesh *mesh_map;
Mesh *sky;

Transform transform_hand;
Mesh *sprite_hand_idle;
Mesh *sprite_hand_fire;

Transform transform_paused;
Mesh *sprite_paused;

int move_time = 0;

void on_start() {

	mesh_map = make_map_mesh("res/tiles.ppm", &map[0][0], MAP_W, MAP_H);
	sky = make_sky_mesh("res/sky.ppm");
	sprite_hand_idle = make_sprite_mesh("res/hand_idle.ppm");
	sprite_hand_fire = make_sprite_mesh("res/hand_fire.ppm");
	sprite_paused = make_text_sprite_mesh("GAME PAUSED", "res/font.ppm", 6, 7);

	init_entity_types();
	add_entity(0, 5, ITEM_HEALTH);
	add_entity(3, 0, ITEM_HEALTH);

	transform_paused.x = (SCREEN_W - (6 * strlen("GAME PAUSED"))) / 2;
	transform_paused.y = SCREEN_H / 2;

	transform_hand.x = SCREEN_W / 2 - 64;
	transform_hand.y = -20;

	camera.y = 0.5;
}

void on_terminate() {

	free_entities();

	free_mesh(sky);
	free_mesh(mesh_map);
	free_mesh(sprite_hand_idle);
	free_mesh(sprite_paused);
}

void move_player_x(float dist) {

	camera.x += dist;

	// boundary collision
	if (camera.x < PLAYER_RADIUS - 0.5)
		camera.x = PLAYER_RADIUS - 0.5;

	if (camera.x > MAP_W - 0.5 - PLAYER_RADIUS)
		camera.x = MAP_W - 0.5 - PLAYER_RADIUS;

	// map collision (PLAYER_RADIUS is made smaller to prevent weird float behaviour when converting to map-space (int))
	if (
		TILE_AT(camera.x + copysign(PLAYER_RADIUS * 0.95, dist), camera.z + PLAYER_RADIUS * 0.95) == TILE_EMPTY ||
		TILE_AT(camera.x + copysign(PLAYER_RADIUS * 0.95, dist), camera.z - PLAYER_RADIUS * 0.95) == TILE_EMPTY) {
		
		// place player right up against wall
		camera.x = dist > 0 ? (floor(camera.x) + 0.5 - PLAYER_RADIUS) : (ceil(camera.x) - 0.5 + PLAYER_RADIUS);
	}
}

void move_player_z(float dist) {

	camera.z += dist;

	// boundary collision
	if (camera.z < PLAYER_RADIUS - 0.5)
		camera.z = PLAYER_RADIUS - 0.5;

	if (camera.z > MAP_H - 0.5 - PLAYER_RADIUS)
		camera.z = MAP_H - 0.5 - PLAYER_RADIUS;

	// map collision
	if (
		TILE_AT(camera.x + PLAYER_RADIUS * 0.95, camera.z + copysign(PLAYER_RADIUS * 0.95, dist)) == TILE_EMPTY ||
		TILE_AT(camera.x - PLAYER_RADIUS * 0.95, camera.z + copysign(PLAYER_RADIUS * 0.95, dist)) == TILE_EMPTY) {
		
		// place player right up against wall
		camera.z = dist > 0 ? (floor(camera.z) + 0.5 - PLAYER_RADIUS) : (ceil(camera.z) - 0.5 + PLAYER_RADIUS);
	}
}

void process(bool up, bool down, bool left, bool right, bool action_1, bool action_2, bool paused) {

	if (!paused) {

		// movement
		if (left)
			camera.yaw -= 0.15;
		if (right)
			camera.yaw += 0.15;

		if (up) {

			move_player_x( PLAYER_MOVE_SPEED * sin(camera.yaw));
			move_player_z(-PLAYER_MOVE_SPEED * cos(camera.yaw));
		}
		if (down) {
			
			move_player_x(-PLAYER_MOVE_SPEED * sin(camera.yaw));
			move_player_z( PLAYER_MOVE_SPEED * cos(camera.yaw));
		}

		// sink down when on a lava tile
		if (camera.x > -0.5 && camera.x < MAP_W - 0.5 && camera.z > -0.5 && camera.z < MAP_H - 0.5)
			camera.y = TILE_AT(camera.x, camera.z) == TILE_LAVA ? 0.3 : 0.5;

		// move bob animation
		if (left || right || up || down) {

			move_time++;
			transform_hand.x = SCREEN_W / 2 - 64 + sin(move_time * 0.2) * 3;
			transform_hand.y = -20 + sin(move_time * 0.4) * 3;
		}

		// shooting
		if (action_1 && fire_cooldown <= 0) {

			fire_cooldown = 5;
			
			Entity *fireball = add_entity(camera.x + sin(camera.yaw) * PLAYER_MOVE_SPEED, camera.z - cos(camera.yaw) * PLAYER_MOVE_SPEED, PROJ_FIREBALL);

			if (fireball) {

				fireball->transform.y = camera.y - 0.1;
				fireball->transform.yaw = camera.yaw;
			}

		} else {

			fire_cooldown--;
		}

		process_entities(&camera);
	}

	// draw world meshes
	draw_mesh(&camera, NULL, sky);
	draw_mesh(&camera, &transform_zero, mesh_map);
	draw_entities(&camera);

	// draw UI elements
	draw_mesh(&camera, &transform_hand, action_1 ? sprite_hand_fire : sprite_hand_idle);

	if (paused)
		draw_mesh(&camera, &transform_paused, sprite_paused);
}