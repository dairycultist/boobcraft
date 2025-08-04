engine.o: src/* res/*
	@gcc -c -o engine.o src/main.c

game: src/game.c engine.o res/*
	@gcc -o game src/game.c engine.o -lGLEW -framework OpenGL $(shell sdl2-config --libs) $(shell sdl2-config --cflags)

run: game
	@./game