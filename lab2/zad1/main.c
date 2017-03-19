#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <time.h>
#include <sys/times.h>
#include <string.h>

void lib_swap(FILE *pFILE, int i, int j, int record_size);
void lib_seek(FILE *pFILE, int offset, int origin);
void lib_read(char *buffer, int count, FILE *pFILE);
void lib_write(FILE *pFILE, char *buff, int record_size);
void lib_generate(char* path_name, int records, int record_size);
void lib_shuffle(char *path_name, int records, int record_size);
void lib_sort(char* path_name, int records, int record_size);

void sys_read(int descriptor, char *buff1, int size);
void sys_seek(int file_descriptor, int i, int i1);
void sys_write(int file_descriptor, char* buff1, int record_size);
void sys_swap(int file_descriptor, int i, int j, int record_size) ;
void sys_generate(char* path_name, int records, int record_size);
void sys_shuffle(char *path_name, int records, int record_size);
void sys_sort(char* path_name, int records, int record_size);
void sys_close_file(int file_descriptor);

void init_time();

void get_and_print_time();

double real_start;
double user_start;
double system_start;

int main(int argc, char* argv[]) {
    char* info = "Usage: <sys|lib> <generate|shuffle|sort> <path file> <records> <size of records>\n";
    if(argc != 6) {
        printf("%s\n",info);
        return -1;
    }
    char* filename = argv[3];
    int records = (int)strtol(argv[4], NULL, 10);
    int size_of_records = (int)strtol(argv[5],NULL, 10);
    init_time();
    if(strcmp(argv[1], "lib") == 0){
        if(strcmp(argv[2], "generate") == 0) lib_generate(filename, records, size_of_records);
        if(strcmp(argv[2], "shuffle") == 0) lib_shuffle(filename, records, size_of_records);
        if(strcmp(argv[2], "sort") == 0) lib_sort(filename, records, size_of_records);
    }
    if(strcmp(argv[1], "sys") == 0){
        if(strcmp(argv[2], "generate") == 0) sys_generate(filename, records, size_of_records);
        if(strcmp(argv[2], "shuffle") == 0) sys_shuffle(filename, records, size_of_records);
        if(strcmp(argv[2], "sort") == 0) sys_sort(filename, records, size_of_records);
    }
    get_and_print_time();
    return 0;
}

void lib_generate(char* path_name, int records, int record_size){
    FILE* file_dev_rand = fopen("/dev/random", "r");
    FILE* file_new_file = fopen(path_name, "w+");
    if(file_dev_rand == NULL || file_new_file == NULL){
        printf("lib_generator - open file - error");
        exit(1);
    }

    char* buffer = (char*)calloc((size_t)record_size + 1, sizeof(char));

    for(int i = 0; i < records; i++){
        lib_read(buffer, record_size, file_dev_rand);

        for(int j = 0; j < record_size; j++){
            buffer[j] = (char)(buffer[j]%25 + 97);
        }
        buffer[record_size] = '\0';
        lib_write(file_new_file, buffer, record_size);
    }

    free(buffer);
    fclose(file_dev_rand);
    fclose(file_new_file);
}

void lib_shuffle(char *path_name, int records, int record_size){
    FILE * file_to_shuffle = fopen(path_name, "r+");
    if(file_to_shuffle == NULL){
        printf("lib_shuffle - open file - error");
        exit(1);
    }

    srand(time(NULL));

    for(int i = records - 1; i > 1; i--){
        int n = rand() % (i + 1);
        lib_swap(file_to_shuffle, i, n, record_size);
    }

    fclose(file_to_shuffle);
}

void lib_sort(char* path_name, int records, int record_size){
    FILE* file_to_sort = fopen(path_name, "r+");
    if(file_to_sort == NULL){
        printf("lib_shuffle - open file - error");
        exit(1);
    }

    char* c1 = (char*)calloc(1, sizeof(char));
    char* c2 = (char*)calloc(1, sizeof(char));
    for(int i = 0; i < records; i++){
        for(int j = 0; j < records - 1 - i; j++){
            lib_seek(file_to_sort, j * record_size, SEEK_SET);
            lib_read(c1, sizeof(char), file_to_sort);
            lib_seek(file_to_sort, (j + 1 ) * record_size, SEEK_SET);
            lib_read(c2, sizeof(char), file_to_sort);
            if(c2[0] < c1[0]) {
                lib_swap(file_to_sort, j, j + 1, record_size);
            }
        }
    }

    free(c1);
    free(c2);
    fclose(file_to_sort);
}

void lib_read(char* buffer, int count, FILE* pFILE) {
    size_t result = fread(buffer, sizeof(char), (size_t)count, pFILE);
    if(result != count){
        printf("lib_read - error");
        exit(1);
    }
}

void lib_seek(FILE* pFILE, int offset, int origin) {
    int result = fseek(pFILE, offset, origin);
    if(result != 0){
        printf("lib_seek - error");
        exit(1);
    }
}



void lib_write(FILE *pFILE, char* buff, int record_size) {
    size_t result = fwrite(buff, sizeof(char), (size_t)record_size, pFILE);
    if(result != record_size){
        printf("lib_generate - write - error\n");
        exit(1);
    }
}

