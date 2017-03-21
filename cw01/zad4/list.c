#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "list.h"

list_t* create_list(){
  list_t* list = (list_t*)calloc(1, sizeof(list));
  node_t* node = (node_t*)calloc(1, sizeof(node_t));
  list->first = node;
  /*
  node = (node_t*)calloc(1, sizeof(node_t));
  Zakomentowanie powoduje ze do firsta i lasta, zostal przypisany ten sam node.
  Co w kolejnym krokach powoduje ze ten node->next = node->prev = NULL;
  Co powoduje przy dodawaniu, dodajemy z uwzgledenieniem straznikow, i
  odwolujemy sie do nie istniejacego elemetnu

  Dodawnie elementu(tutaj wystapil blad):
  void add_node(list_t* list, node_t* node){
    node->next = list->last;
    node->prev = list->last->prev;
    list->last->prev->next = node;
                  /\
                  |
                  w tym miejscu odwolujemy sie do NULL-a
    list->last->prev = node;
  }
*/
  list->last = node;

  list->first->next = list->last;
  list->last->prev = list->first;
  list->first->prev = list->last->next = NULL;
  return list;
}

node_t* create_node(contact_t* contact){
  node_t* node = (node_t*)calloc(1, sizeof(node_t));
  node->data = contact;
  node->next = node->prev = NULL;
  return node;
}

void add_node(list_t* list, node_t* node){
  node->next = list->last;
  node->prev = list->last->prev;
  list->last->prev->next = node;
  list->last->prev = node;
}

void add_contact_to_address_book_list(list_t* list, contact_t* contact){
    if(list) add_node(list, create_node(contact));
}

void remove_node(node_t* node){
  if(node->next) node->prev->next = node->next;
  if(node->next) node->next->prev = node->prev;
  delete_contact(node->data);
  free(node);
}

list_t* remove_list(list_t* list){
  if(list != NULL){
    node_t* next;
    node_t* current = list->first->next;
    while(current->next){
      next = current->next;
      delete_contact(current->data);
      free(current);
      current = next;
    }
    free(list->first);
    free(list->last);
    free(list);
  }
  return NULL;
}

node_t* find_contact_list(list_t* list, char* query, int search_by){
  if(search_by == 1 || search_by == 2 || search_by == 3 || search_by == 5){
    node_t* node = list->first->next;
    while(node->next){
      if(strcmp(node->data->data[search_by], query) == 0) return node;
      node = node->next;
    }
    return NULL;
  }
  else{
    printf("Wrong argument. You can use 1, 2, 3, 5.\n");
    return NULL;
  }
}

node_t* find_min(node_t* node, int search_by){
  node_t* result;
  if(node->next){
    result = node;
  }
  else{
    node = NULL;
  }
  while(node->next){
    if(strcmp(node->data->data[search_by], result->data->data[search_by]) < 0){
      result = node;
    }
    node = node->next;
  }
  return result;
}

list_t* sort_list_by(list_t* list, int order_by){
  if(order_by == 1 || order_by == 2 || order_by == 3 || order_by == 5){
    list_t* sorted = create_list();
    while(list->first->next != list->last){
      node_t* node = find_min(list->first->next, order_by);
      node->prev->next = node->next;
      node->next->prev = node->prev;
      add_node(sorted, node);
    }
    free(list->first);
    free(list->last);
    free(list);
    return(sorted);
  }
  else{
    printf("Wrong argument. You can use 1, 2, 3, 5.\n");
    return NULL;
  }
}


void find_and_remove(list_t* list, char* query, int search_by){
  node_t* node = find_contact_list(list, query, search_by);
  if(node) remove_node(node);
}

void print_node(node_t* node){
  print_contact(node->data);
  printf("\n");
}

void print_list(node_t* node){
  if(node->next){
    print_node(node);
    print_list(node->next);
  }
}

void find_and_print(list_t* list, char* query, int search_by){
  node_t* node = find_contact_list(list, query, search_by);
  if(node) print_node(node);
}
