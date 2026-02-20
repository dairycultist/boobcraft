UNAME := $(shell uname)

ifeq ($(UNAME), Darwin) # macOS
	GCCFLAGS = -lGLEW -framework OpenGL
else ifeq ($(UNAME), Linux)
	GCCFLAGS = -lGLEW -lGL -lm
else
	$(error Unsupported OS: $(UNAME))
endif

game: res/* src/*
	@gcc -o game src/main.c $(GCCFLAGS) $(shell pkg-config --cflags --libs sdl2 SDL2_image)

run: game
	@./game

clean:
	rm game