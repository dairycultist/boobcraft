// this file handles both 2D and 3D mesh data creation, rendering, manipulation, and destruction

// shader programs
static GLuint sp_shaded;
static GLuint sp_unshaded;

// perspective projection matrix (converts from view space to clip space)
// hardcoded with FOV=100 aspect=1.666 near=0.01 far=100
// https://www.songho.ca/opengl/gl_projectionmatrix.html#fov
static GLfloat proj_matrix[4][4] = {
	{0.503661243, 0, 0, 0},
	{0, 0.839099631, 0, 0},
	{0, 0, -1.0, -1.0},
	{0, 0, -0.02, 0},
};

static GLuint load_shader(const char *shadercode, GLenum shader_type) {

	GLuint shader = glCreateShader(shader_type);
	glShaderSource(shader, 1, &shadercode, NULL);
	glCompileShader(shader);

	return shader;
}

static GLuint load_shader_program(const char *vertex_shadercode, const char *fragment_shadercode) {
	
	GLuint shader_program = glCreateProgram();
	glAttachShader(shader_program, load_shader(vertex_shadercode, GL_VERTEX_SHADER)); // does not catch errors load_shader makes lol
	glAttachShader(shader_program, load_shader(fragment_shadercode, GL_FRAGMENT_SHADER));
	glLinkProgram(shader_program); // apply changes to shader program, not gonna call "glUseProgram" yet bc not drawing

	// IF you want to have arbitrary shaders allowed, you should stop using glGetAttribLocation/glGetUniformLocation, but I don't so I won't
	// https://stackoverflow.com/questions/15639957/glgetattriblocation-returns-1-when-retrieving-existing-shader-attribute

	return shader_program;
}

static void get_ppm_resolution(const char *ppm_path, int *width, int *height) {

	FILE *file = fopen(ppm_path, "r");

	char line[1024];

	// read header
	fgets(line, 1024, file);
	fgets(line, 1024, file);
	sscanf(line, "%d %d", width, height);

	fclose(file);
}

static void import_ppm(GLenum target, const char *ppm_path) {

	// by default, OpenGL reads texture data with a 4-byte row alignment: https://stackoverflow.com/questions/72177553/why-is-gl-unpack-alignment-default-4
	// it's more efficient, but means this function cannot properly read images whose dimensions aren't a multiple of 4 correctly (fix is simple tho)

	int width, height;

	FILE *file = fopen(ppm_path, "r");

	// read header
	{
		char line[1024];

		fgets(line, 1024, file); // not gonna verify header because I'm lazy and just wanna get this working right now
		fgets(line, 1024, file);
		sscanf(line, "%d %d", &width, &height);
		fgets(line, 1024, file);
	}

	unsigned char *pixels = malloc(width * height * 3);

	// ppms store pixels starting from the top left, but opengl wants them starting from the bottom left, so you need to flip the "layers"
	int i;

	for (i = height - 1; i >= 0; i--)
		fread(pixels + i * width * 3, 3, width, file);

	fclose(file);

	// write texture data to target buffer
	glTexImage2D(target, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);
}

