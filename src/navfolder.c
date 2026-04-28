#include "navfolder.h"
#include "structs.h"
#include "wavParser.h"


#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>

void nav_folder(const char *foldername, int ident, struct MusicMetadata *lib, int *count)
{

    DIR *dir = opendir(foldername);

    if (dir == NULL){
        printf("Failed to open directory: %s\n", foldername);
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {

        char filepath[1024];
        struct stat statbuf;

        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) { continue; }

        snprintf(filepath, sizeof(filepath), "%s/%s", foldername, entry->d_name);

        if(stat(filepath, &statbuf) == -1) { perror("lstat"); }

        if (S_ISDIR(statbuf.st_mode))
        {
            //printf("Folder name: %s\n", entry->d_name);
            nav_folder(filepath, ident+1, lib, count);
        }
        else
        {
            for(int i = 0; i < ident; i++){
                //printf("\t");
            }
            //printf("File name: %s\n", entry->d_name);

            char *ext = strrchr(entry->d_name, '.');
            if(ext != NULL && strcmp(ext, ".wav") == 0)
            {
                if (*count < MAX_MUSIC_COUNT)
                {
                    struct MusicMetadata music = wavParser(filepath);
                    snprintf(music.filepath, sizeof(music.filepath), "%s", filepath);
                    lib[*count] = music;
                    (*count)++;
                }
                else
                {
                    printf("Max music count reached\n");
                    return;
                }

                //printf("Music title: %s \nArtist: %s\n\n", musicMetadata.title, musicMetadata.artist);
                //printf("Sample rate: %u \nNum channels: %u\n\n", musicMetadata.sample_rate, musicMetadata.num_channels);
            }
            else if(ext != NULL && strcmp(ext, ".mp3") == 0)
            {
                //struct Metadata test = mp3Parser(filepath);
                // printf("Music title: %s \n Artist: %s\n\n",test.title,test.artist);
            }
            else if(ext != NULL && strcmp(ext, ".flac") == 0)
            {
                //struct Metadata test = flacParser(filepath);
                // printf("Music title: %s \n Artist: %s\n\n",test.title,test.artist);
            }
        }
    }
    closedir(dir);
}
