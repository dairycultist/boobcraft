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

	// TODO store texture info (GLuint)
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

// returns NULL on error
Mesh *load_obj_as_mesh(const char *path, const GLuint shader_program) {

	FILE *file = fopen(path, "r");

	if (file == NULL) {
		return NULL;
	}

	char line[1024];

	EZArray position_data;
	EZArray normal_data;
	// EZArray texture_data;
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

		// else if (!strcmp(prefix, "vt")) {

		// }

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
			append_ezarray(&composite_data, normal_data.data + ((n[0] - 1) * sizeof(float) * 3), sizeof(float) * 3);

			append_ezarray(&composite_data, position_data.data + ((p[1] - 1) * sizeof(float) * 3), sizeof(float) * 3);
			append_ezarray(&composite_data, normal_data.data + ((n[1] - 1) * sizeof(float) * 3), sizeof(float) * 3);

			append_ezarray(&composite_data, position_data.data + ((p[2] - 1) * sizeof(float) * 3), sizeof(float) * 3);
			append_ezarray(&composite_data, normal_data.data + ((n[2] - 1) * sizeof(float) * 3), sizeof(float) * 3);
			
			vertex_count += 3;
		}
	}

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
	GLint posAttrib = glGetAttribLocation(shader_program, "position");
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, 0);
	glEnableVertexAttribArray(posAttrib); // requires a VAO to be bound

	GLint normal_attrib = glGetAttribLocation(shader_program, "normal");
	glVertexAttribPointer(normal_attrib, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (GLvoid *) (sizeof(float) * 3));
	glEnableVertexAttribArray(normal_attrib);

	Mesh *mesh = malloc(sizeof(Mesh));
	mesh->transform.x 		= 0.0f;
	mesh->transform.y 		= 0.0f;
	mesh->transform.z 		= 0.0f;
	mesh->transform.pitch 	= 0.0f;
	mesh->transform.yaw 	= 0.0f;
	mesh->vertex_array = vertex_array;
	mesh->vertex_count = vertex_count;
	mesh->shader_program = shader_program;

	return mesh;
}

void draw_mesh(const Mesh *mesh) {

	// mesh->transform.x, mesh->transform.y, mesh->transform.z
	// mesh->transform.pitch
	// mesh->transform.yaw

	// gotta construct matrices
	GLfloat matrix[4][4] = {
		{1, 0, 0, 0},
		{0, 1, 0, 0},
		{0, 0, 1, 0},
		{0, 0, 0, 1}
	};

	glUniformMatrix4fv(glGetUniformLocation(mesh->shader_program, "position_matrix"), 1, GL_FALSE, &matrix[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(mesh->shader_program, "normal_matrix"), 1, GL_FALSE, &matrix[0][0]);

	glBindVertexArray(mesh->vertex_array);
	glUseProgram(mesh->shader_program);

	glDrawArrays(GL_TRIANGLES, 0, mesh->vertex_count);
}



// mat4 model_matrix;   // to world space
// mat4 view_matrix;    // to view space (aka account for camera transformations)
// mat4 proj_matrix;    // to clip space (projection)



// view_matrix[0][0] = 1.;
// view_matrix[1][1] = 1.;
// view_matrix[2][2] = 1.;
// view_matrix[3][3] = 1.;



// mat4 rot_pitch = mat4(
// 	1, 0,           0,          0,
// 	0, cos(pitch), -sin(pitch), 0,
// 	0, sin(pitch),  cos(pitch), 0,
// 	0, 0,           0,          1
// );
// mat4 rot_yaw = mat4(
// 		cos(yaw), 0, sin(yaw), 0,
// 		0,        1, 0,        0,
// 	-sin(yaw), 0, cos(yaw), 0,
// 	0,         0, 0,        1
// );



// normal_matrix = inverse(rot_pitch * rot_yaw);

// model_matrix = rot_yaw * rot_pitch;
// model_matrix[3][0] = -translation.x;
// model_matrix[3][1] = -translation.y;
// model_matrix[3][2] = translation.z;



// // construct perspective projection matrix
// float fovY = 90;
// float aspectRatio = 2.0;
// float front = 0.01; // near plane
// float back = 10;    // far plane

// const float DEG2RAD = acos(-1.0f) / 180;

// float tangent = tan(fovY/2 * DEG2RAD);    // tangent of half fovY
// float top = front * tangent;              // half height of near plane
// float right = top * aspectRatio;          // half width of near plane

// proj_matrix[0][0] =  front / right;
// proj_matrix[1][1] =  front / top;
// proj_matrix[2][2] = -(back + front) / (back - front);
// proj_matrix[2][3] = -1;
// proj_matrix[3][2] = -(2 * back * front) / (back - front);
// proj_matrix[3][3] =  0;



// position_matrix = proj_matrix * view_matrix * model_matrix;