static Mesh *mesh_builder(const float data[], const int byte_count, const int vertex_count, const char *ppm_path, MeshType type) {

	// make vertex array
	GLuint vertex_array;
	glGenVertexArrays(1, &vertex_array);
	glBindVertexArray(vertex_array);

	// make vertex buffer (stored by vertex_array)
	GLuint vertexBuffer;
	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);					// make it the active buffer
	glBufferData(GL_ARRAY_BUFFER, byte_count, data, GL_STATIC_DRAW);// copy vertex data into the active buffer

	// link active vertex data and shader attributes (for MESH_SHADED)
	if (type == MESH_SHADED) {

		GLint pos_attrib = glGetAttribLocation(sp_shaded, "position");
		glVertexAttribPointer(pos_attrib, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, 0);
		glEnableVertexAttribArray(pos_attrib); // requires a VAO to be bound

		GLint normal_attrib = glGetAttribLocation(sp_shaded, "normal");
		glVertexAttribPointer(normal_attrib, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (GLvoid *) (sizeof(float) * 3));
		glEnableVertexAttribArray(normal_attrib);

		GLint uv_attrib = glGetAttribLocation(sp_shaded, "UV");
		glVertexAttribPointer(uv_attrib, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (GLvoid *) (sizeof(float) * 6));
		glEnableVertexAttribArray(uv_attrib);

	} else if (type == MESH_SKY || type == MESH_UI) {

		GLint pos_attrib = glGetAttribLocation(sp_unshaded, "position");
		glVertexAttribPointer(pos_attrib, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, 0);
		glEnableVertexAttribArray(pos_attrib);

		GLint uv_attrib = glGetAttribLocation(sp_unshaded, "UV");
		glVertexAttribPointer(uv_attrib, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (GLvoid *) (sizeof(float) * 3));
		glEnableVertexAttribArray(uv_attrib);

	}

	// debind vertex array
	glBindVertexArray(0);

	// create texture object
	GLuint texture;
	glGenTextures(1, &texture);

	// bind texture (to active texture 2D)
	glBindTexture(GL_TEXTURE_2D, texture);

	// wrap repeat
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// filter linear
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// write texture data
	import_ppm(GL_TEXTURE_2D, ppm_path);

	// create final mesh object to return
	Mesh *mesh = malloc(sizeof(Mesh));
	mesh->vertex_array = vertex_array;
	mesh->vertex_count = vertex_count;
	mesh->type = type;
	mesh->texture = texture;

	return mesh;
}

// returns NULL on error
Mesh *import_mesh(const char *obj_path, const char *ppm_path) {

	// read obj file
	FILE *file = fopen(obj_path, "r");

	if (file == NULL) {
		return NULL;
	}

	char line[1024];

	EZArray position_data  = {0};
	EZArray normal_data    = {0};
	EZArray texture_data   = {0};
	EZArray composite_data = {0}; // stores combined vertex position, normal, and texture data

	int vertex_count;

	while (fgets(line, 1024, file)) {

		char prefix[8];

		sscanf(line, "%s", prefix);

		if (!strcmp(prefix, "v")) {

			float v[3];
			
			sscanf(line, "v %f %f %f", &v[0], &v[1], &v[2]);

			// obj vertices have reverse xz
			v[0] = -v[0];
			v[2] = -v[2];

			append_ezarray(&position_data, v, sizeof(float) * 3);
		}

		else if (!strcmp(prefix, "vn")) {

			float n[3];
			
			sscanf(line, "vn %f %f %f", &n[0], &n[1], &n[2]);

			// idk why I have to do this but it fixes lighting so
			n[0] = -n[0];

			append_ezarray(&normal_data, n, sizeof(float) * 3);
		}

		else if (!strcmp(prefix, "vt")) {

			float n[2];
			
			sscanf(line, "vt %f %f", &n[0], &n[1]);

			append_ezarray(&texture_data, n, sizeof(float) * 2);
		}

		else if (!strcmp(prefix, "f")) {

			// only works with tris right now (no quads or ngons)

			GLuint p[3]; // vertex position indices
			GLuint t[3]; // vertex texture coordinate indices
			GLuint n[3]; // vertex normal indices
			
			sscanf(line, "f %u/%u/%u %u/%u/%u %u/%u/%u",
				&p[0], &t[0], &n[0],
				&p[1], &t[1], &n[1],
				&p[2], &t[2], &n[2]);

			// convert vertex indices to vertex positions (indices start at 1 for some reason)
			append_ezarray(&composite_data, position_data.data + ((p[0] - 1) * sizeof(float) * 3), sizeof(float) * 3);
			append_ezarray(&composite_data, normal_data.data +   ((n[0] - 1) * sizeof(float) * 3), sizeof(float) * 3);
			append_ezarray(&composite_data, texture_data.data +  ((t[0] - 1) * sizeof(float) * 2), sizeof(float) * 2);

			append_ezarray(&composite_data, position_data.data + ((p[1] - 1) * sizeof(float) * 3), sizeof(float) * 3);
			append_ezarray(&composite_data, normal_data.data +   ((n[1] - 1) * sizeof(float) * 3), sizeof(float) * 3);
			append_ezarray(&composite_data, texture_data.data +  ((t[1] - 1) * sizeof(float) * 2), sizeof(float) * 2);

			append_ezarray(&composite_data, position_data.data + ((p[2] - 1) * sizeof(float) * 3), sizeof(float) * 3);
			append_ezarray(&composite_data, normal_data.data +   ((n[2] - 1) * sizeof(float) * 3), sizeof(float) * 3);
			append_ezarray(&composite_data, texture_data.data +  ((t[2] - 1) * sizeof(float) * 2), sizeof(float) * 2);
			
			vertex_count += 3;
		}
	}

	fclose(file);

	return mesh_builder((const float *) composite_data.data, composite_data.byte_count, vertex_count, ppm_path, MESH_SHADED);
}

