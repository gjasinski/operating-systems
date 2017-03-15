#ifndef LIST_H
#define LIST_H

#include "addressbook.h"

typedef struct node_t{
  struct node_t* next;
  struct node_t* prev;
  contact_t *data;
}node_t;

typedef struct list_t{
  struct node_t* first;
  struct node_t* last;
}list_t;

list_t* create_list();
node_t* create_node(char* fname, char* lname, char* bdate, char* email, char* address, char* telephone);
void add_node(list_t* list, node_t* node);
void add_contact_to_address_book_list(list_t* list, char* fname, char* lname, char* bdate, char* email, char* address, char* telephone);
void remove_node(node_t* node);
list_t* remove_list(list_t* list);
node_t* find_contact_list(list_t* list, char* query, int search_by);
node_t* find_min(node_t* node, int search_by);
list_t* sort_list_by(list_t* list, int order_by);
void find_and_remove(list_t* list, char* query, int search_by);
void print_node(node_t* node);
void print_list(node_t* node);
void find_and_print(list_t* list, char* query, int search_by);
#endif
