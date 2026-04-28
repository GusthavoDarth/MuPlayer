#ifndef NAV_FOLDER_H
#define NAV_FOLDER_H
#include "structs.h"

#define MAX_MUSIC_COUNT 100


void nav_folder(const char *foldername, int ident, struct MusicMetadata *lib, int *count);

#endif
