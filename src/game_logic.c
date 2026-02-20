#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define MAP_W 4
#define MAP_H 4

#define PLAYER_RADIUS 0.2

static const tile map[MAP_W][MAP_H] = {
	{ TILE_FLOOR,  TILE_LAVA,  TILE_LAVA, TILE_FLOOR },
	{ TILE_FLOOR, TILE_EMPTY,  TILE_LAVA, TILE_FLOOR },
	{ TILE_FLOOR, TILE_FLOOR, TILE_FLOOR, TILE_FLOOR },
	{ TILE_EMPTY, TILE_FLOOR, TILE_FLOOR, TILE_EMPTY }
};

Transform camera;

Mesh *mesh1;
Transform mesh1_transform;

Mesh *mesh2;
Transform mesh2_transform;

Mesh *sky;

Mesh *sprite;
Transform sprite_transform;

int move_time = 0;

void on_start() {

	mesh1 = import_mesh("res/miku.obj", "res/miku.ppm");
	mesh2 = make_map_mesh("res/block.ppm", &map[0][0], MAP_W, MAP_H);
	sky = make_sky_mesh("res/sky.ppm");
	sprite = make_sprite_mesh("res/gun.ppm"); //make_text_sprite_mesh("HELLO WORLD\nNEW LINE", "res/font.ppm", 6, 7);

	mesh1_transform.yaw = 3.14 * -0.2;

	sprite_transform.x = 136;
	sprite_transform.y = -20;

	camera.y = 0.5;
}

void on_terminate() {

	free_mesh(sky);
	free_mesh(mesh1);
	free_mesh(mesh2);
	free_mesh(sprite);
}

void process(bool up, bool down, bool left, bool right, bool action_1, bool action_2, bool paused) {

	if (!paused) {

		// boundary collision
		if (camera.x < PLAYER_RADIUS - 0.5)
			camera.x = PLAYER_RADIUS - 0.5;
	
		if (camera.x > MAP_W - 0.5 - PLAYER_RADIUS)
			camera.x = MAP_W - 0.5 - PLAYER_RADIUS;

		if (camera.z < PLAYER_RADIUS - 0.5)
			camera.z = PLAYER_RADIUS - 0.5;
	
		if (camera.z > MAP_H - 0.5 - PLAYER_RADIUS)
			camera.z = MAP_H - 0.5 - PLAYER_RADIUS;

		// sink down when on a lava tile
		if (camera.x > -0.5 && camera.x < MAP_W - 0.5 && camera.z > -0.5 && camera.z < MAP_H - 0.5)
			camera.y = map[(int) floor(camera.z + 0.5)][(int) floor(camera.x + 0.5)] == TILE_LAVA ? 0.3 : 0.5;

		// movement
		if (left)
			camera.yaw -= 0.15;
		if (right)
			camera.yaw += 0.15;

		if (up) {

			camera.x += 0.1 * sin(camera.yaw);
			camera.z -= 0.1 * cos(camera.yaw);
		}
		if (down) {
			
			camera.x -= 0.1 * sin(camera.yaw);
			camera.z += 0.1 * cos(camera.yaw);
		}

		// move bob animation
		if (left || right || up || down) {

			move_time++;
			sprite_transform.x = 136 + sin(move_time * 0.2) * 3;
			sprite_transform.y = -20 + sin(move_time * 0.4) * 3;
		}

		if (action_1) {
			mesh1_transform.yaw += 0.1;
		} else if (action_2) {
			mesh1_transform.yaw -= 0.1;
		}
	}

	draw_mesh(&camera, NULL, sky);
	draw_mesh(&camera, &mesh1_transform, mesh1);
	draw_mesh(&camera, &mesh2_transform, mesh2);
	draw_mesh(&camera, &sprite_transform, sprite);
}