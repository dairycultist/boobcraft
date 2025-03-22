#include <stdio.h>
#include "GLFW/glfw3.h"
// #include <thread>

// gcc main.c -framework OpenGL
// -o
// ./a.out

// I really don't wanna make a makefile
// https://open.gl/
// we gon use GLFW

// int main() {

// 	printf("Hello world! Idk how OpenGL works\n");

// 	return 0;
// }

int main()
{
	int myvar = 0;

    glfwInit();

	scanf("%d", &myvar);
    // std::this_thread::sleep_for(std::chrono::seconds(1));
    // glfwTerminate();
}