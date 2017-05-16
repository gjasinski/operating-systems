#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int main (int argc, char* argv)
{
    FILE* file_ptr = fopen("data.txt", "w");
    char* buf = (char*)calloc(1024 + 4, sizeof(char));
    int i;
    for (i = 0; i < 10; i++){
        sprintf(buf, "000%d", i);
        for(int j = 0; j < 1024; j++){
            buf[j+4] = 65 + rand() % 25;
        }
        fwrite(buf, sizeof(char), 1028, file_ptr);
        printf("%d\n", i);
    }
    for (; i < 100; i++){
        sprintf(buf, "00%d", i);
        for(int j = 0; j < 1024; j++){
            buf[j+4] = 65 + rand() % 25;
        }
        fwrite(buf, sizeof(char), 1028, file_ptr);
        printf("%d\n", i);
    }
    for (; i < 600; i++){
        sprintf(buf, "0%d", i);
        for(int j = 0; j < 1024; j++){
            buf[j+4] = 65 + rand() % 25;
        }
        fwrite(buf, sizeof(char), 1028, file_ptr);
        printf("%d\n", i);
    }
    /*for (; i < 5000; i++){
        sprintf(buf, "00%d", i);
        for(int j = 0; j < 1024; j++){
            buf[j+3] = 65 + rand() % 25;
            fwrite(buf, sizeof(char), 1028, file_ptr);
        }
        printf("%d\n", i);
    }*/
    fclose(file_ptr);
}