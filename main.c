#include <stdio.h>
#include "lib/glfw3.h"

// gcc main.c lib/libglfw.3.dylib -o a.out -framework OpenGL -rpath lib/
// ./a.out

// I really don't wanna make a makefile
// https://open.gl/

int main() {

	printf("Hello world! Idk how OpenGL/GLFW works\n");
	// printf("OpenGL version: %s\n", glGetString(GL_VERSION));

    glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL", NULL, NULL); // Windowed
	// GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL", glfwGetPrimaryMonitor(), nullptr); // Fullscreen

	glfwMakeContextCurrent(window);

	while (!glfwWindowShouldClose(window)) {
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

    glfwTerminate();
}