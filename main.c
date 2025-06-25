#define GLEW_STATIC
#include "GLEW/glew.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include "util.c"

#define TRUE 1
#define FALSE 0
#define PROGRAM_NAME "Boobcraft"

int main() {

	printf("Starting %s\n", PROGRAM_NAME);

	App *app = init_app(PROGRAM_NAME);

	// test OpenGL rendering

	// make vertex array
	GLuint vertexArray;
	glGenVertexArrays(1, &vertexArray);
	glBindVertexArray(vertexArray);

	// make vertex buffer
	float vertices[] = {
		0.0f,  0.5f,
		0.5f, -0.5f,
		-0.5f, -0.5f
	};

	GLuint vertexBuffer;			 											// create vertex buffer object
	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);								// make it the active buffer

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); 	// copy vertex data into the active buffer

	// make shader
	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, load_shader("vertex.glsl", GL_VERTEX_SHADER));
	glAttachShader(shaderProgram, load_shader("fragment.glsl", GL_FRAGMENT_SHADER));

	glLinkProgram(shaderProgram);
	glUseProgram(shaderProgram);
	
	// link active vertex data and shader attributes
	GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
	glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(posAttrib); // requires a VAO to be bound

	// process events until window is closed
	SDL_Event event;
	int running = TRUE;

	glClearColor(0.0f, 0.5f, 0.0f, 1.0f);

	while (running) {

		while (SDL_PollEvent(&event)) {

			if (event.type == SDL_QUIT) {
				running = FALSE;
			}
		}

		glClear(GL_COLOR_BUFFER_BIT);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		SDL_GL_SwapWindow(app->window);
	}

	SDL_DestroyWindow(app->window);
	SDL_GL_DeleteContext(app->context);
	SDL_Quit();

    return 0;
}