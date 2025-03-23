#include <stdio.h>
#define GLEW_STATIC
#include "lib/glew.h"
#include "lib/glfw3.h"

// gcc main.c lib/libglfw.3.dylib -o a.out -framework OpenGL -rpath lib/
// ./a.out

// I really don't wanna make a makefile
// https://open.gl/

// OpenGL/GLFW/GLEW

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

/* send vertices to graphics card via VBO */
	
	// create buffer (returns index)
	GLuint vbo;
	glGenBuffers(1, &vbo);

	// make this buffer the active object
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	float vertices[] = {
		0.0f,  0.5f, // Vertex 1 (X, Y)
		0.5f, -0.5f, // Vertex 2 (X, Y)
	   -0.5f, -0.5f  // Vertex 3 (X, Y)
	};

	// copy vertex data to active buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

/* create shader program and send to graphics card */

	const char* vertexSource = "#version 150 core \n in vec2 position; void main() { gl_Position = vec4(position, 0.0, 1.0); }";

	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexSource, NULL);
	glCompileShader(vertexShader);

	const char* fragmentSource = "#version 150 core \n out vec4 outColor; void main() { outColor = vec4(1.0, 1.0, 1.0, 1.0); }";

	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
	glCompileShader(fragmentShader);

	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);

	glLinkProgram(shaderProgram);
	glUseProgram(shaderProgram);

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

/* link between vertex data and attributes */

	GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
	glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(posAttrib);

/* loop */
	while (!glfwWindowShouldClose(window)) {

		glDrawArrays(GL_TRIANGLES, 0, 3);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}




/* terminate program */
    glfwTerminate();
}