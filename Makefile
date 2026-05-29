default:
	gcc -o MuPlay src/main.c -I src/include -L src/lib -lraylib -lgdi32 -lwinmm src/wavParser.c src/navfolder.c src/playback.c