Mesh *make_sky_mesh(const char *ppm_path) {

	// hardcoded inverted cube with sidelength 100 units (aka a cheap skybox)
	const float data[] = {
		// +z
		50, 50, 50, 0.5, 0.75,
		-50, 50, 50, 0.25, 0.75,
		-50, -50, 50, 0.25, 0.5,
		50, 50, 50, 0.5, 0.75,
		-50, -50, 50, 0.25, 0.5,
		50, -50, 50, 0.5, 0.5,
		// -z
		50, 50, -50, 0.75, 0.75,
		-50, -50, -50, 1, 0.5,
		-50, 50, -50, 1, 0.75,
		50, 50, -50, 0.75, 0.75,
		50, -50, -50, 0.75, 0.5,
		-50, -50, -50, 1, 0.5,
		// +x
		50, 50, 50, 0.5, 0.75,
		50, -50, -50, 0.75, 0.5,
		50, 50, -50, 0.75, 0.75,
		50, 50, 50, 0.5, 0.75,
		50, -50, 50, 0.5, 0.5,
		50, -50, -50, 0.75, 0.5,
		// -x
		-50, 50, 50, 0.25, 0.75,
		-50, 50, -50, 0, 0.75,
		-50, -50, -50, 0, 0.5,
		-50, 50, 50, 0.25, 0.75,
		-50, -50, -50, 0, 0.5,
		-50, -50, 50, 0.25, 0.5,
		// +y
		50, 50, 50, 0.5, 0.75,
		-50, 50, -50, 0.25, 1,
		-50, 50, 50, 0.25, 0.75,
		50, 50, 50, 0.5, 0.75,
		50, 50, -50, 0.5, 1,
		-50, 50, -50, 0.25, 1,
		// -y
		50, -50, 50, 0.5, 0.5,
		-50, -50, 50, 0.25, 0.5,
		-50, -50, -50, 0.25, 0.25,
		50, -50, 50, 0.5, 0.5,
		-50, -50, -50, 0.25, 0.25,
		50, -50, -50, 0.5, 0.25
	};

	return mesh_builder((const float *) data, sizeof(float) * 5 * 36, 36, ppm_path, MESH_SKY);
}

Mesh *make_sprite_mesh(const char *ppm_path) {

	// sprite mesh is pixel-perfect - every pixel in the texture lines up with one on the screen
	int width, height;
	get_ppm_resolution(ppm_path, &width, &height);

	float w = width * 2 / 400.;
	float h = height * 2 / 240.;

	// sprite mesh is initialized with bottom left corner in center, and moved to screen bottom left corner by transformations
	const float data[] = {
		0, 0, 1,	0, 0,
		0, h, 1,	0, 1,
		w, h, 1,	1, 1,

		0, 0, 1,	0, 0,
		w, h, 1,	1, 1,
		w, 0, 1,	1, 0,
	};
	
	return mesh_builder((const float *) data, sizeof(float) * 5 * 6, 6, ppm_path, MESH_UI);
}

