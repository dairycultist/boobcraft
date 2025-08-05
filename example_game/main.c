// I'm tempted to fabricate my own preprocessor directives for telling the compiler what resources to compile as part of the program

#include <stdio.h>
#include <stdlib.h>
#include "../engine/engine.h"

Transform *camera;
void *mesh1;
void *mesh2;
void *sky;

char *get_title() {

	return "Example Game";
}

void on_start() {

	set_skybox_color(0.2f, 0.2f, 0.23f);

	camera = calloc(sizeof(Transform), 1);
	camera->z = 5;

	mesh1 = import_mesh("example_game/res/miku.obj", "example_game/res/miku.ppm");
	mesh2 = import_mesh("example_game/res/block.obj", "example_game/res/block.ppm");
	sky = make_sky_mesh("example_game/res/sky.ppm");

	get_mesh_transform(mesh1)->yaw = 3.14 * -0.2;

	get_mesh_transform(mesh2)->y = -2.3;
}

void on_terminate() {

	free(camera);
	free_mesh(mesh1);
	free_mesh(mesh2);
	free_mesh(sky);
}

void process(bool up, bool down, bool left, bool right, bool action_1, bool action_2, bool menu) {

	if (menu) {
		printf("Menu!\n");
	}

	if (left) {
		// get_mesh_transform(mesh1)->x -= 0.1;
		camera->yaw -= 0.05;
	} else if (right) {
		// get_mesh_transform(mesh1)->x += 0.1;
		camera->yaw += 0.05;
	}

	if (up) {
		// get_mesh_transform(mesh1)->z -= 0.1;
		camera->pitch -= 0.05;
	} else if (down) {
		// get_mesh_transform(mesh1)->z += 0.1;
		camera->pitch += 0.05;
	}

	if (action_1) {
		get_mesh_transform(mesh1)->yaw += 0.1;
	} else if (action_2) {
		get_mesh_transform(mesh1)->yaw -= 0.1;
	}

	draw_mesh(camera, sky);
	draw_mesh(camera, mesh1);
	draw_mesh(camera, mesh2);
}