#ifndef DATAGENERATOR_H
#define DATAGENERATOR_H

#include "addressbook.h"

/*
const int GEN_NAME_MAX = 10;
const int GEN_DATE_MAX = 10;
const int GEN_MAIL_MAX = 35;
const int GEN_PHONE_MAX = 9;
const int GEN_ADDRESS_MAX = 60;
*/

contact_t** generate_data(int max);
char* gen_name(int max);
char* gen_date();
char* gen_mail();
char* gen_phone();
char* gen_address();

#endif
