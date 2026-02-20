#include <stdio.h>
#include <stdlib.h>

Transform camera;

void *mesh1;
Transform mesh1_transform;

void *mesh2;
Transform mesh2_transform;

void *sky;

void *sprite;
Transform sprite_transform;

void on_start() {

	set_skybox_color(0.2f, 0.2f, 0.23f);

	camera.z = 5;

	mesh1 = import_mesh("res/miku.obj", "res/miku.ppm");
	mesh2 = import_mesh("res/block.obj", "res/block.ppm");
	sky = make_sky_mesh("res/sky.ppm");
	sprite = make_text_sprite_mesh("HELLO WORLD\nNEW LINE", "res/font.ppm", 6, 7);

	mesh1_transform.yaw = 3.14 * -0.2;

	mesh2_transform.y = -2.3;

	sprite_transform.x = 60;
	sprite_transform.y = 60;
}

void on_terminate() {

	free_mesh(sky);
	free_mesh(mesh1);
	free_mesh(mesh2);
	free_mesh(sprite);
}

void process(bool up, bool down, bool left, bool right, bool action_1, bool action_2, bool menu) {

	if (menu) {
		printf("Menu!\n");
	}

	if (left) {
		camera.yaw -= 0.05;
	} else if (right) {
		camera.yaw += 0.05;
	}

	if (up) {
		camera.pitch -= 0.05;
	} else if (down) {
		camera.pitch += 0.05;
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