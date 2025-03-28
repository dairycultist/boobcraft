#include <stdio.h>
#include <stdlib.h>

#define GLEW_STATIC // idk if I need this
#include "lib/glew.h"
#include "lib/glfw3.h"

#define TRUE 1
#define FALSE 0

typedef struct {
	float x;
	float y;
	float z;
} Vec3;

int input_up    = FALSE;
int input_down  = FALSE;
int input_left  = FALSE;
int input_right = FALSE;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {

	if (action == GLFW_PRESS) {

		if (key == GLFW_KEY_W) {
			input_up = TRUE;
		} else if (key == GLFW_KEY_S) {
			input_down = TRUE;
		}else if (key == GLFW_KEY_A) {
			input_left = TRUE;
		} else if (key == GLFW_KEY_D) {
			input_right = TRUE;
		}

	} else if (action == GLFW_RELEASE) {

		if (key == GLFW_KEY_W) {
			input_up = FALSE;
		} else if (key == GLFW_KEY_S) {
			input_down = FALSE;
		}else if (key == GLFW_KEY_A) {
			input_left = FALSE;
		} else if (key == GLFW_KEY_D) {
			input_right = FALSE;
		}

	}
}

void update_mesh(const Vec3 *player_pos, GLuint shader_program) {

	float vertices[] = {
		0,      0.5f, 2.0f,
		0.5f,  -0.5f, 2.0f,
		-0.5f, -0.5f, 2.0f
	};

	GLint gl_player_pos = glGetUniformLocation(shader_program, "player_pos");
	glUniform3f(gl_player_pos, player_pos->x, player_pos->y, player_pos->z);

	// copy vertex data to active buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
}

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

	return shader;
}

int main() {

/************************************
	initialize GLFW, window, and GLEW
	*/

    glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL", NULL, NULL);
	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, key_callback);

	glewExperimental = GL_TRUE;
	glewInit();

	printf("OpenGL version: %s\n", glGetString(GL_VERSION));

/***********************************************************************************
	initialize a VAO (stores all the VERTEX-ATTRIBUTE information declared after it)
	*/

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

/*************************
	initialize VERTEX DATA
	*/
	
	// create buffer (returns index)
	GLuint vbo; // rename to like, map_vbo
	glGenBuffers(1, &vbo);

	// make this buffer the active object
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	// would update mesh here, but we do it in the update loop anyways

/****************************
	initialize shader program
	*/

	// create program
	GLuint shader_program = glCreateProgram();

	// create shaders and link them to program
	glAttachShader(shader_program, load_shader("vertex.glsl", GL_VERTEX_SHADER));
	glAttachShader(shader_program, load_shader("fragment.glsl", GL_FRAGMENT_SHADER));
	glLinkProgram(shader_program); // must be called to 'refresh' attachment

	// use program for future drawing
	glUseProgram(shader_program);

/**************************************************************************************
	link between VERTEX DATA and SHADER ATTRIBUTES (aka registering vertex data format)
	*/

	// vertices have the attribute "position"
	GLint gl_pos_attribute = glGetAttribLocation(shader_program, "position");

	// tell the program that, to read the attribute "position," use the VBO
	// currently bound to GL_ARRAY_BUFFER and read three floats per vertex in-order
	glVertexAttribPointer(gl_pos_attribute, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// enable this attribute
	glEnableVertexAttribArray(gl_pos_attribute);

/************
	main loop
	*/

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	Vec3 *player_pos = calloc(sizeof(Vec3), 1);

	while (!glfwWindowShouldClose(window)) {

		if (input_up)    { player_pos->z += 0.01; }
		if (input_down)  { player_pos->z -= 0.01; }
		if (input_right) { player_pos->x += 0.01; }
		if (input_left)  { player_pos->x -= 0.01; }

		update_mesh(player_pos, shader_program);

		glClear(GL_COLOR_BUFFER_BIT);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

/********************
	terminate program
	*/

	free(player_pos);

    glfwTerminate();
}