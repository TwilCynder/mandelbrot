EXEC = mandelbrot

ifdef RELEASE
	CFLAGS = `sdl2-config --cflags` -Wall -Iinclude -std=c++20 -O3
else
	CFLAGS = `sdl2-config --cflags` -Wall -g -Iinclude -std=c++20 -DDEBUG=1
endif 

LDFLAGS = `sdl2-config --libs` -lm

SRC= $(wildcard *.cpp)
_OBJS= $(SRC:.cpp=.o)

OBJS = $(patsubst %,bin/%,$(_OBJS))

NB_FILES = $(words $(SRC))
CURRENT_NB = 0

all: start $(EXEC)

run: all
	./$(EXEC)

clean: clear all

start: 
	@if [ -z "$(NOCLEAR)" ]; then\
		clear;\
	fi
	@printf "\n\033[1;33mStarted compilation\n\033[0m"
	@mkdir -p bin

bin/%.o: %.cpp #$(SRC)
	$(eval CURRENT_NB=$(shell echo $$(($(CURRENT_NB)+1))))
	@printf "\033[1;33m[$(CURRENT_NB)/$(NB_FILES)]\033[0m "
	@mkdir -p $(dir $@)
	g++ $(CFLAGS) -c $< -o $@

clear:
	rm -r bin/*

$(EXEC): $(OBJS)
	@printf "\033[1;33m[Linking]\033[0m "
	g++ $^ -o $@ $(LDFLAGS) 
	@printf "\033[0;32mCompilation finished successfully !\n\033[0m"