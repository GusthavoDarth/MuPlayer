CC     = gcc
CFLAGS = -Wall -Wextra -I src/include -I src
LIBS   = -L src/lib -lraylib -lgdi32 -lwinmm

SRCS = src/main.c src/wavParser.c src/navfolder.c src/playback.c src/flac.c
OUT  = MuPlayer

default:
	$(CC) -o $(OUT) $(SRCS) $(CFLAGS) $(LIBS)

clean:
	del $(OUT).exe 2>NUL || rm -f $(OUT)
