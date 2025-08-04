game: src/* res/*
	@gcc -o game src/main.c GLEW/glew.o -framework OpenGL $(shell sdl2-config --libs) $(shell sdl2-config --cflags)

run: game
	@./game