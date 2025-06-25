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

	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		crash("Could not initialize SDL");
	}

	// init OpenGL
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

	// window
	SDL_Window *window = SDL_CreateWindow(PROGRAM_NAME, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 700, 500, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

	if (!window) {
        crash("Could not create window");
    }

	SDL_GLContext context = SDL_GL_CreateContext(window);

	glewExperimental = GL_TRUE;
	glewInit();

	// test OpenGL rendering
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
	GLuint vertexShader = load_shader("vertex.glsl", GL_VERTEX_SHADER);
	GLuint fragmentShader = load_shader("fragment.glsl", GL_VERTEX_SHADER);

	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);

	glLinkProgram(shaderProgram);
	glUseProgram(shaderProgram);

	// link vertex data and shader attributes
	GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
	glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(posAttrib);

	// process events until window is closed
	SDL_Event event;
	int quit = FALSE;

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	while (!quit) {

		while (SDL_PollEvent(&event)) {

			if (event.type == SDL_QUIT) {
				quit = TRUE;
			}
		}

		glClear(GL_COLOR_BUFFER_BIT);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		SDL_GL_SwapWindow(window);
	}

	SDL_DestroyWindow(window);
	SDL_GL_DeleteContext(context);
	SDL_Quit();

    return 0;
}