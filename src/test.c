#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>

void nav_folder(const char *foldername, int ident){

    DIR *dir = opendir(foldername);

    if (dir == NULL){
        printf("Failed to open directory: %s\n", foldername);
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
            printf("Folder name: %s\n", entry->d_name);
            nav_folder(filepath, ident+1);
        }
        else
        {
            for(int i = 0; i < ident; i++){
                printf("\t");
            }
            //printf("File name: %s\n", entry->d_name);
            printf("filepath: %s\n", filepath);
        }
    }
    closedir(dir);
}


int main(void){
    nav_folder("C:/",0);
    return 0;
}
