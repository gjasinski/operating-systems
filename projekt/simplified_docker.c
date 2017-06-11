#define _GNU_SOURCE
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mount.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>

static char child_stack[1048576];
void search_dir(char* path, int max_size);
static int child_fn() {
  printf("Creating new namespaces:\nStarting init process\n");
  /*printf("pid %d\n", getpid());
  char absolute_path[255];
  getcwd(absolute_path, 255);
  printf("Cwd %s\n", absolute_path);
  mkdir("test", 0777);
  //if(mount(absolute_path, "/test", "ext34", MS_REMOUNT, NULL) != 0) printf("err\n %s", strerror(errno));
  getcwd(absolute_path, 255);
  printf("Cwd %s\n", absolute_path);
  //if(umount("/")!= 0)printf("err\n %s\n", strerror(errno));
  //getcwd(absolute_path, 255);
  //printf("Cwd %s\n", absolute_path);
  */

  if(mount("/", "/", "ext4", MS_SLAVE, NULL) != 0) printf("err1\n%s\n", strerror(errno));
  if(mount("/dev/sda1", "/vm", "ext4" , 0, NULL) != 0) printf("err2\n%s\n", strerror(errno));
  search_dir("/vm", 255);
//  if(mount("/dev/sda1", "/vm", "ext4" , 0, NULL) != 0) printf("err2\n%s\n", strerror(errno));
  //mkdir("/vm/alamakota", 0777);
  chdir("/vm");	
char buf[250];
getcwd(buf,250);
printf("cwd: %s\n", buf);
printf("pid: %d\n", getpid());
  execl("/vm/sbin/init", "/vm/sbin/init 3", NULL);
  return 0;
}

int main(int argc, char** argv){
  if(argc != 2 && argc != 3){
    printf("./simplified_docker system_path \n./simplified_docker system_path pid_1_path\n");
  }

  char* pid_1_path = "/sbin/init";
  if (argc == 3){
    pid_1_path = argv[2];
  }
  if(mount("/", "/", "ext4", MS_SHARED, NULL) != 0) printf("err0\n%s\n", strerror(errno));
  pid_t child_pid = clone(child_fn, child_stack+1048576, CLONE_NEWPID | CLONE_NEWNET | CLONE_NEWUTS |
                          CLONE_NEWIPC | CLONE_NEWNS | SIGCHLD, NULL);
  /*
    CLONE_NEWPID - create the process in a new PID namespace
    CLONE_NEWNET - create the process in a new network namespace.
    CLONE_NEWNS -  then create the process in a new network namespace. 
    CLONE_NEWUTS - then create the process in a new UTS namespace, whose identifiers are initialized by duplicating the identifiers from the UTS namespace of the calling process. 
    CLONE_NEWIPC -  then create the process in a new IPC namespace.
  */

  waitpid(child_pid, NULL, 0);
  return 0;
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

        printf("%s\n", file_path);
        file = readdir(curr_dir);
    }


    if(closedir(curr_dir) == -1){
        printf("search_dir - stat - error %s\n", path);
        exit(1);
    }
}


