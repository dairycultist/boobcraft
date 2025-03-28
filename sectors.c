typedef struct {
	float *corners_x;
    float *corners_y;
	GLuint vbo; // vertex buffer object
	// later we'll have floor and ceiling height but not rn
} Sector;

void remesh_sector(const Sector *const sector) {

    // construct vertices of the sector based on its corners, floor, and ceiling
	float vertices[] = {
		0,     -0.5f, -2.0f,
		0.5f,  -0.5f, 2.0f,
		-0.5f, -0.5f, 2.0f
	};

	// make this sector's buffer the active object
	glBindBuffer(GL_ARRAY_BUFFER, sector->vbo);

	// copy vertex data to active buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
}

Sector *init_sector(float corners_x[], float corners_y[], int corners_bytes) {

    Sector *sector = malloc(sizeof(Sector));
	glGenBuffers(1, &(sector->vbo)); // returns index to new buffer

    sector->corners_x = malloc(corners_bytes);
    sector->corners_y = malloc(corners_bytes);

    memcpy(sector->corners_x, corners_x, corners_bytes);
    memcpy(sector->corners_y, corners_y, corners_bytes);

    // after initializing the sector object, we need to push
    // vertex data about the sector to the vbo it references
	remesh_sector(sector);

	return sector;
}

void redraw_sector(const Sector *const sector, const Vec3 player_pos, const float player_rot, const GLuint shader_program) {

	GLint gl_player_pos = glGetUniformLocation(shader_program, "player_pos");
	GLint gl_player_rot = glGetUniformLocation(shader_program, "player_rot");

	glUniform3f(gl_player_pos, player_pos.x, player_pos.y, player_pos.z);
	glUniform1f(gl_player_rot, player_rot);

	// make this sector's buffer the active object
	glBindBuffer(GL_ARRAY_BUFFER, sector->vbo);

	// draw active object
	glDrawArrays(GL_TRIANGLES, 0, 3);
}

void free_sector(Sector *sector) {

    free(sector->corners_x);
    free(sector->corners_y);
    free(sector);
}