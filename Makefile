UNAME := $(shell uname)

ifeq ($(UNAME), Darwin) # macOS
	GCCFLAGS = -lGLEW -framework OpenGL
else ifeq ($(UNAME), Linux)
	GCCFLAGS = -lGLEW -lGL -lm
else
	$(error Unsupported OS: $(UNAME))
endif

client.out: client/*
	@gcc -o client.out client/main.c $(GCCFLAGS) $(shell pkg-config --cflags --libs sdl2 SDL2_image)

run: client.out
	@./client.out

clean:
	rm client.out