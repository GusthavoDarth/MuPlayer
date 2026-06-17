CC     = gcc
CFLAGS = -Wall -Wextra -I src/include -I src
SRCS   = src/main.c src/wavParser.c src/navfolder.c src/playback.c src/flac.c
OUT    = MuPlayer

ifeq ($(OS),Windows_NT)
    LIBS = -L src/lib -lraylib -lgdi32 -lwinmm
    RM   = del /Q $(OUT).exe 2>NUL
else
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Darwin)
        LIBS = -L src/lib -lraylib -framework IOKit -framework Cocoa -framework OpenGL
    else
        LIBS = -L src/lib -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
    endif
    RM = rm -f $(OUT)
endif

default:
	$(CC) -o $(OUT) $(SRCS) $(CFLAGS) $(LIBS)

clean:
	$(RM)