Mesh *make_text_sprite_mesh(const char *text, const char *ppm_path, const int glyph_width, const int glyph_height) {
	
	EZArray vertices = {0};
	int vertex_count = 0;

	int width, height;
	get_ppm_resolution(ppm_path, &width, &height);

	float w = glyph_width * 2 / 400.;
	float h = glyph_height * 2 / 240.;

	float x = 0, y = 0;

	float uv_w = glyph_width / (float) width;
	float uv_h = glyph_height / (float) height;

	float uv_x, uv_y;

	int i;

	for (i = 0; text[i]; i++) {

		if (text[i] == ' ') {

			x += w;

		} else if (text[i] == '\n') {

			x = 0;
			y -= h;

		} else {

			uv_x = ((int) text[i] - 65) * uv_w;
			uv_y = 0;

			const float data[] = {
				x, 		y, 		1,		uv_x, 			uv_y,
				x, 		y + h, 	1,		uv_x, 			uv_y + uv_h,
				x + w, 	y + h, 	1,		uv_x + uv_w, 	uv_y + uv_h,

				x, 		y, 		1,		uv_x, 			uv_y,
				x + w, 	y + h, 	1,		uv_x + uv_w, 	uv_y + uv_h,
				x + w, 	y, 		1,		uv_x + uv_w, 	uv_y,
			};

			append_ezarray(&vertices, (void *) data, sizeof(float) * 5 * 6);
			vertex_count += 6;

			x += w;
		}
	}

	return mesh_builder((const float *) vertices.data, vertices.byte_count, vertex_count, ppm_path, MESH_UI);
}

// returns number of vertices added
int mesh_wall(EZArray *data, const tile* map, int w, int h, int x, int y, int z, tile to_block) {

	float data_wall[] = {
		// +z
		x + 0.5,	y + 1,	z + 0.5,	0, 0, -1,	1,	1,
		x - 0.5,	y,		z + 0.5,	0, 0, -1,	0,	0,
		x - 0.5,	y + 1,	z + 0.5,	0, 0, -1,	0,	1,
		x + 0.5,	y + 1,	z + 0.5,	0, 0, -1,	1,	1,
		x + 0.5,	y,		z + 0.5,	0, 0, -1,	1,	0,
		x - 0.5,	y,		z + 0.5,	0, 0, -1,	0,	0,
		// -z
		x + 0.5,	y + 1,	z - 0.5,	0, 0, 1,	0,	1,
		x - 0.5,	y + 1,	z - 0.5,	0, 0, 1,	1,	1,
		x - 0.5,	y,		z - 0.5,	0, 0, 1,	1,	0,
		x + 0.5,	y + 1,	z - 0.5,	0, 0, 1,	0,	1,
		x - 0.5,	y,		z - 0.5,	0, 0, 1,	1,	0,
		x + 0.5,	y,		z - 0.5,	0, 0, 1,	0,	0,
		// +x
		x + 0.5,	y + 1,	z + 0.5,	-1, 0, 0,	0,	1,
		x + 0.5,	y + 1,	z - 0.5,	-1, 0, 0,	1,	1,
		x + 0.5,	y,		z - 0.5,	-1, 0, 0,	1,	0,
		x + 0.5,	y + 1,	z + 0.5,	-1, 0, 0,	0,	1,
		x + 0.5,	y,		z - 0.5,	-1, 0, 0,	1,	0,
		x + 0.5,	y,		z + 0.5,	-1, 0, 0,	0,	0,
		// -x
		x - 0.5,	y + 1,	z + 0.5,	1, 0, 0,	1,	1,
		x - 0.5,	y,		z - 0.5,	1, 0, 0,	0,	0,
		x - 0.5,	y + 1,	z - 0.5,	1, 0, 0,	0,	1,
		x - 0.5,	y + 1,	z + 0.5,	1, 0, 0,	1,	1,
		x - 0.5,	y,		z + 0.5,	1, 0, 0,	1,	0,
		x - 0.5,	y,		z - 0.5,	1, 0, 0,	0,	0,
	};

	int vertex_count = 0;

	// check if the tile to_block is in any of the four directions; if so, put a wall there
	if (z == h - 1 || map[x + (z + 1) * w] == to_block) {

		append_ezarray(data, data_wall, sizeof(float) * 8 * 6);
		vertex_count += 6;
	}
	if (z == 0 || map[x + (z - 1) * w] == to_block) {

		append_ezarray(data, data_wall + 8 * 6, sizeof(float) * 8 * 6);
		vertex_count += 6;
	}
	if (x == w - 1 || map[(x + 1) + z * w] == to_block) {

		append_ezarray(data, data_wall + 8 * 6 * 2, sizeof(float) * 8 * 6);
		vertex_count += 6;
	}
	if (x == 0 || map[(x - 1) + z * w] == to_block) {

		append_ezarray(data, data_wall + 8 * 6 * 3, sizeof(float) * 8 * 6);
		vertex_count += 6;
	}

	return vertex_count;
}

