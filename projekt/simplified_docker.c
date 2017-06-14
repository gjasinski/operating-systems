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

static char child_stack[10485760];
char** argv_global;

static int child_fn() {
  if(mount("/", "/", "ext4", MS_PRIVATE, NULL) != 0) printf("err0\n%s\n", strerror(errno));
  if(mount("/", "/proc", "ext4", MS_PRIVATE, NULL) != 0) printf("err1\n%s\n", strerror(errno));
  if(mount("/", "/dev", "ext4", MS_PRIVATE, NULL) != 0) printf("err2\n%s\n", strerror(errno));
  if(mount("/", "/sys", "ext4", MS_PRIVATE, NULL) != 0) printf("err3\n%s\n", strerror(errno));
  if(mount("/", "/dev/pts", "ext4", MS_PRIVATE, NULL) != 0) printf("err4\n%s\n", strerror(errno));
  if(mount("/", "/dev/shm", "ext4", MS_PRIVATE, NULL) != 0) printf("err5\n%s\n", strerror(errno));
  if(mount("/", "/run", "ext4", MS_PRIVATE, NULL) != 0) printf("err8\n%s\n", strerror(errno));

  umount("/dev/pts");
  umount("/dev/shm");
  umount("/dev/mqueue");
  umount("/dev/hugepages");
  umount("/dev");

  mkdir("/vm", 0777);
  mkdir("/vm/proc", 0777);
  if(mount("/vm/dev", "/dev", "devtmpfs", MS_NOSUID, NULL) == -1) printf("err mount dev %s\n", strerror(errno));
  if(mount("/vm/proc", "/proc", "proc", MS_NOSUID | MS_NODEV | MS_NOEXEC, NULL) == -1) printf("err mount proc %s\n", strerror(errno));
  if(mount("/vm/sys", "/sys", "sysfs", MS_NOSUID | MS_NODEV | MS_NOEXEC, NULL) == -1) printf("err mount sys %s\n", strerror(errno));
  if(mount("/vm/dev/pts", "/dev/pts", "devpts", MS_NOSUID | MS_NOEXEC, NULL) == -1) printf("err mount dev/pts %s\n", strerror(errno));
  if(mount("/vm/dev/shm", "/dev/shm", "tmpfs", MS_NOSUID | MS_NOEXEC, NULL) == -1) printf("err mount dev/dhm %s\n", strerror(errno));
  if(mount("/vm/run", "/run", "tmpfs", 0, NULL) == -1) printf("err mount dev/mque %s\n", strerror(errno));

  sethostname(argv_global[1], strlen(argv_global[1]));


  //execlp("/sbin/init", "init", "2", NULL);
  execlp("/bin/bash", "shell", NULL);
  return 0;
}

int main(int argc, char** argv){
  if(argc != 2){
    printf("./simplified_docker hostname\n");
    return 0;
  }
  argv_global = argv;
  pid_t child_pid = clone(child_fn, child_stack+10485760, CLONE_NEWPID | CLONE_NEWNET | CLONE_NEWUTS |
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
