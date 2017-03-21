#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>

#define _XOPEN_SOURCE 500
#include <ftw.h>

struct FTW {
    int base;
    int level;
};

int MAX_SIZE;

void list_catalog_nftw(char* path, int max_size);
void list_catalog_nftw(char* path, int max_size);
const int S_ISVTX = 0001000;
int nftw(const char *dirpath, int (*fn) (const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf), int nopenfd, int flags);

int main(int argc, char* argv[]) {
    char* info = "Usage: <path> <size>\n";
    if(argc != 3) {
        printf("%s", info);
        return -1;
    }
    char* path = argv[1];
    int max_size = (int)strtol(argv[2], NULL, 10);
    list_catalog_nftw(path, max_size);
    return 0;
}

int print_file_info_nftw(const char* path, const struct stat* pstat, int typeflag, struct FTW* sftw){
  char file_path[255];
  strcpy(file_path, path);

  if(!S_ISREG(pstat->st_mode) || (int)pstat->st_size > MAX_SIZE){
      return 0;
  }
  off_t file_size = pstat->st_size;
  time_t file_st_mtime = pstat->st_mtime;

  char* file_access = (char*)calloc(10, sizeof(char));
  if(pstat->st_mode & S_IRUSR) file_access[0] = 'r'; else file_access[0] = '-';
  if(pstat->st_mode & S_IWUSR) file_access[1] = 'w'; else file_access[1] = '-';
  if(pstat->st_mode & S_ISUID){
      if(pstat->st_mode & S_IXUSR) file_access[2] = 's'; else file_access[2] = 'S';
  }
  else{
      if(pstat->st_mode & S_IXUSR) file_access[2] = 'x'; else file_access[2] = '-';
  }
  if(pstat->st_mode & S_IRGRP) file_access[3] = 'r'; else file_access[3] = '-';
  if(pstat->st_mode & S_IWGRP) file_access[4] = 'w'; else file_access[4] = '-';
  if(pstat->st_mode & S_ISGID){
      if(pstat->st_mode & S_IXGRP) file_access[5] = 's'; else file_access[5] = 'S';
  }
  else{
      if(pstat->st_mode & S_IXGRP) file_access[5] = 'x'; else file_access[5] = '-';
  }
  if(pstat->st_mode & S_IROTH) file_access[6] = 'r'; else file_access[6] = '-';
  if(pstat->st_mode & S_IWOTH) file_access[7] = 'w'; else file_access[7] = '-';
  if(pstat->st_mode & S_ISVTX){
      if(pstat->st_mode & S_IXOTH) file_access[8] = 't'; else file_access[8] = 'T';
  }
  else{
      if(pstat->st_mode & S_IXOTH) file_access[8] = 'x'; else file_access[8] = '-';
  }
  file_access[9] ='\0';
  printf("%s %d %s %s", file_path, (int)file_size, file_access, ctime(&file_st_mtime));

  free(file_access);
  return 0;
}

void list_catalog_nftw(char* path, int max_size){
    char absolute_path[255];
    if(path[0] != '/'){
        getcwd(absolute_path, 255);
        if(strlen(absolute_path) + strlen(path) > 255){
            printf("Too long path\n");
            exit(0);
        }
        strcat(absolute_path, "/");
    }
    MAX_SIZE = max_size;
    strcat(absolute_path, path);
    nftw(absolute_path, print_file_info_nftw, 10, 0);
}
