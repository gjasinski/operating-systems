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

static char child_stack[1048576];

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
  if(mount("/dev/sda", "/vm", "ext4" , 0, NULL) != 0) printf("err2\n%s\n", strerror(errno));
  if(mount("/dev/sda1", "/vm", "ext4" , 0, NULL) != 0) printf("err2\n%s\n", strerror(errno));
  //mkdir("/vm/alamakota", 0777);
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

