boobcraft: main.c
	gcc -o boobcraft main.c -I./SDL2/include/SDL2 -D_THREAD_SAFE -L./SDL2/lib -lSDL2

run: boobcraft
	@./boobcraft