#ifndef MAIN_H
#define MAIN_H

#include "addressbook.h"
#include "list.h"
#include "bsttree.h"

typedef struct result_t{
  double data[6];
  char* info;
}result_t;

contact_t** generate_data(int max, void* handle);
char* gen_name(int max);
char* gen_date();
char* gen_mail();
char* gen_phone();
char* gen_address();


#endif
