EXEC = mandelbrot

ifdef RELEASE
	CFLAGS = `shell pkg-config sdl3 --cflags` -Wall -Iinclude -std=c++20 -O3
else
	CFLAGS = `shell pkg-config sdl3 --cflags` -Wall -g -Iinclude -std=c++20 -DDEBUG=1
endif 

LDFLAGS = $(shell pkg-config sdl3 --libs) -lm

SRC= $(wildcard *.cpp)
_OBJS= $(SRC:.cpp=.o)
OBJS = $(patsubst %,bin/%,$(_OBJS))

NB_FILES = $(words $(SRC))
CURRENT_NB = 0

all: start $(EXEC) shaders

run: all
	./$(EXEC)

clean: clear all

start: 
	@if [ -z "$(NOCLEAR)" ]; then\
		clear;\
	fi
	@printf "\n\033[1;33mStarted compilation\n\033[0m"
	@mkdir -p bin

clear:
	rm -r bin/*

bin/%.o: %.cpp #$(SRC)
	$(eval CURRENT_NB=$(shell echo $$(($(CURRENT_NB)+1))))
	@printf "\033[1;33m[$(CURRENT_NB)/$(NB_FILES)]\033[0m "
	@mkdir -p $(dir $@)
	g++ $(CFLAGS) -c $< -o $@

shaders/fragment.spv: shaders/fragment.glsl
	glslc -fshader-stage=fragment $< -o $@ 

shaders/vertex.spv: shaders/vertex.glsl
	glslc -fshader-stage=vertex $< -o $@ 

shaders: shaders/fragment.spv shaders/vertex.spv

$(EXEC): $(OBJS) 
	@printf "\033[1;33m[Linking]\033[0m "
	g++ $^ -o $@ $(LDFLAGS) 
	@printf "\033[0;32mSuccessfully compiled executable : $(EXEC) !\n\033[0m"