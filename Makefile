# this is game-implementation agnostic
engine/engine.o: engine/*.c engine/engine.h
	@gcc -c -o engine/engine.o engine/main.c

# this should be modified based on your game's file structure
example_game/game: example_game/main.c example_game/res/* engine/engine.o
	@gcc -o example_game/game example_game/main.c engine/engine.o -lGLEW -framework OpenGL $(shell sdl2-config --libs) $(shell sdl2-config --cflags)

run: example_game/game
	@./example_game/game