Mesh *make_map_mesh(const char *ppm_path, const tile* map, int w, int h) {

	EZArray data = {0};
	int vertex_count = 0;

	for (int z = 0; z < h; z++) {

		for (int x = 0; x < w; x++) {

			switch (map[x + z * w]) {
		
				case TILE_EMPTY:
					break;
				
				case TILE_FLOOR:;

					float data_floor[] = {
						// floor
						x + 0.5, 0, z + 0.5, 0, 0, 0.5, 0, 0,
						x - 0.5, 0, z - 0.5, 0, 0, 0.5, 1, 1,
						x - 0.5, 0, z + 0.5, 0, 0, 0.5, 1, 0,
						x + 0.5, 0, z + 0.5, 0, 0, 0.5, 0, 0,
						x + 0.5, 0, z - 0.5, 0, 0, 0.5, 0, 1,
						x - 0.5, 0, z - 0.5, 0, 0, 0.5, 1, 1,
						// ceiling
						x + 0.5, 1, z + 0.5, 0, 0, 0.5, 0, 0,
						x - 0.5, 1, z + 0.5, 0, 0, 0.5, 1, 0,
						x - 0.5, 1, z - 0.5, 0, 0, 0.5, 1, 1,
						x + 0.5, 1, z + 0.5, 0, 0, 0.5, 0, 0,
						x - 0.5, 1, z - 0.5, 0, 0, 0.5, 1, 1,
						x + 0.5, 1, z - 0.5, 0, 0, 0.5, 0, 1
					};

					append_ezarray(&data, &data_floor, sizeof(float) * 8 * 12);
					vertex_count += 12;

					vertex_count += mesh_wall(&data, map, w, h, x, 0, z, TILE_EMPTY);
					break;

				case TILE_LAVA:;

					float data_lava[] = {
						// floor
						x + 0.5, -0.2, z + 0.5, 0, 0, 0.5, 0, 0,
						x - 0.5, -0.2, z - 0.5, 0, 0, 0.5, 1, 1,
						x - 0.5, -0.2, z + 0.5, 0, 0, 0.5, 1, 0,
						x + 0.5, -0.2, z + 0.5, 0, 0, 0.5, 0, 0,
						x + 0.5, -0.2, z - 0.5, 0, 0, 0.5, 0, 1,
						x - 0.5, -0.2, z - 0.5, 0, 0, 0.5, 1, 1,
						// ceiling
						x + 0.5, 1, z + 0.5, 0, 0, 0.5, 0, 0,
						x - 0.5, 1, z + 0.5, 0, 0, 0.5, 1, 0,
						x - 0.5, 1, z - 0.5, 0, 0, 0.5, 1, 1,
						x + 0.5, 1, z + 0.5, 0, 0, 0.5, 0, 0,
						x - 0.5, 1, z - 0.5, 0, 0, 0.5, 1, 1,
						x + 0.5, 1, z - 0.5, 0, 0, 0.5, 0, 1
					};

					append_ezarray(&data, &data_lava, sizeof(float) * 8 * 12);
					vertex_count += 12;

					vertex_count += mesh_wall(&data, map, w, h, x,  0, z, TILE_EMPTY);
					vertex_count += mesh_wall(&data, map, w, h, x, -1, z, TILE_EMPTY);
					vertex_count += mesh_wall(&data, map, w, h, x, -1, z, TILE_FLOOR);
					break;
			}
		}
	}

	return mesh_builder((const float *) data.data, sizeof(float) * 8 * vertex_count, vertex_count, ppm_path, MESH_SHADED);
}

