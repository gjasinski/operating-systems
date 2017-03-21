#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>

void print_file_info(char* file_path, int max_size);
void search_dir(char* path, int max_size);
void list_catalog_openclose(char *path, int max_size);
const int S_ISVTX = 0001000;

int main(int argc, char* argv[]) {
    char* info = "Usage: <path> <size>\n";
    if(argc != 3) {
        printf("%s", info);
        return -1;
    }
    char* path = argv[1];
    int max_size = (int)strtol(argv[2], NULL, 10);
    list_catalog_openclose(path, max_size);
    return 0;
}

void print_file_info(char* path, int max_size){
    char file_path[255];
    strcpy(file_path, path);
    struct stat* buf = (struct stat*)calloc(1, sizeof(struct stat));
    stat(file_path, buf);

    if(!S_ISREG(buf->st_mode) || (int)buf->st_size > max_size){
        free(buf);
        return;
    }
    int file_size = buf->st_size;
    time_t file_st_mtime = buf->st_mtime;

    char* file_access = (char*)calloc(10, sizeof(char));
    if(buf->st_mode & S_IRUSR) file_access[0] = 'r'; else file_access[0] = '-';
    if(buf->st_mode & S_IWUSR) file_access[1] = 'w'; else file_access[1] = '-';
    if(buf->st_mode & S_ISUID){
        if(buf->st_mode & S_IXUSR) file_access[2] = 's'; else file_access[2] = 'S';
    }
    else{
        if(buf->st_mode & S_IXUSR) file_access[2] = 'x'; else file_access[2] = '-';
    }
    if(buf->st_mode & S_IRGRP) file_access[3] = 'r'; else file_access[3] = '-';
    if(buf->st_mode & S_IWGRP) file_access[4] = 'w'; else file_access[4] = '-';
    if(buf->st_mode & S_ISGID){
        if(buf->st_mode & S_IXGRP) file_access[5] = 's'; else file_access[5] = 'S';
    }
    else{
        if(buf->st_mode & S_IXGRP) file_access[5] = 'x'; else file_access[5] = '-';
    }
    if(buf->st_mode & S_IROTH) file_access[6] = 'r'; else file_access[6] = '-';
    if(buf->st_mode & S_IWOTH) file_access[7] = 'w'; else file_access[7] = '-';
    if(buf->st_mode & S_ISVTX){
        if(buf->st_mode & S_IXOTH) file_access[8] = 't'; else file_access[8] = 'T';
    }
    else{
        if(buf->st_mode & S_IXOTH) file_access[8] = 'x'; else file_access[8] = '-';
    }
    file_access[9] ='\0';
    printf("%s %d %s %s", file_path, (int)file_size, file_access, ctime(&file_st_mtime));

    free(buf);
    free(file_access);
}


void search_dir(char* path, int max_size){
    char file_path[255];
    struct stat* stat_buf = (struct stat*)calloc(1, sizeof(struct stat));
    DIR* curr_dir = opendir(path);
    if(curr_dir == NULL){
        printf("search_dir - error %s\n", path);
        exit(1);
    }
    struct dirent* file;
    file = readdir(curr_dir);
    while(file != NULL){
        if(strcmp(file->d_name, ".") == 0 || strcmp(file->d_name, "..") == 0){
            file = readdir(curr_dir);
            continue;
        }
        strcpy(file_path, path);
        strcat(file_path, "/");
        strcat(file_path, file->d_name);
        if(stat(file_path, stat_buf) < 0) {
            printf("search_dir - stat - error %s\n", path);
            exit(1);
        }

        if(S_ISDIR(stat_buf->st_mode)) search_dir(file_path, max_size);
        else
        print_file_info(file_path, max_size);
        file = readdir(curr_dir);
    }

    if(closedir(curr_dir) == -1){
        printf("search_dir - stat - error %s\n", path);
        exit(1);
    }
}

void list_catalog_openclose(char *path, int max_size){
    char absolute_path[255];
    if(path[0] != '/'){
        getcwd(absolute_path, 255);
        if(strlen(absolute_path) + strlen(path) > 255){
            printf("Too long path\n");
            exit(0);
        }
        strcat(absolute_path, "/");
    }

    strcat(absolute_path, path);
    search_dir(absolute_path, max_size);
}
