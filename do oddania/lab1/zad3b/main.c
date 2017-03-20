#include <time.h>
#include <sys/times.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dlfcn.h>
#include "main.h"

double real_start;
double real_previous;
double user_start;
double user_previous;
double system_start;
double system_previus;
result_t** results;
int result_i;



int main(){
  void* handle;
  #ifdef DLL

  typedef struct contact_t{
    char *data[6];
  }contact_t;

  typedef struct node_t{
    struct node_t* next;
    struct node_t* prev;
    contact_t *data;
  }node_t;

  typedef struct list_t{
    struct node_t* first;
    struct node_t* last;
  }list_t;
  typedef struct bst_node_t{
    struct bst_node_t *parent;
    struct bst_node_t *left;
    struct bst_node_t *right;
    contact_t *data;
  }bst_node_t;

  typedef struct bst_tree_t{
    struct bst_node_t *tree;
    int order_by;
  }bst_tree_t;
  handle = dlopen("./libad.so", RTLD_NOW);
  if(handle == NULL){
    printf("Nie udalo sie zaladowac biblioteki\n");
  }

  void (*print_contact)(contact_t*) = dlsym(handle, "print_contact");
  void (*delete_contact)(contact_t*)= dlsym(handle, "delete_contact");

  int (*depth)(bst_node_t*) = dlsym(handle, "depth");
  bst_node_t* (*create_bst_node)(contact_t*) = dlsym(handle, "create_bst_node");
  bst_tree_t* (*create_tree)(int) = dlsym(handle, "create_tree");
  void (*add_bst_node_recusive)(bst_node_t*, bst_node_t*, int) = dlsym(handle, "add_bst_node_recusive");
  void (*add_bst_node)(bst_tree_t*, bst_node_t*) = dlsym(handle, "add_bst_node");
  void (*add_contact_to_address_book_tree)(bst_tree_t*, contact_t*) = dlsym(handle, "add_contact_to_address_book_tree");
  void (*remove_bst_tree)(bst_node_t*) = dlsym(handle, "remove_bst_tree");
  void (*remove_address_book_tree)(bst_tree_t*) = dlsym(handle, "remove_address_book_tree");
  bst_node_t* (*find_bst_node)(bst_node_t*, char*, int) = dlsym(handle, "find_bst_node");
  bst_node_t* (*find_bst_tree)(bst_tree_t*, char*) = dlsym(handle, "find_bst_tree");
  bst_node_t* (*bst_min)(bst_node_t*) = dlsym(handle, "bst_min");
  bst_node_t* (*bst_successor)(bst_node_t*)= dlsym(handle, "bst_successor");
  void (*bst_remove_node)(bst_node_t*) = dlsym(handle, "remove_node");
  void (*bst_find_and_remove)(bst_tree_t*, char*) = dlsym(handle, "bst_find_and_remove");
  void (*reorder_addres_book_recursive)(bst_tree_t*, bst_node_t*) = dlsym(handle, "reorder_addres_book_recursive");
  bst_tree_t* (*reorder_addres_book)(bst_tree_t*, int) = dlsym(handle, "reorder_addres_book");
  void (*print_bst_node)(bst_node_t*) = dlsym(handle, "print_bst_node");
  void (*find_contact_bst_tree)(bst_tree_t*, char*) = dlsym(handle, "find_contact_bst_tree");
  void (*print_tree)(bst_node_t*) = dlsym(handle, "print_tree");

  list_t* (*create_list)() = dlsym(handle, "create_list");
  node_t* (*create_node)(contact_t*) = dlsym(handle, "create_node");
  void (*add_node)(list_t*, node_t*) = dlsym(handle, "add_node");
  void (*add_contact_to_address_book_list)(list_t*, contact_t*) = dlsym(handle, "add_contact_to_address_book_list");
  void (*remove_node)(node_t*) = dlsym(handle, "remove_node");
  list_t* (*remove_list)(list_t*) = dlsym(handle, "remove_list");
  node_t* (*find_contact_list)(list_t*, char* query, int search_by) = dlsym(handle, "find_contact_list");
  node_t* (*find_min)(node_t*, int) = dlsym(handle, "find_min");
  list_t* (*sort_list_by)(list_t*, int) = dlsym(handle, "sort_list_by");
  void (*find_and_remove)(list_t*, char*, int) = dlsym(handle, "find_and_remove");
  void (*print_node)(node_t*) = dlsym(handle, "print_node");
  void (*print_list)(node_t*) = dlsym(handle, "print_list");
  void (*find_and_print)(list_t*, char*, int) = dlsym(handle, "find_and_print");
  #endif

  results = (result_t**)calloc(2100, sizeof(result_t*));
  result_i = 0;
  contact_t** contacts = generate_data(1000, handle);
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

  contacts = generate_data(1000, handle);
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

  save_results();

  #ifdef DLL
  dlclose(handle);
  #endif
  return 0;
}

contact_t** generate_data(int max, void* handle){
  #ifdef DLL
  contact_t* (*create_contact)(char*, char*, char*, char*, char*, char*) = dlsym(handle, "create_contact");
  #endif
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
    result_t* res = (result_t*)calloc(1, sizeof(result_t));
    res->info = (char*)calloc(strlen(info), sizeof(char));
    memcpy(res->info, info, strlen(info));
    res->data[0] = real - real_previous;
    res->data[1] = user - user_previous;
    res->data[2] = sys - system_previus;
    res->data[3] = real - real_start;
    res->data[4] = user - user_start;
    res->data[5] = sys - system_start;
    results[result_i] = res;
    result_i++;
  }
  real_previous = real;
  user_previous = user;
  system_previus = sys;
}

void save_results(){
  FILE *f = fopen("results.txt", "w");
  if (f == NULL)
  {
      printf("Error opening file!\n");
      exit(1);
  }
  for(int i = 0; i < result_i; i++){
    fprintf(f, "%s:\n", results[i]->info);
    fprintf(f, "Times: real = %f, user = %f, system = %f\n", results[i]->data[0], results[i]->data[1], results[i]->data[2]);
    fprintf(f, "Times from start: real = %f, user = %f, system = %f\n\n", results[i]->data[3], results[i]->data[4], results[i]->data[5]);

    free(results[i]->info);
    free(results[i]);
  }
  free(results);
  fclose(f);
}
