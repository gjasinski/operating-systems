#include <time.h>
#include <sys/times.h>
#include <stdio.h>
#include "string.h"
#include "datagenerator.h"
#include "addressbook.h"
#include "list.h"
#include "bsttree.h"

double real_start;
double real_previous;
double user_start;
double user_previous;
double system_start;
double system_previus;

void init_time(){
  struct tms buffer;
  times(&buffer);
  real_start = real_previous = clock() / (double)CLOCKS_PER_SEC;
  user_start = user_previous = buffer.tms_utime / (double)CLOCKS_PER_SEC;
  system_start = system_previus = buffer.tms_stime / (double)CLOCKS_PER_SEC;
}

void get_and_print_time(char* info){
  struct tms buffer;
  times(&buffer);

  double real = clock() / (double)CLOCKS_PER_SEC;
  double user = buffer.tms_utime / (double)CLOCKS_PER_SEC;
  double sys = buffer.tms_stime / (double)CLOCKS_PER_SEC;
  if(strcmp(info, "--") != 0 ){
    printf("%s:\n", info);
    printf("Times: real = %f, user = %f, system = %f\n", real - real_previous, user - user_previous, sys - system_previus);
    printf("Times from start: real = %f, user = %f, system = %f\n\n", real - real_start, user - user_start, sys - system_start);
  }
  real_previous = real;
  user_previous = user;
  system_previus = sys;
}

int main(){
  contact_t** contacts = generate_data(1000);

  init_time();
  bst_tree_t* addressbook_tree = create_tree(1);
  get_and_print_time("Stworzenie ksiazki adresowe - drzewo bst");

  for(int i = 0; i < 1000; i++){
    add_bst_node(addressbook_tree, create_bst_node(contacts[i]));
    get_and_print_time("Dodatenie kontaktu do ksiazki adresowej");
  }

  addressbook_tree = reorder_addres_book(addressbook_tree, 2);
  get_and_print_time("Przebudowaie ksiazki adresowej");

  bst_node_t* root;
  char* query = addressbook_tree->tree->data->data[2];
  get_and_print_time("--");
  root = find_bst_tree(addressbook_tree, query);
  get_and_print_time("Wyszukanie w ksiazce adresowej - przypadek optymistyczny");

  root = bst_min(addressbook_tree->tree);
  query = root->data->data[2];
  get_and_print_time("--");
  root = find_bst_tree(addressbook_tree, query);
  get_and_print_time("Wyszukanie w ksiazce adresowej - przypadek pesymistyczny");

  query = addressbook_tree->tree->data->data[2];
  get_and_print_time("--");
  bst_find_and_remove(addressbook_tree, query);
  get_and_print_time("Wyszukanie i usuwanie w ksiazce adresowej - przypadek optymistyczny");

  root = bst_min(addressbook_tree->tree);
  query = addressbook_tree->tree->data->data[2];
  get_and_print_time("--");
  bst_find_and_remove(addressbook_tree, query);
  get_and_print_time("Wyszukanie i usuwanie w ksiazce adresowej - przypadek pesymistyczny");

  remove_address_book_tree(addressbook_tree);
  get_and_print_time("Usuniecie ksiazki");

//-----------------

  contacts = generate_data(1000);
  list_t* addressbook_list = create_list(1);
  get_and_print_time("Stworzenie ksiazki adresowe - lista");

  for(int i = 0; i < 1000; i++){
    add_contact_to_address_book_list(addressbook_list, contacts[i]);
    get_and_print_time("Dodatenie kontaktu do ksiazki adresowej");
  }

  addressbook_list = sort_list_by(addressbook_list, 2);
  get_and_print_time("Sortowanie ksiazki adresowej");

  node_t* node;
  query = addressbook_list->first->next->data->data[2];
  get_and_print_time("--");
  node = find_contact_list(addressbook_list, query, 2);
  get_and_print_time("Wyszukanie w ksiazce adresowej - przypadek optymistyczny");


  query = addressbook_list->last->prev->data->data[2];
  get_and_print_time("--");
  node = find_contact_list(addressbook_list, query, 2);
  get_and_print_time("Wyszukanie w ksiazce adresowej - przypadek pesymistyczny");

  query = addressbook_list->first->next->data->data[2];
  get_and_print_time("--");
  find_and_remove(addressbook_list, query, 2);
  get_and_print_time("Wyszukanie i usuwanie w ksiazce adresowej - przypadek optymistyczny");

  query = addressbook_list->last->prev->data->data[2];
  get_and_print_time("--");
  find_and_remove(addressbook_list, query, 2);
  get_and_print_time("Wyszukanie i usuwanie w ksiazce adresowej - przypadek pesymistyczny");

  query = node->data->data[2];

  remove_list(addressbook_list);
  get_and_print_time("Usuniecie ksiazki");


  return 0;
}