void mat4_mult(const GLfloat b[4][4], const GLfloat a[4][4], GLfloat out[4][4]) {

	// a (rightmost) is applied first, then b

	GLfloat matrix[4][4] = {
		{
			a[0][0] * b[0][0] + a[0][1] * b[1][0] + a[0][2] * b[2][0] + a[0][3] * b[3][0],
			a[0][0] * b[0][1] + a[0][1] * b[1][1] + a[0][2] * b[2][1] + a[0][3] * b[3][1],
			a[0][0] * b[0][2] + a[0][1] * b[1][2] + a[0][2] * b[2][2] + a[0][3] * b[3][2],
			a[0][0] * b[0][3] + a[0][1] * b[1][3] + a[0][2] * b[2][3] + a[0][3] * b[3][3],
		},
		{
			a[1][0] * b[0][0] + a[1][1] * b[1][0] + a[1][2] * b[2][0] + a[1][3] * b[3][0],
			a[1][0] * b[0][1] + a[1][1] * b[1][1] + a[1][2] * b[2][1] + a[1][3] * b[3][1],
			a[1][0] * b[0][2] + a[1][1] * b[1][2] + a[1][2] * b[2][2] + a[1][3] * b[3][2],
			a[1][0] * b[0][3] + a[1][1] * b[1][3] + a[1][2] * b[2][3] + a[1][3] * b[3][3],
		},
		{
			a[2][0] * b[0][0] + a[2][1] * b[1][0] + a[2][2] * b[2][0] + a[2][3] * b[3][0],
			a[2][0] * b[0][1] + a[2][1] * b[1][1] + a[2][2] * b[2][1] + a[2][3] * b[3][1],
			a[2][0] * b[0][2] + a[2][1] * b[1][2] + a[2][2] * b[2][2] + a[2][3] * b[3][2],
			a[2][0] * b[0][3] + a[2][1] * b[1][3] + a[2][2] * b[2][3] + a[2][3] * b[3][3],
		},
		{
			a[3][0] * b[0][0] + a[3][1] * b[1][0] + a[3][2] * b[2][0] + a[3][3] * b[3][0],
			a[3][0] * b[0][1] + a[3][1] * b[1][1] + a[3][2] * b[2][1] + a[3][3] * b[3][1],
			a[3][0] * b[0][2] + a[3][1] * b[1][2] + a[3][2] * b[2][2] + a[3][3] * b[3][2],
			a[3][0] * b[0][3] + a[3][1] * b[1][3] + a[3][2] * b[2][3] + a[3][3] * b[3][3],
		},
	};

	int x, y;

	for (x = 0; x < 4; x++) {
		for (y = 0; y < 4; y++) {
			out[x][y] = matrix[x][y];
		}
	}
}

