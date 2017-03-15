#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "addressbook.h"

const int  CONTACT_SIZE = 6;

contact_t* create_contact(char* fname, char* lname, char* bdate, char* email, char* address, char* telephone){
  contact_t* contact = (contact_t*)calloc(1, sizeof(contact_t));
  contact->data[0] = (char*)calloc(strlen(fname), sizeof(char));
  contact->data[1] = (char*)calloc(strlen(lname), sizeof(char));
  contact->data[2] = (char*)calloc(strlen(bdate), sizeof(char));
  contact->data[3] = (char*)calloc(strlen(email), sizeof(char));
  contact->data[4] = (char*)calloc(strlen(address), sizeof(char));
  contact->data[5] = (char*)calloc(strlen(telephone), sizeof(char));

  memcpy(contact->data[0], fname, strlen(fname));
  memcpy(contact->data[1], lname, strlen(lname));
  memcpy(contact->data[2], bdate, strlen(bdate));
  memcpy(contact->data[3], email, strlen(email));
  memcpy(contact->data[4], address, strlen(address));
  memcpy(contact->data[5], telephone, strlen(telephone));
  return contact;
}

void print_contact(contact_t* contact){
  for(int i = 0; i < CONTACT_SIZE; i++){
    printf(contact->data[i] + " ");
  }
  printf("\n");
}

void delete_contact(contact_t* contact){
  for(int i = 0; i < CONTACT_SIZE; i++){
    free(contact->data[i]);
  }
  free(contact);
}
