boobcraft: main.c util.c app.c 3D.c res
	@gcc -o boobcraft main.c \
	GLEW/glew.o -framework OpenGL \
	-I./SDL2/include -D_THREAD_SAFE -L./SDL2/lib -lSDL2 # SDL2 headers and libraries

run: boobcraft
	@./boobcraft