void generate_rotation_matrices(GLfloat pitch_matrix[4][4], float pitch, GLfloat yaw_matrix[4][4], float yaw) {

	pitch_matrix[0][0] = 1;
	pitch_matrix[0][1] = 0;
	pitch_matrix[0][2] = 0;
	pitch_matrix[0][3] = 0;

	pitch_matrix[1][0] = 0;
	pitch_matrix[1][1] = cos(pitch);
	pitch_matrix[1][2] = -sin(pitch);
	pitch_matrix[1][3] = 0;

	pitch_matrix[2][0] = 0;
	pitch_matrix[2][1] = sin(pitch);
	pitch_matrix[2][2] = cos(pitch);
	pitch_matrix[2][3] = 0;

	pitch_matrix[3][0] = 0;
	pitch_matrix[3][1] = 0;
	pitch_matrix[3][2] = 0;
	pitch_matrix[3][3] = 1;

	yaw_matrix[0][0] = cos(yaw);
	yaw_matrix[0][1] = 0;
	yaw_matrix[0][2] = sin(yaw);
	yaw_matrix[0][3] = 0;

	yaw_matrix[1][0] = 0;
	yaw_matrix[1][1] = 1;
	yaw_matrix[1][2] = 0;
	yaw_matrix[1][3] = 0;

	yaw_matrix[2][0] = -sin(yaw);
	yaw_matrix[2][1] = 0;
	yaw_matrix[2][2] = cos(yaw);
	yaw_matrix[2][3] = 0;

	yaw_matrix[3][0] = 0;
	yaw_matrix[3][1] = 0;
	yaw_matrix[3][2] = 0;
	yaw_matrix[3][3] = 1;
}

void draw_mesh(const Transform *camera, const Transform *mesh_transform, const Mesh *mesh) {

	// shared buffers
	GLfloat pitch_matrix[4][4];
	GLfloat yaw_matrix[4][4];

	GLfloat position_matrix[4][4];

	// bind the mesh and its texture
	glBindVertexArray(mesh->vertex_array);
	glBindTexture(GL_TEXTURE_2D, mesh->texture);

	// do different stuff depending on mesh type (informing what shaders, matrices, etc to use)
	if (mesh->type == MESH_SHADED) {

		// model matrix (converts from model space to world space)
		generate_rotation_matrices(
			pitch_matrix, mesh_transform->pitch,
			yaw_matrix, mesh_transform->yaw
		);

		GLfloat model_matrix[4][4];

		mat4_mult(yaw_matrix, pitch_matrix, model_matrix); // rotation

		model_matrix[3][0] = mesh_transform->x; // translation
		model_matrix[3][1] = mesh_transform->y;
		model_matrix[3][2] = mesh_transform->z;

		// view matrix (converts from world space to view space, aka accounts for camera transformations)
		// must apply translations before rotations this time, unlike model matrix!
		generate_rotation_matrices(
			pitch_matrix, -camera->pitch,
			yaw_matrix, -camera->yaw
		);

		GLfloat view_matrix[4][4] = {
			{1, 0, 0, 0},
			{0, 1, 0, 0},
			{0, 0, 1, 0},
			{-camera->x, -camera->y, -camera->z, 1}
		};

		mat4_mult(yaw_matrix, view_matrix, view_matrix);
		mat4_mult(pitch_matrix, view_matrix, view_matrix);

		// final position matrix (proj_matrix * view_matrix * model_matrix)
		mat4_mult(proj_matrix, view_matrix, position_matrix);
		mat4_mult(position_matrix, model_matrix, position_matrix);

		// normal matrix (applied to normals to account for model rotation)
		GLfloat normal_matrix[4][4];

		generate_rotation_matrices(
			pitch_matrix, -mesh_transform->pitch,
			yaw_matrix, -mesh_transform->yaw
		);

		mat4_mult(yaw_matrix, pitch_matrix, normal_matrix);

		// load the shader program and the uniforms we just calculated
		glUseProgram(sp_shaded);
		glUniformMatrix4fv(glGetUniformLocation(sp_shaded, "position_matrix"), 1, GL_FALSE, &position_matrix[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(sp_shaded, "normal_matrix"), 1, GL_FALSE, &normal_matrix[0][0]);

	} else if (mesh->type == MESH_SKY) {

		// view matrix (converts from world space to view space, aka accounts for camera transformations)
		// must apply translations before rotations this time, unlike model matrix!
		generate_rotation_matrices(
			pitch_matrix, -camera->pitch,
			yaw_matrix, -camera->yaw
		);

		mat4_mult(pitch_matrix, yaw_matrix, position_matrix);
		mat4_mult(proj_matrix, position_matrix, position_matrix);

		// load the shader program and the uniforms we just calculated
		glUseProgram(sp_unshaded);
		glUniformMatrix4fv(glGetUniformLocation(sp_unshaded, "position_matrix"), 1, GL_FALSE, &position_matrix[0][0]);

	} else if (mesh->type == MESH_UI) {

		memset(position_matrix, 0, sizeof(GLfloat) * 16);
		position_matrix[0][0] = 1;
		position_matrix[1][1] = 1;
		position_matrix[2][2] = 1;
		position_matrix[3][3] = 1;

		position_matrix[3][0] = mesh_transform->x / 200 - 1; // translation (converted from screen [0,400]x[0,240] to UV [-1,1]x[-1,1])
		position_matrix[3][1] = mesh_transform->y / 120 - 1;

		// load the shader program and the uniforms we just calculated
		glUseProgram(sp_unshaded);
		glUniformMatrix4fv(glGetUniformLocation(sp_unshaded, "position_matrix"), 1, GL_FALSE, &position_matrix[0][0]);
	}

	// for sprites ONLY - disable depth buffer testing/writing and backface culling
	if (mesh->type == MESH_UI) {

		glDisable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);

	} else {

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glFrontFace(GL_CCW);
	}

	// draw
	glDrawArrays(GL_TRIANGLES, 0, mesh->vertex_count);
}

