.PHONY: all clean build

all: main build

build: main_f.o
	gcc -o main.so -shared main_f.o `pkg-config --libs goocanvas`

main_f.o: main.c
	gcc -o main_f.o -c -fPIC main.c `pkg-config --cflags goocanvas`

main: main.o
	gcc -o main main.o `pkg-config --libs goocanvas`

main.o: main.c
	gcc -g -c -o main.o main.c `pkg-config --cflags goocanvas` -g

clean:
	rm -f main
	rm -f main.o
