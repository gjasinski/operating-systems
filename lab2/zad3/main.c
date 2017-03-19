#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

void set_lock_read_error(int file_desc, long int i_char);
void set_lock_read_wait(int file_desc, long int i_char);
void set_lock_write_error(int file_desc, long int i_char);
void set_lock_write_wait(int file_desc, long int i_char);
void check_locks(int file_desc);
void unlock_file(int file_desc, long int i_char);
void read_char(int file_desc, long int i_char);
void write_char(int file_desc, long int i_char, char* c);

int main(int argc, char* argv[]) {
    if(argc != 2){
        printf("%s\n", "main <file path>");
        return 0;
    }
    int file_desc = open(argv[1], O_RDWR);
    long int end = lseek(file_desc, 0, SEEK_END);
    char input;
    char* char_to_write = (char*)calloc(1, sizeof(char));
    long int i_char;
    do{
        printf("%s\n", "Choose 1-8:");
        scanf("\n%c", &input);
        if(input != '5' && input != 'q' && input != '8') {
            printf("Choose char from 0 to %ld\n", end);
            scanf("%ld", &i_char);
        }
        if(input == '1') set_lock_read_error(file_desc, i_char);
        if(input == '2') set_lock_read_wait(file_desc, i_char);
        if(input == '3') set_lock_write_error(file_desc, i_char);
        if(input == '4') set_lock_write_wait(file_desc, i_char);
        if(input == '5') check_locks(file_desc);
        if(input == '6') unlock_file(file_desc, i_char);
        if(input == '7') read_char(file_desc, i_char);
        if(input == '8') {
            printf("Choose char from 0 to %ld and char to write\n", end);
            scanf("%ld %c", &i_char, char_to_write);
            write_char(file_desc, i_char, char_to_write);
        }
    }while(input != 'q');
    return 0;
}

int lock_reg(int fd, int cmd, int type, off_t offset, int whence, off_t len){
    struct flock lock;
    lock.l_type = type;
    lock.l_start = offset;
    lock.l_whence = whence;
    lock.l_len = len;

    return(fcntl(fd, cmd, &lock));
}

struct flock get_lock(int file_desc, long int i_char, int lng){
  struct flock lock;
  lseek(file_desc, i_char, SEEK_SET);
  lock.l_type = F_WRLCK;
  lock.l_start = i_char;
  lock.l_whence = SEEK_SET;
  lock.l_len = lng;
  if(fcntl(file_desc, F_GETLK, &lock) < 0){
    printf("%s\n", "get_lock - error");
    exit(1);
  }
  return lock;
}

void set_lock_read_error(int file_desc, long int i_char){
    if(lock_reg(file_desc, F_SETLK, F_RDLCK, i_char, SEEK_SET, 1) == -1){
        printf("%d - can't be locked to read\n", file_desc);
    }
}

void set_lock_read_wait(int file_desc, long int i_char){
    if(lock_reg(file_desc, F_SETLKW, F_RDLCK, i_char, SEEK_SET, 1) == -1){
        printf("%d - can't be locked to read\n", file_desc);
    }
}


void set_lock_write_error(int file_desc, long int i_char){
    if(lock_reg(file_desc, F_SETLK, F_WRLCK, i_char, SEEK_SET, 1) == -1){
        printf("%d - can't be locked to write\n", file_desc);
    }
}

void set_lock_write_wait(int file_desc, long int i_char){
    if(lock_reg(file_desc, F_SETLKW, F_WRLCK, i_char, SEEK_SET, 1) == -1){
        printf("%d - can't be locked to write\n", file_desc);
    }
}

void check_locks(int file_desc){
    long int end = lseek(file_desc, 0, SEEK_END);
    struct flock lock;
    for(long int i = 0; i < end; i++){
        lock = get_lock(file_desc, i, 1);
        if(lock.l_type == F_WRLCK){
            printf("%ld WRITE %d\n", i, lock.l_pid);
        }
        if(lock.l_type == F_RDLCK){
            printf("%ld READ %d\n", i, lock.l_pid);
        }
    }
}

void unlock_file(int file_desc, long int i_char){
    if(lock_reg(file_desc, F_SETLK, F_UNLCK, i_char, SEEK_SET, 1) == -1){
        printf("%d - can't be unlocked\n", file_desc);
    }
}

void read_char(int file_desc, long int i_char){
    struct flock lock = get_lock(file_desc, i_char, 1);
    if(lock.l_type == F_WRLCK){
      printf("%s\n", "this char is locked");
      return;
    }
    lseek(file_desc, i_char, SEEK_SET);
    char c;
    if(read(file_desc, &c, sizeof(char)) == 1){
        printf("%c\n", c);
    }
    else{
        printf("%s\n", "read_char - can't read char\n");
    }
}

void write_char(int file_desc, long int i_char, char* c){
    struct flock lock = get_lock(file_desc, i_char, 1);
    if(lock.l_type != F_UNLCK){
      printf(" %s\n", "this char is locked");
      return;
    }
    lseek(file_desc, i_char, SEEK_SET);
    if(write(file_desc, c, sizeof(char)) != 1){
        printf(" %s\n", "write_char - can't write char\n");
    }
}
