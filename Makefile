CC=clang++
CFLAGS= -Iinclude/ --std=c++17 -g3
LDFLAGS=-lglfw -lGL -lGLEW # -Llib

OBJ_FILES=obj/main.o obj/render.o obj/shader.o obj/components.o

build: $(OBJ_FILES) obj/flecs.o
	$(CC) -o build/a.out $(OBJ_FILES) obj/flecs.o $(CFLAGS) $(LDFLAGS)

obj/flecs.o:
	gcc -c -o obj/flecs.o src/flecs.c -Iinclude

obj/%.o: src/%.cpp
	$(CC) -c -o $@ $< $(CFLAGS)

run: build
	./build/a.out

clean:
	rm -f build/*
	rm -f obj/*

.PHONY: build run
