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
void search_dir(char* path, int max_size);
static int child_fn() {
  printf("Creating new namespaces:\nStarting init process\n");

  if(mount("/", "/", "ext4", MS_PRIVATE, NULL) != 0) printf("err1\n%s\n", strerror(errno));
  if(mount("/", "/proc", "ext4", MS_PRIVATE, NULL) != 0) printf("err1\n%s\n", strerror(errno));
  if(mount("/", "/dev", "ext4", MS_PRIVATE, NULL) != 0) printf("err1\n%s\n", strerror(errno));
  if(mount("/", "/sys", "ext4", MS_PRIVATE, NULL) != 0) printf("err1\n%s\n", strerror(errno));
  if(mount("/", "/dev/pts", "ext4", MS_PRIVATE, NULL) != 0) printf("err1\n%s\n", strerror(errno));
  if(mount("/", "/dev/shm", "ext4", MS_PRIVATE, NULL) != 0) printf("err1\n%s\n", strerror(errno));
  if(mount("/", "/dev/hugepages", "ext4", MS_PRIVATE, NULL) != 0) printf("err1\n%s\n", strerror(errno));
  if(mount("/", "/dev/mqueue", "ext4", MS_PRIVATE, NULL) != 0) printf("err1\n%s\n", strerror(errno));
  if(mount("/", "/run", "ext4", MS_PRIVATE, NULL) != 0) printf("err1\n%s\n", strerror(errno));
  if(mount("/", "/usr", "ext4", MS_PRIVATE, NULL) != 0) printf("err1\n%s\n", strerror(errno));
//  if(mount("/", "/etc", "ext4", MS_PRIVATE, NULL) != 0) printf("err1\n%s\n", strerror(errno));
  //if(mount("/", "/sys/kernel/security", "ext4", MS_PRIVATE, NULL) != 0) printf("err1\n%s\n", strerror(errno));
//  if(mount("/", "/sys/kernel/debug", "ext4", MS_PRIVATE, NULL) != 0) printf("err1\n%s\n", strerror(errno));
 // if(mount("/", "/run/lock", "ext4", MS_PRIVATE, NULL) != 0) printf("err1\n%s\n", strerror(errno));
  //if(mount("/dev/sda1", "/vm", "ext4", 0, NULL) == -1) printf("err mount /%s\n", strerror(errno));
umount("/dev/pts");
  umount("/dev/shm");
  umount("/dev/mqueue");
  umount("/dev/hugepages");
if(umount("/dev")!= 0) printf("errrrrrr");
  //if(mount("/vm", "/", "ext4", MS_REMOUNT, NULL) == -1) printf("err mount dev/mque %s\n", strerror(errno));
  if(mount("/vm/dev", "/dev", "devtmpfs", MS_NOSUID, NULL) == -1) printf("err mount dev %s\n", strerror(errno));
if(mount("/vm/proc", "/proc", "proc", MS_NOSUID | MS_NODEV | MS_NOEXEC, NULL) == -1) printf("err mount proc %s\n", strerror(errno));
  if(mount("/vm/sys", "/sys", "sysfs", MS_NOSUID | MS_NODEV | MS_NOEXEC, NULL) == -1) printf("err mount sys %s\n", strerror(errno));

  if(mount("/vm/dev/pts", "/dev/pts", "devpts", MS_NOSUID | MS_NOEXEC, NULL) == -1) printf("err mount dev/pts %s\n", strerror(errno));
  if(mount("/vm/dev/shm", "/dev/shm", "tmpfs", MS_NOSUID | MS_NOEXEC, NULL) == -1) printf("err mount dev/dhm %s\n", strerror(errno));
  if(mount("/vm/dev/hugepages", "/dev/hugepages", "hugetlbfs", 0, NULL) == -1) printf("err mount dev/hugr %s\n", strerror(errno));
  if(mount("/vm/dev/mqueue", "/dev/mqueue", "mqueue", 0, NULL) == -1) printf("err mount dev/mque %s\n", strerror(errno));
  if(mount("/vm/run", "/run", "tmpfs", 0, NULL) == -1) printf("err mount dev/mque %s\n", strerror(errno));
  //if(mount("/vm/usr", "/usr", "tmpfs", 0, NULL) == -1) printf("err mount dev/mque %s\n", strerror(errno));
//if(mount("/vm/etc", "/etc", "tmpfs", 0, NULL) == -1) printf("err mount dev/mque %s\n", strerror(errno));
 // if(mount("/vm/sys/kernel/security", "/sys/kernel/security", "ext4", MS_PRIVATE, NULL) != 0) printf("err1\n%s\n", strerror(errno));
//  if(mount("/vm/sys/kernel/debug", "/sys/kernel/debug", "debugfs", 0, NULL) != 0) printf("err1\n%s\n", strerror(errno));
//  if(mount("/vm/etc/run/lock", "/run/lock", "ext4", MS_PRIVATE, NULL) != 0) printf("err1\n%s\n", strerror(errno));

  /*if(mount("/vm", "/dev/sda", "ext4", 0, NULL) == -1) printf("err mount dev/mque %s\n", strerror(errno));*/

 /* if(mount("/", "/dev/pts", "ext4", MS_SLAVE, NULL) != 0) printf("err1\n%s\n", strerror(errno));
  if(mount("/", "/dev/shm", "ext4", MS_SLAVE, NULL) != 0) printf("err1\n%s\n", strerror(errno));
  if(mount("/", "/dev/hugepages", "ext4", MS_SLAVE, NULL) != 0) printf("err1\n%s\n", strerror(errno));
  if(mount("/", "/dev/mqueue", "ext4", MS_SLAVE, NULL) != 0) printf("err1\n%s\n", strerror(errno));
  if(mount("/", "/dev", "ext4", MS_SLAVE, NULL) != 0) printf("err1\n%s\n", strerror(errno));
//  if(mount("/dev/sda1/vm", "/", "ext4", 0, NULL) == -1) printf("err umount vm %s\n", strerror(errno));
*/
  
//if(umount("/dev") != 0) printf("err");
  chdir("/vm");	
char buf[250];
getcwd(buf,250);
printf("cwd: %s\n", buf);
printf("pid: %d\n", getpid());
//execlp("less", "less", "/proc/self/mountinfo", NULL);
//execlp("ls", "ls", "/", NULL);
execlp("/sbin/init", "init", "2", NULL);
//execlp("/bin/sh", "sh", NULL);
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
  /*if(mount("", "/proc", "", MS_SHARED, NULL) != 0) printf("err0\n%s\n", strerror(errno));
  if(mount("", "/sys", "", MS_SHARED, NULL) != 0) printf("err0\n%s\n", strerror(errno));
  if(mount("", "/dev", "", MS_SHARED, NULL) != 0) printf("err0\n%s\n", strerror(errno));

  if(mount("", "/dev/pts", "", MS_SHARED, NULL) != 0) printf("err1\n%s\n", strerror(errno));
  if(mount("", "/dev/shm", "", MS_SHARED, NULL) != 0) printf("err1\n%s\n", strerror(errno));
  if(mount("", "/dev/hugepages", "", MS_SHARED, NULL) != 0) printf("err1\n%s\n", strerror(errno));
  if(mount("", "/dev/mqueue", "", MS_SHARED, NULL) != 0) printf("err1\n%s\n", strerror(errno));
*/
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

void search_dir(char* path, int max_size){
    char file_path[255];
    struct stat* stat_buf = (struct stat*)calloc(1, sizeof(struct stat));
    DIR* curr_dir = opendir(path);
    if(curr_dir == NULL){
       // printf("search_dir - error %s\n", path);
//        exit(1);
return;
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
	search_dir(file_path,255);
        file = readdir(curr_dir);
    }


    if(closedir(curr_dir) == -1){
        printf("search_dir - stat - error %s\n", path);
        exit(1);
    }
}


