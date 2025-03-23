#include <stdio.h>
#include <stdlib.h>

#define GLEW_STATIC // idk if I need this
#include "lib/glew.h"
#include "lib/glfw3.h"

/*
 * gcc main.c lib/libglfw.3.dylib lib/glew.o -o a.out -framework OpenGL -rpath lib/
 * ./a.out
 *
 * I really don't wanna make a makefile
 * https://open.gl/
 *
 * using OpenGL/GLFW/GLEW
 */

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

	glewExperimental = GL_TRUE;
	glewInit();

	printf("OpenGL version: %s\n", glGetString(GL_VERSION));

/*****************************************
	send vertices to graphics card via VBO
	*/
	
	// create buffer (returns index)
	GLuint vbo;
	glGenBuffers(1, &vbo);

	// make this buffer the active object
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	float vertices[] = {
		0.0f,  0.5f,
		0.5f, -0.5f,
	   -0.5f, -0.5f
	};

	// copy vertex data to active buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

/**************************************************
	create shader program and send to graphics card
	*/

	GLuint vertexShader = load_shader("vertex.glsl", GL_VERTEX_SHADER);
	GLuint fragmentShader = load_shader("fragment.glsl", GL_FRAGMENT_SHADER);

	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);

	glLinkProgram(shaderProgram);
	glUseProgram(shaderProgram);

/******************************************
	link between vertex data and attributes
	*/

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
	glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(posAttrib);

/*******
	loop
	*/

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	while (!glfwWindowShouldClose(window)) {

		glClear(GL_COLOR_BUFFER_BIT);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

/********************
	terminate program
	*/

    glfwTerminate();
}