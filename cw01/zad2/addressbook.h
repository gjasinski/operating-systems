#ifndef ADDRESSBOOK_H
#define ADDRESSBOOK_H

typedef struct contact_t{
  char *data[6];
}contact_t;

contact_t* create_contact(char* fname, char* lname, char* bdate, char* email, char* address, char* telephone);
void print_contact(contact_t* contact);
void delete_contact(contact_t* contact);

#endif
