#include "util.c"

typedef struct {

	float x;
	float y;
	float z;
	float pitch;
	// no one needs roll
	float yaw;

} Transform;

typedef struct {

	Transform transform;

	GLuint vertex_array; // "VAO"
	uint vertex_count;
	GLuint shader_program; // not stored by the VAO so have to include separately
	GLuint texture;

	// maybe also store the vertex_buffer ("VBO") if it needs to be manipulated

} Mesh;

// returns -1 on error
GLuint load_shader(const char* path, GLenum shader_type) {

	FILE *file = fopen(path, "r");
	if (file == NULL) {
		return -1; // file IO error
	}
  
	// determine file size
	fseek(file, 0, SEEK_END);
	long file_size = ftell(file);
	fseek(file, 0, SEEK_SET);
  
	// allocate memory for the string (+1 for null terminator)
	char *shadercode = (char *) malloc(file_size + 1);
	if (shadercode == NULL) {
		fclose(file);
		return -1; // memory allocation error
	}
	
	// read file content
	size_t bytes_read = fread(shadercode, 1, file_size, file);
	if (bytes_read != file_size) {
		fclose(file);
		free(shadercode);
		return -1; // read error or incomplete read
	}
	
	shadercode[file_size] = '\0';
  
	fclose(file);

	// use shadercode to create shader
	const char *const_shadercode = shadercode;

	GLuint shader = glCreateShader(shader_type);
	glShaderSource(shader, 1, &const_shadercode, NULL);
	glCompileShader(shader);

    // check for compilation log
    char compile_log[512];
    glGetShaderInfoLog(shader, 512, NULL, compile_log);

    if (compile_log[0] != '\0') {
        printf("'%s' provided a compilation log: %s\n", path, compile_log);
    }

    // check if it compiled successfully
    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

    if (status != GL_TRUE) {
        log_error("A shader failed to compile");
		return -1;
    }

	return shader;
}

GLuint load_shader_program(const char *vertex_path, const char *fragment_path) {
	
	GLuint shader_program = glCreateProgram();
	glAttachShader(shader_program, load_shader(vertex_path, GL_VERTEX_SHADER)); // does not catch errors load_shader makes lol
	glAttachShader(shader_program, load_shader(fragment_path, GL_FRAGMENT_SHADER));
	glLinkProgram(shader_program); // apply changes to shader program, not gonna call "glUseProgram" yet bc not drawing

	// TODO you should stop using glGetAttribLocation/glGetUniformLocation
	// https://stackoverflow.com/questions/15639957/glgetattriblocation-returns-1-when-retrieving-existing-shader-attribute

	return shader_program;
}

void load_ppm(GLenum target, const char *ppm_path) {

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

	fread(pixels, 3, width * height, file);
	fclose(file);

	glTexImage2D(target, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);
}

