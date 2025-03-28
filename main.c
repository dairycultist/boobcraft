#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define GLEW_STATIC // idk if I need this
#include "lib/glew.h"
#include "lib/glfw3.h"

#define PI 3.141592654

#define TRUE 1
#define FALSE 0

typedef struct {
	float x;
	float y;
	float z;
} Vec3;

typedef struct { // would be nice to load map data from a file...
	Vec3 *corners;
	GLuint vbo; // vertex buffer object
	// later we'll have floor and ceiling height but not rn
} Sector;

int IN_UP    = FALSE;
int IN_DOWN  = FALSE;
int IN_LEFT  = FALSE;
int IN_RIGHT = FALSE;
int IN_LTURN = FALSE;
int IN_RTURN = FALSE;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {

	if (action == GLFW_PRESS) {

		switch (key) {
			case GLFW_KEY_W: IN_UP = TRUE; break;
			case GLFW_KEY_S: IN_DOWN = TRUE; break;
			case GLFW_KEY_A: IN_LEFT = TRUE; break;
			case GLFW_KEY_D: IN_RIGHT = TRUE; break;
			case GLFW_KEY_N: IN_LTURN = TRUE; break;
			case GLFW_KEY_M: IN_RTURN = TRUE; break;
		}

	} else if (action == GLFW_RELEASE) {

		switch (key) {
			case GLFW_KEY_W: IN_UP = FALSE; break;
			case GLFW_KEY_S: IN_DOWN = FALSE; break;
			case GLFW_KEY_A: IN_LEFT = FALSE; break;
			case GLFW_KEY_D: IN_RIGHT = FALSE; break;
			case GLFW_KEY_N: IN_LTURN = FALSE; break;
			case GLFW_KEY_M: IN_RTURN = FALSE; break;
		}
	}
}

Sector *init_sector() {

	// create buffer (returns index)
	GLuint vbo;
	glGenBuffers(1, &vbo);

	// construct vertices of all sectors
	float vertices[] = {
		0,     -0.5f, -2.0f,
		0.5f,  -0.5f, 2.0f,
		-0.5f, -0.5f, 2.0f
	};

	// make this sector's buffer the active object
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	// copy vertex data to active buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	Sector *sector = malloc(sizeof(Sector));
	sector->vbo = vbo;

	return sector;
}

// later we'll have remesh sector for when its height changes
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

/*************************************
	initialize VERTEX DATA for sectors
	*/
	
	Sector *my_sector = init_sector();

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

	Vec3 *player_pos = calloc(sizeof(Vec3), 1); // should really put this inside a "agent" class or smth
	float player_rot = 0;

	while (!glfwWindowShouldClose(window)) {

		if (IN_UP)    { player_pos->x -= sin(player_rot) * 0.01; player_pos->z += cos(player_rot) * 0.01; }
		if (IN_DOWN)  { player_pos->x += sin(player_rot) * 0.01; player_pos->z -= cos(player_rot) * 0.01; }
		if (IN_RIGHT) { player_pos->x += cos(player_rot) * 0.01; player_pos->z += sin(player_rot) * 0.01; }
		if (IN_LEFT)  { player_pos->x -= cos(player_rot) * 0.01; player_pos->z -= sin(player_rot) * 0.01; }
		if (IN_RTURN) { player_rot -= 0.01; }
		if (IN_LTURN) { player_rot += 0.01; }

		glClear(GL_COLOR_BUFFER_BIT);

		redraw_sector(my_sector, *player_pos, player_rot, shader_program);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

/********************
	terminate program
	*/

	free(player_pos);

    glfwTerminate();
}