// isolation/abstraction is good :)

#define TRUE 1
#define FALSE 0

// typedef struct {

// 	int x;
// 	int y;
// 	int z;
// 	int pitch;
// 	// no one needs roll
// 	int yaw;

// } Transform;

typedef struct {

	GLuint vertex_array; // "VAO"
	uint vertex_count;
	GLuint shader_program; // not stored by the VAO so have to include separately

	// TODO store Transform
	// TODO store texture info (GLuint)
	// maybe also store the vertex_buffer ("VBO") if it needs to be manipulated

} Mesh;

void log_error(const char *msg) {
	
	if (strlen(SDL_GetError()) == 0) {
		fprintf(stderr, "\n%s: <No error given>\n\n", msg);
	} else {
		fprintf(stderr, "\n%s: %s\n\n", msg, SDL_GetError());
	}
}

int app(const char *window_title, void (*on_start)(), void (*on_terminate)(), void (*process_tick)(), void (*process_event)(SDL_Event)) {

	printf("Starting %s\n", window_title);

	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		log_error("Could not initialize SDL");
		return 1;
	}

	// init OpenGL
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

	// create the window
	SDL_Window *window = SDL_CreateWindow(window_title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 700, 500, SDL_WINDOW_OPENGL);

	if (!window) {
        log_error("Could not create window");
		return 1;
    }

	SDL_GLContext context = SDL_GL_CreateContext(window);

	glewExperimental = GL_TRUE;
	glewInit();
	
	// let programmer initialize stuff
	on_start();

	// process events until window is closed
	SDL_Event event;
	int running = TRUE;

	while (running) {

		while (SDL_PollEvent(&event)) {

			if (event.type == SDL_QUIT) {
				running = FALSE;
			} else {
				process_event(event);
			}
		}

		process_tick();

		SDL_GL_SwapWindow(window);
	}

	// free everything
	on_terminate();

	SDL_DestroyWindow(window);
	SDL_GL_DeleteContext(context);
	SDL_Quit();

	return 0;
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
		exit(1);
    }

	return shader;
}

GLuint load_shader_program(const char *vertex_path, const char *fragment_path) {
	
	GLuint shader_program = glCreateProgram();
	glAttachShader(shader_program, load_shader(vertex_path, GL_VERTEX_SHADER));
	glAttachShader(shader_program, load_shader(fragment_path, GL_FRAGMENT_SHADER));
	glLinkProgram(shader_program); // apply changes to shader program, not gonna call "glUseProgram" yet bc not drawing

	return shader_program;
}

Mesh *load_obj_as_mesh(const char *obj_path, const GLuint shader_program) {

	// https://open.gl/drawing
	// need Element buffers for loading obj

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
	
	// link active vertex data and shader attributes
	GLint posAttrib = glGetAttribLocation(shader_program, "position");
	glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(posAttrib); // requires a VAO to be bound

	Mesh *mesh = malloc(sizeof(Mesh));
	mesh->vertex_array = vertexArray;
	mesh->vertex_count = 3;
	mesh->shader_program = shader_program;

	return mesh;
}

void draw_mesh(const Mesh *mesh) {

	glBindVertexArray(mesh->vertex_array);
	glUseProgram(mesh->shader_program);

	glClear(GL_COLOR_BUFFER_BIT);
	glDrawArrays(GL_TRIANGLES, 0, mesh->vertex_count);
}