// returns NULL on error
Mesh *import_mesh(const char *obj_path, const char *ppm_path, const GLuint shader_program) {

	// read obj file
	FILE *file = fopen(obj_path, "r");

	if (file == NULL) {
		return NULL;
	}

	char line[1024];

	EZArray position_data;
	EZArray normal_data;
	EZArray texture_data;
	EZArray composite_data; // stores combined vertex position, normal, and texture data

	int vertex_count;

	while (fgets(line, 1024, file)) {

		char prefix[8];

		sscanf(line, "%s", prefix);

		if (!strcmp(prefix, "v")) {

			float v[3];
			
			sscanf(line, "v %f %f %f", &v[0], &v[1], &v[2]);

			append_ezarray(&position_data, v, sizeof(float) * 3);
		}

		else if (!strcmp(prefix, "vn")) {

			float n[3];
			
			sscanf(line, "vn %f %f %f", &n[0], &n[1], &n[2]);

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

	// make vertex array
	GLuint vertex_array;
	glGenVertexArrays(1, &vertex_array);
	glBindVertexArray(vertex_array);

	// make vertex buffer (stored by vertex_array)
	GLuint vertexBuffer;
	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);													// make it the active buffer
	glBufferData(GL_ARRAY_BUFFER, composite_data.bytecount, composite_data.data, GL_STATIC_DRAW);	// copy vertex data into the active buffer

	// link active vertex data and shader attributes
	GLint pos_attrib = glGetAttribLocation(shader_program, "position");
	glVertexAttribPointer(pos_attrib, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, 0);
	glEnableVertexAttribArray(pos_attrib); // requires a VAO to be bound

	GLint normal_attrib = glGetAttribLocation(shader_program, "normal");
	glVertexAttribPointer(normal_attrib, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (GLvoid *) (sizeof(float) * 3));
	glEnableVertexAttribArray(normal_attrib);

	GLint uv_attrib = glGetAttribLocation(shader_program, "UV");
	glVertexAttribPointer(uv_attrib, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (GLvoid *) (sizeof(float) * 6));
	glEnableVertexAttribArray(uv_attrib);

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
	load_ppm(GL_TEXTURE_2D, ppm_path);

	// create final mesh object to return
	Mesh *mesh = malloc(sizeof(Mesh));
	mesh->transform.x 		= 0.0f;
	mesh->transform.y 		= 0.0f;
	mesh->transform.z 		= 0.0f;
	mesh->transform.pitch 	= 0.0f;
	mesh->transform.yaw 	= 0.0f;
	mesh->vertex_array = vertex_array;
	mesh->vertex_count = vertex_count;
	mesh->shader_program = shader_program;
	mesh->texture = texture;

	return mesh;
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

void draw_mesh(const Mesh *mesh) {

	// rotation matrices (used later)
	GLfloat pitch_matrix[4][4] = {
		{ 1, 0,          				  0,       					  0 },
		{ 0, cos(mesh->transform.pitch), -sin(mesh->transform.pitch), 0 },
		{ 0, sin(mesh->transform.pitch),  cos(mesh->transform.pitch), 0 },
		{ 0, 0,           				  0,         				  1 }
	};

	GLfloat yaw_matrix[4][4] = {
		{  cos(mesh->transform.yaw), 0, sin(mesh->transform.yaw), 0 },
		{  0,       				 1, 0,      				  0 },
		{ -sin(mesh->transform.yaw), 0, cos(mesh->transform.yaw), 0 },
		{  0,         				 0, 0,       				  1 }
	};

	// model matrix (converts from model space to world space)
	GLfloat model_matrix[4][4];

	mat4_mult(yaw_matrix, pitch_matrix, model_matrix);

	model_matrix[3][0] = -mesh->transform.x;
	model_matrix[3][1] = -mesh->transform.y;
	model_matrix[3][2] =  mesh->transform.z;

	// view matrix (converts from world space to view space, aka accounts for camera transformations)
	GLfloat view_matrix[4][4] = {
		{1, 0, 0, 0},
		{0, 1, 0, 0},
		{0, 0, 1, 0},
		{0, 0, 0, 1}
	};

	// perspective projection matrix (converts from view space to clip space)
	const float fovY = 90;
	const float aspectRatio = 2.0;
	const float front = 0.01; // near plane
	const float back = 10;    // far plane

	float tangent = tan(fovY / 2 * DEG2RAD); // tangent of half fovY
	float top = front * tangent;             // half height of near plane
	float right = top * aspectRatio;         // half width of near plane

	GLfloat proj_matrix[4][4] = {
		{ front / right, 0, 		   0, 									   0   },
		{ 0, 			 front / top,  0, 									   0   },
		{ 0, 			 0, 		  -(back + front) / (back - front), 	  -1.0 },
		{ 0, 			 0, 		  -(2.0 * back * front) / (back - front),  0   }
	};

	// final position matrix (proj_matrix * view_matrix * model_matrix)
	GLfloat position_matrix[4][4];

	mat4_mult(proj_matrix, view_matrix, position_matrix);
	mat4_mult(position_matrix, model_matrix, position_matrix);

	// normal matrix, accounts for rotation (inverse(yaw_matrix) * inverse(pitch_matrix))
	GLfloat normal_matrix[4][4];

	pitch_matrix[2][1] *= -1;
	pitch_matrix[1][2] *= -1;
	yaw_matrix[2][0] *= -1;
	yaw_matrix[0][2] *= -1;

	mat4_mult(yaw_matrix, pitch_matrix, normal_matrix);

	// load in the matrices we just calculated as uniforms
	glUniformMatrix4fv(glGetUniformLocation(mesh->shader_program, "position_matrix"), 1, GL_FALSE, &position_matrix[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(mesh->shader_program, "normal_matrix"), 1, GL_FALSE, &normal_matrix[0][0]);

	// bind the mesh, its shader, and its texture
	glBindVertexArray(mesh->vertex_array);
	glUseProgram(mesh->shader_program);
	glBindTexture(GL_TEXTURE_2D, mesh->texture);

	// draw
	glDrawArrays(GL_TRIANGLES, 0, mesh->vertex_count);
}