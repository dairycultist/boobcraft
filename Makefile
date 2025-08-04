engine.o: engine/*
	@gcc -c -o engine.o engine/main.c

game: example_game/* engine.o
	@gcc -o game example_game/main.c engine.o -lGLEW -framework OpenGL $(shell sdl2-config --libs) $(shell sdl2-config --cflags)

run: game
	@./game