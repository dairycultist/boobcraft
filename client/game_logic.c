#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define PLAYER_MOVE_SPEED 0.1

Transform transform_zero;
Transform camera;

Mesh *sky;

Transform transform_test_sprite;
Mesh *test_sprite;

Transform transform_miku;
Mesh *miku;

void on_start() {

	sky = make_sky_mesh("client/sky.ppm");

	transform_test_sprite.x = 0.0;
	transform_test_sprite.y = 0.0;
	test_sprite = make_sprite_mesh("client/miku.ppm", 0.3);

	transform_miku.z = -0.5;
	miku = import_mesh("client/miku.obj", "client/miku.ppm", TRUE);

	camera.y = 0.5;
}

void on_terminate() {

	free_mesh(sky);
	free_mesh(test_sprite);
}

void process(bool up, bool down, bool left, bool right, bool action_1, bool action_2, bool paused) {

	// movement
	if (left)
		camera.yaw -= 0.15;
	if (right)
		camera.yaw += 0.15;

	if (up) {

		camera.x += PLAYER_MOVE_SPEED * sin(camera.yaw);
		camera.z -= PLAYER_MOVE_SPEED * cos(camera.yaw);
	}
	if (down) {
		
		camera.x -= PLAYER_MOVE_SPEED * sin(camera.yaw);
		camera.z += PLAYER_MOVE_SPEED * cos(camera.yaw);
	}

	// draw
	draw_mesh(&camera, NULL, sky);
	draw_mesh(&camera, &transform_miku, miku);
	draw_mesh(&camera, &transform_test_sprite, test_sprite);
}