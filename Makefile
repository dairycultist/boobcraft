# this is game-implementation agnostic
include/framework.o: src/* include/framework.h
	@gcc -c -o include/framework.o src/main.c

# this should be modified based on your game's file structure
example_game/game: example_game/main.c example_game/res/* include/framework.o
	@gcc -o example_game/game example_game/main.c include/framework.o -lGLEW -framework OpenGL $(shell sdl2-config --libs) $(shell sdl2-config --cflags)

run: example_game/game
	@./example_game/game