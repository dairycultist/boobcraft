#include <stdio.h>
#include <stdlib.h>
#include <math.h>

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

	camera.z = 5;

	mesh1 = import_mesh("res/miku.obj", "res/miku.ppm");
	mesh2 = import_mesh("res/block.obj", "res/block.ppm");
	sky = make_sky_mesh("res/sky.ppm");
	sprite = make_sprite_mesh("res/gun.ppm"); //make_text_sprite_mesh("HELLO WORLD\nNEW LINE", "res/font.ppm", 6, 7);

	mesh1_transform.yaw = 3.14 * -0.2;

	mesh2_transform.y = -2.3;

	sprite_transform.x = 400 / 2 - 64;
	sprite_transform.y = -20;
}

void on_terminate() {

	free_mesh(sky);
	free_mesh(mesh1);
	free_mesh(mesh2);
	free_mesh(sprite);
}

void process(bool up, bool down, bool left, bool right, bool action_1, bool action_2, bool paused) {

	if (paused) {
		return;
	}

	if (left) {
		camera.yaw -= 0.15;
	}
	if (right) {
		camera.yaw += 0.15;
	}

	if (up) {
		camera.x += 0.2 * sin(camera.yaw);
		camera.z -= 0.2 * cos(camera.yaw);
	}
	if (down) {
		camera.x -= 0.2 * sin(camera.yaw);
		camera.z += 0.2 * cos(camera.yaw);
	}

	if (left || right || up || down) {
		move_time++;
		sprite_transform.y = -20 + sin(move_time * 0.4) * 3;
	}

	if (action_1) {
		mesh1_transform.yaw += 0.1;
	} else if (action_2) {
		mesh1_transform.yaw -= 0.1;
	}

	draw_mesh(&camera, NULL, sky);
	draw_mesh(&camera, &mesh1_transform, mesh1);
	draw_mesh(&camera, &mesh2_transform, mesh2);
	draw_mesh(&camera, &sprite_transform, sprite);
}