SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=)

FFMPEG_LIBS=    libavdevice                        \
                libavformat                        \
                libavfilter                        \
                libavcodec                         \
                libswresample                      \
                libswscale                         \
                libavutil                          \

all: circles spirals pendula

% : %.c
	gcc $^ -fopenmp -lm -lSDL -D_REENTRANT -I/usr/include/SDL -o $@

archimedes : archimedes.c recorder.o
	gcc -g $^ -lm $(shell pkg-config --cflags --libs $(FFMPEG_LIBS)) -fopenmp -lSDL -D_REENTRANT -I/usr/include/SDL -o $@

circles : circles.c recorder.o
	gcc -g $^ -lm $(shell pkg-config --cflags --libs $(FFMPEG_LIBS)) -fopenmp -lSDL -D_REENTRANT -I/usr/include/SDL -o $@

lines : lines.c recorder.o
	gcc -g $^ -lm $(shell pkg-config --cflags --libs $(FFMPEG_LIBS)) -fopenmp -lSDL -D_REENTRANT -I/usr/include/SDL -o $@

pendula: pendula.c recorder.o
	gcc -g $^ -lm $(shell pkg-config --cflags --libs $(FFMPEG_LIBS)) -fopenmp -lSDL -D_REENTRANT -I/usr/include/SDL -o $@

dot: dot.c recorder.o
	gcc -g $^ -lm $(shell pkg-config --cflags --libs $(FFMPEG_LIBS)) -fopenmp -lSDL -D_REENTRANT -I/usr/include/SDL -o $@

spiral: spiral.c recorder.o
	gcc -g $^ -lm $(shell pkg-config --cflags --libs $(FFMPEG_LIBS)) -fopenmp -lSDL -D_REENTRANT -I/usr/include/SDL -o $@

spirals: spirals.c recorder.o
	gcc -g $^ -lm $(shell pkg-config --cflags --libs $(FFMPEG_LIBS)) -fopenmp -lSDL -D_REENTRANT -I/usr/include/SDL -o $@

recorder.o: recorder.c
	gcc -g $^ -lm $(shell pkg-config --cflags --libs $(FFMPEG_LIBS)) -fopenmp -lSDL -D_REENTRANT -I/usr/include/SDL -c 

clean:
	$(RM) $(OBJS) recorder.o

install:
	cp circles /usr/bin/
