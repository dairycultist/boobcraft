a.out: main.c sectors.c
	@gcc main.c lib/libglfw.3.dylib lib/glew.o -o a.out -framework OpenGL -rpath lib/

run: a.out
	@./a.out