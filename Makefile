default:
	gcc -o muplayer src/main.c src/wavParser.c src/navfolder.c

ray:
	gcc -o rayTest src/MainRay.c -I src/include -L src/lib -lraylib -lgdi32 -lwinmm src/wavParser.c src/navfolder.c
