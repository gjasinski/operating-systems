#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "datagenerator.h"

contact_t** generate_data(int max){
  srand(time(NULL));
  contact_t** data = (contact_t**)calloc(max, sizeof(contact_t*));
  for(int i = 0; i < max; i++){
    data[i] = create_contact(gen_name(10), gen_name(10), gen_date(), gen_mail(), gen_phone(), gen_address());
  }
  return data;
}

char* gen_name(int max){
  char* data = (char*)calloc(max, sizeof(char));
  for(int i = 0; i < max; i++){
    data[i] = 'A' + rand()%57;
  }
  return data;
}

char* gen_date(){
  char* data = (char*)calloc(10, sizeof(char));
  data[0] = '0' + rand()%2;
  data[1] = '1' + rand()%9;
  data[2] = data[5] = ':';
  data[3] = '0';
  data[4] = '1' + rand()%9;
  data[6] = '1';
  for(int i = 7; i < 10; i++){
    data[i] = '0' + rand()%10;
  }
  return data;
}

char* gen_mail(){
  char* data = (char*)calloc(35, sizeof(char));
  strcpy(data, gen_name(20));
  data[20] = '@';
  strcat(data, gen_name(10));
  return data;
}

char* gen_phone(){
  char* data = (char*)calloc(9, sizeof(char));
  for(int i = 0; i < 9; i++){
    data[i] = '0' + rand()%10;
  }
  return data;
}

char* gen_address(){
  char* data = (char*)calloc(60, sizeof(char));
  strcpy(data, gen_name(20));
  strcat(data, ", ");
  strcat(data, gen_name(30));
  return data;
}
