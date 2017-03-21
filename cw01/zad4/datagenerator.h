#ifndef DATAGENERATOR_H
#define DATAGENERATOR_H

#include "addressbook.h"

contact_t** generate_data(int max);
char* gen_name(int max);
char* gen_date();
char* gen_mail();
char* gen_phone();
char* gen_address();

#endif