// TODO proper OpenGL-compliant freeing of meshes
void free_mesh(Mesh *mesh) {

	free(mesh);
}

void initialize_shaders() {

	// shader programs
	sp_shaded = load_shader_program(
		"#version 150 core\n"
		"uniform mat4 position_matrix;"
		"uniform mat4 normal_matrix;"
		"in vec3 position;"
		"in vec3 normal;"
		"in vec2 UV;"
		"out vec3 normal_camera;"
		"out vec2 frag_UV;"
		"void main() {"
			"gl_Position = position_matrix * vec4(position.xyz, 1.0);" // get final position
			"normal_camera = (normal_matrix * vec4(normal, 1.0)).xyz;" // get final normal
			"frag_UV = UV;" // pass along UV
		"}",
		"#version 150 core\n"
		"uniform sampler2D tex;"
		"in vec3 normal_camera;"
		"in vec2 frag_UV;"
		"out vec4 outColor;"
		"void main() {"
			"float c = dot(normal_camera, vec3(0.7, 0.7, 0)) * 0.5 + 0.5;" // TODO use gl_Position.z to make further fragments darker
			"outColor = texture(tex, frag_UV) * vec4(c, c, c, 1.0);"
		"}"
	);

	sp_unshaded = load_shader_program(
		"#version 150 core\n"
		"uniform mat4 position_matrix;"
		"in vec3 position;"
		"in vec2 UV;"
		"out vec2 frag_UV;"
		"void main() {"
			"gl_Position = position_matrix * vec4(position.xy, -position.z, 1.0);" // get final position
			"frag_UV = UV;" // pass along UV
		"}",
		"#version 150 core\n"
		"uniform sampler2D tex;"
		"in vec2 frag_UV;"
		"out vec4 outColor;"
		"void main() {"
			"outColor = texture(tex, frag_UV);"
			"if (outColor == vec4(0, 0, 0, 1)) { discard; }" // texture clip transparency (discard any fully black pixels in unshaded)
		"}"
	);
}