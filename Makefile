boobcraft: main.c
	gcc -o boobcraft main.c `./sdl2-config --cflags`

run: boobcraft
	@./boobcraft