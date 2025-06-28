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
	uint index_count;
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

	EZArray vertex_data;

	GLuint index_data[65536];
	int index_i = 0;

	while (fgets(line, 1024, file)) {

		char prefix[8];

		sscanf(line, "%s", prefix);

		if (!strcmp(prefix, "v")) {

			float v1, v2, v3;
			
			sscanf(line, "v %f %f %f", &v1, &v2, &v3);

			append_ezarray(vertex_data, v1, sizeof(float));
			append_ezarray(vertex_data, v2, sizeof(float));
			append_ezarray(vertex_data, v3, sizeof(float));
		}

		else if (!strcmp(prefix, "f")) {

			// only works with tris right now (no quads or ngons)

			GLuint i1, i2, i3; // vertex indices
			GLuint t1, t2, t3; // vertex texture coordinate indices
			GLuint n1, n2, n3; // vertex normal indices
			
			sscanf(line, "f %d/%d/%d %d/%d/%d %d/%d/%d", &i1, &t1, &n1, &i2, &t2, &n2, &i3, &t3, &n3);

			index_data[index_i]   = i1 - 1; // starts at 1 for some reason
			index_data[index_i+1] = i2 - 1;
			index_data[index_i+2] = i3 - 1;

			index_i += 3;
		}
	}

	// make vertex array
	GLuint vertex_array;
	glGenVertexArrays(1, &vertex_array);
	glBindVertexArray(vertex_array);

	// make vertex buffer (stored by vertex_array)
	GLuint vertexBuffer;
	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);											// make it the active buffer
	glBufferData(GL_ARRAY_BUFFER, vertex_data->bytecount, vertex_data->data, GL_STATIC_DRAW);	// copy vertex data into the active buffer
	
	// make element buffer (stored by vertex_array)
	GLuint elementBuffer;
	glGenBuffers(1, &elementBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_i * sizeof(GLuint), index_data, GL_STATIC_DRAW);

	// link active vertex data and shader attributes
	GLint posAttrib = glGetAttribLocation(shader_program, "position");
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(posAttrib); // requires a VAO to be bound

	Mesh *mesh = malloc(sizeof(Mesh));
	mesh->transform.x 		= 0.0f;
	mesh->transform.y 		= 0.0f;
	mesh->transform.z 		= 0.0f;
	mesh->transform.pitch 	= 0.0f;
	mesh->transform.yaw 	= 0.0f;
	mesh->vertex_array = vertex_array;
	mesh->index_count = index_i;
	mesh->shader_program = shader_program;

	return mesh;
}

void draw_mesh(const Mesh *mesh) {

	glUniform3f(glGetUniformLocation(mesh->shader_program, "translation"), mesh->transform.x, mesh->transform.y, mesh->transform.z);
	glUniform1f(glGetUniformLocation(mesh->shader_program, "pitch"), mesh->transform.pitch);
	glUniform1f(glGetUniformLocation(mesh->shader_program, "yaw"), mesh->transform.yaw);

	glBindVertexArray(mesh->vertex_array);
	glUseProgram(mesh->shader_program);

	glDrawElements(GL_TRIANGLES, mesh->index_count, GL_UNSIGNED_INT, 0);
}