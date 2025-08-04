// I'm tempted to fabricate my own preprocessor directives for telling the compiler what resources to compile as part of the program

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "engine.h"

Transform *camera;
void *mesh1;
void *mesh2;
void *sky;

char *get_title() {

	return "Boobcraft";
}

void on_start() {

	camera = calloc(sizeof(Transform), 1);

	mesh1 = import_mesh("res/miku.obj", "res/miku.ppm", MESH_SHADED);
	mesh2 = import_mesh("res/block.obj", "res/block.ppm", MESH_SHADED);
	sky = import_mesh("res/sky.obj", "res/sky.ppm", MESH_SKY);

	get_mesh_transform(mesh1)->z = -2.0;
	get_mesh_transform(mesh1)->yaw = M_PI * -0.2;

	get_mesh_transform(mesh2)->z = -2.0;
	get_mesh_transform(mesh2)->y = -2.3;
}

void on_terminate() {

	free(camera);
	free(mesh1);
	free(mesh2);
	free(sky);
}

void process(bool up, bool down, bool left, bool right, bool trigger_1, bool trigger_2, bool menu) {

	if (menu) {
		printf("Menu!\n");
	}

	if (left) {
		camera->z -= sin(camera->yaw) * 0.1;
		camera->x -= cos(camera->yaw) * 0.1;
	} else if (right) {
		camera->z += sin(camera->yaw) * 0.1;
		camera->x += cos(camera->yaw) * 0.1;
	}

	if (up) {
		camera->z -= cos(camera->yaw) * 0.1;
		camera->x += sin(camera->yaw) * 0.1;
	} else if (down) {
		camera->z += cos(camera->yaw) * 0.1;
		camera->x -= sin(camera->yaw) * 0.1;
	}

	if (trigger_1) {
		camera->yaw -= 0.1;
	} else if (trigger_2) {
		camera->yaw += 0.1;
	}

	draw_mesh(camera, sky);
	draw_mesh(camera, mesh1);
	draw_mesh(camera, mesh2);
}