void lib_swap(FILE *pFILE, int i, int j, int record_size) {
    char* buff1 = (char*)calloc((size_t)record_size + 1, sizeof(char));
    char* buff2 = (char*)calloc((size_t)record_size + 1, sizeof(char));
    lib_seek(pFILE, i * record_size, SEEK_SET);
    lib_read(buff1, record_size, pFILE);
    lib_seek(pFILE, j * record_size, SEEK_SET);
    lib_read(buff2, record_size, pFILE);

    lib_seek(pFILE, i * record_size, SEEK_SET);
    lib_write(pFILE, buff2, record_size);
    lib_seek(pFILE, j * record_size, SEEK_SET);
    lib_write(pFILE, buff1, record_size);

    free(buff1);
    free(buff2);
}


void sys_generate(char* path_name, int records, int record_size){
    srand(time(NULL));
    int file_dev_rand = open("/dev/random", O_RDONLY);
    int file_new_file = open(path_name, O_WRONLY | O_CREAT | O_TRUNC, 0666);

    if (file_dev_rand == -1 || file_new_file == -1){
        printf("sys_generate - open file - error");
        exit(1);
    }
    char* buff = (char*)calloc((size_t)record_size + 1, sizeof(char));
    for(int i = 0; i < records; i++){
        sys_read(file_dev_rand, buff, record_size);

        for(int j = 0; j < record_size; j++){
            buff[j] = (char)(buff[j] % 74 + 48);
            //buff[j] = (char)(rand() % 25 + 97);
        }
        buff[record_size-1]='\n';
        buff[record_size] = '\0';
        sys_write(file_new_file, buff, record_size);
    }
    free(buff);
    sys_close_file(file_new_file);
}

void sys_shuffle(char *path_name, int records, int record_size){
    int file_to_shuffle = open(path_name, O_RDWR);
    if(file_to_shuffle == -1){
        printf("sys_shuffle - open file - error\n");
        exit(1);
    }

    srand(time(NULL));

    for(int i = records - 1; i > 1; i--){
        int n = rand() % (i + 1);
        sys_swap(file_to_shuffle, i, n, record_size);
    }

    sys_close_file(file_to_shuffle);
}

void sys_sort(char* path_name, int records, int record_size){
    int file_to_sort = open(path_name, O_RDWR);
    if(file_to_sort == -1){
        printf("sys_sort - open file - error");
        exit(1);
    }
    char* c1 = (char*)calloc(1, sizeof(char));
    char* c2 = (char*)calloc(1, sizeof(char));
    for(int i = 0; i < records; i++){
        for(int j = 0; j < records - 1 - i; j++){
            sys_seek(file_to_sort, j * record_size, SEEK_SET);
            sys_read(file_to_sort, c1, sizeof(char));
            sys_seek(file_to_sort, (j + 1 ) * record_size, SEEK_SET);
            sys_read(file_to_sort, c2, sizeof(char));
            if(c2[0] < c1[0]) {
                printf("%c %c", c1[0], c2[0]);
                sys_swap(file_to_sort, j, j + 1, record_size);
            }
        }
    }

    free(c1);
    free(c1);
    sys_close_file(file_to_sort);
}

void sys_swap(int file_descriptor, int i, int j, int record_size) {
    char* buff1 = (char*)calloc((size_t)record_size + 1, sizeof(char));
    char* buff2 = (char*)calloc((size_t)record_size + 1, sizeof(char));
    sys_seek(file_descriptor, i * record_size, SEEK_SET);
    sys_read(file_descriptor, buff1, record_size);
    sys_seek(file_descriptor, j * record_size, SEEK_SET);
    sys_read(file_descriptor, buff2, record_size);

    sys_seek(file_descriptor, i * record_size, SEEK_SET);
    sys_write(file_descriptor, buff2, record_size);
    sys_seek(file_descriptor, j * record_size, SEEK_SET);
    sys_write(file_descriptor, buff1, record_size);

    free(buff1);
    free(buff2);
}

void sys_seek(int file_descriptor, int offset, int whence) {
    int c = (int)lseek(file_descriptor, offset, whence);
    if(c == -1){
        printf("sys_seek - lseek - error\n");
        exit(1);
    }
}

void sys_read(int file_descriptor, char* buff1, int record_size) {
    int c = (int)read(file_descriptor, buff1, (size_t)record_size);
    if(c != record_size){
        printf("sys_read - read - error\n");
        // exit(1);
    }
}


void sys_write(int file_descriptor, char* buff1, int record_size) {
    int c = (int)write(file_descriptor, buff1, (size_t)record_size);
    if(c != record_size){
        printf("sys_write - read - error\n");
        exit(1);
    }
}

void sys_close_file(int file_descriptor){
    if(close(file_descriptor) != 0){
        printf("sys_close_file - error\n");
        exit(1);
    }
}

void init_time(){
    struct tms buffer;
    times(&buffer);
    real_start = clock() / (double)CLOCKS_PER_SEC;
    user_start = buffer.tms_utime / (double)CLOCKS_PER_SEC;
    system_start = buffer.tms_stime / (double)CLOCKS_PER_SEC;
}

void get_and_print_time(){
    struct tms buffer;
    times(&buffer);

    double real = clock() / (double)CLOCKS_PER_SEC;
    double user = buffer.tms_utime / (double)CLOCKS_PER_SEC;
    double sys = buffer.tms_stime / (double)CLOCKS_PER_SEC;

    printf("Times: real = %f, user = %f, system = %f\n", real - real_start, user - user_start, sys - system_start);
    printf("Times from start: real = %f, user = %f, system = %f\n\n", real - real_start, user - user_start, sys - system_start);

}
